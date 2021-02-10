#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lib/protocol.h"
#include "lib/common.h"
#include "lib/IO.h"
#include "server/database.h"

static db_info_t *info;

int db_init(db_info_t *db_info)
{
    if (mysql_library_init(0, NULL, NULL))
    {
        log_error("mysql library init failed", 0);
        return ERROR;
    }
    info = db_info;
    return SUCCESS;
}

void db_create_info(db_info_t *db_info,
                    const char *db_name,
                    const char *host, const char *user,
                    const char *pw,
                    const unsigned long flags)
{
    strncpy(db_info->user, user, DB_INFO_STR_LEN);
    strncpy(db_info->pw, pw, DB_INFO_STR_LEN);
    strncpy(db_info->db_name, db_name, DB_INFO_STR_LEN);
    strncpy(db_info->host, host, DB_INFO_STR_LEN);
    db_info->flags = flags;
}

MYSQL *db_open()
{
    MYSQL *db = mysql_init(NULL);
    mysql_real_connect(db, info->host, info->user, info->pw,
                       info->db_name, 0, NULL, info->flags);
    if (db == NULL)
    {
        log_error(mysql_error(db), SYS_ERROR);
        mysql_close(db);
    }
    return db;
}

// TODO: Make this generic
int db_insert_user(MYSQL *db, user_model_t *model)
{
    char query[SQL_SIZE];
    char escaped_uname[escape_len(MODEL_USER_NAME_LEN)];

    mysql_real_escape_string(db, escaped_uname, model->username,
                             strlen(model->username));

    snprintf(query, SQL_SIZE, "INSERT INTO user (username) VALUES('%s')",
             escaped_uname);

    if (mysql_real_query(db, query, strlen(query)))
    {
        log_error(mysql_error(db), 0);
        return ERROR;
    }
    return mysql_affected_rows(db);
}

void db_deserialize_tasks(MYSQL_RES *db_results,
                          void *real_results,
                          int fcount,
                          char **fnames)
{
    linked_node_t **head = (linked_node_t **)real_results;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(db_results)))
    {
        linked_node_t *node = calloc(1, sizeof(linked_node_t));
        task_model_t *model = calloc(1, sizeof(task_model_t));
        for (int i = 0; i < fcount; i++)
        {
            if (strcmp(fnames[i], "id") == 0)
            {
                model->id = atoi(row[i]);
            }
            else if (strcmp(fnames[i], "user_id") == 0)
            {
                model->user_id = atoi(row[i]);
            }
            else if (strcmp(fnames[i], "content") == 0)
            {
                strncpy(model->content, row[i], MODEL_TASK_CNT_LEN);
            }
        }
        node->value = model;
        ll_link(head, node);
    }
}

void db_deserialize_users(MYSQL_RES *db_results,
                          void *real_results,
                          int fcount,
                          char **fnames)
{
    linked_node_t **head = (linked_node_t **)real_results;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(db_results)))
    {
        linked_node_t *node = calloc(1, sizeof(linked_node_t));
        user_model_t *model = calloc(1, sizeof(user_model_t));
        for (int i = 0; i < fcount; i++)
        {
            if (strcmp(fnames[i], "id") == 0)
            {
                model->id = atoi(row[i]);
            }
            else if (strcmp(fnames[i], "username") == 0)
            {
                stpncpy(model->username, row[i], MODEL_USER_NAME_LEN);
            }
        }
        node->value = model;
        ll_link(head, node);
    }
}

int db_delete(MYSQL *db, const char *sql, const int argc, ...)
{
    char query[SQL_SIZE];
    char real_query[escape_len(SQL_SIZE)];
    va_list argv;
    va_start(argv, argc);
    vsnprintf(query, SQL_SIZE, sql, argv);
    mysql_real_escape_string(db, real_query, query, strlen(query));

    if (mysql_real_query(db, real_query, strlen(real_query)))
    {
        log_error(mysql_error(db), 0);
        return ERROR;
    }
    va_end(argv);
    int affected_rows = mysql_affected_rows(db);
    return affected_rows;
}

void replace_format_with_s(int argc, char *format_str)
{
    int counter = 0;
    char curr;
    while ((curr = *(format_str++)))
    {
        if (curr == '%')
        {
            curr = *(format_str);
            if (curr == 'd' || curr == 's')
            {
                *format_str = 's';
                if (++counter == argc)
                    return;
            }
        }
    }
}

void argv_to_strs(va_list argv, int argc, char *format_str,
                  char escaped[][64], int mlen)
{
    int index = 0;
    char *curr;
    while (*(curr = format_str++))
    {
        if (*curr == '%')
        {
            curr = format_str++;
            if (*curr == 'd')
            {
                snprintf(escaped[index++],
                         mlen, "%d", va_arg(argv, int));
                *curr = 's';
            }
            else if (*curr == 's')
            {
                snprintf(escaped[index++], mlen,
                         "%s", va_arg(argv, char *));
            }
        }
    }
}

void escape_sql_args(MYSQL *db, char *dest, size_t max_size,
                     char *format, char arr[][64])
{
    char *curr;
    while (*(curr = format++))
    {
        if (*curr == '%' && *(curr + 1) == 's')
        {
            format++;
            char *next_str = *arr++;
            size_t next_str_len = strlen(next_str);
            int escaped_len = mysql_real_escape_string(db, dest,
                                                       next_str,
                                                       next_str_len);
            dest += escaped_len;
        }
        else
        {
            strncpy(dest++, curr, 1);
        }
    }
    *dest = 0;
}

int db_select(MYSQL *db,
              void (*deserialize_cb)(MYSQL_RES *, void *, int, char **),
              void *results,
              char *sql,
              const int argc,
              ...)
{
    char escaped_sql[escape_len(SQL_SIZE)];
    va_list argv;
    va_start(argv, argc);

    int sql_format_len = strlen(sql) + 1;
    char *sql_cpy = calloc(sql_format_len, sizeof(char));
    strncpy(sql_cpy, sql, sql_format_len);

    // We need to convert variadic args to strings for
    // SQL escape. Also change all occurences of %d in
    // SQL format string to %s.
    char escaped_args[argc][64];
    argv_to_strs(argv, argc, sql_cpy, escaped_args, 64);
    // Create a final escaped SQL string from variadic arguments.
    escape_sql_args(db, escaped_sql, SQL_SIZE, sql_cpy, escaped_args);

    if (mysql_real_query(db, escaped_sql, strlen(escaped_sql)))
    {
        log_error(mysql_error(db), 0);
        return ERROR;
    }

    MYSQL_RES *result = mysql_store_result(db);
    if (result == NULL)
    {
        log_error(mysql_error(db), 0);
        return ERROR;
    }
    int fcount = mysql_num_fields(result);
    if (fcount > 0)
    {
        // Field names
        char **fnames = malloc(sizeof(char *) * fcount);
        for (int i = 0; i < fcount; i++)
            fnames[i] = mysql_fetch_field(result)->name;

        deserialize_cb(result, results, fcount, fnames);
        safe_free((void **)&fnames);
        mysql_free_result(result);
    }
    sfree(sql_cpy);
    va_end(argv);
    return fcount;
}

void db_thread_quit()
{
    mysql_thread_end();
}
