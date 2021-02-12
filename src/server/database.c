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

void db_free_serialized_model(ser_model_t *model, int size)
{
    char **temp = *model;
    for (int i = 0; i < size; i++)
    {
        if (temp[i] != NULL)
        {
            free(temp[i]);
        }
    }
    sfree(*model);
}

void serialize_task_model(task_model_t *task,
                          int *field, int len,
                          ser_model_t *serialized)
{
    *serialized = calloc(MODEL_TASK_FCOUNT, sizeof(char *));
    char *id, *userid, *content;
    for (int i = 0; i < len; i++, field++)
    {
        switch (*field)
        {
        case TASK_ID_FIELD:
            id = calloc(64, sizeof(char));
            snprintf(id, 64, "%d", task->id);
            (*serialized)[USER_ID_FIELD] = id;
            break;
        case TASK_USERID_FIELD:
            userid = calloc(64, sizeof(char));
            snprintf(userid, 64, "%d", task->user_id);
            (*serialized)[TASK_USERID_FIELD] = userid;
            break;
        case TASK_CONTENT_FIELD:
            content = calloc(MODEL_TASK_CNT_LEN, sizeof(char));
            snprintf(content, MODEL_TASK_CNT_LEN, "%s", task->content);
            (*serialized)[TASK_CONTENT_FIELD] = content;
            break;
        default:
            break;
        }
    }
}

void serialize_user_model(user_model_t *user,
                          int *field, int len,
                          ser_model_t *fields)
{
    *fields = calloc(MODEL_USER_FCOUNT, sizeof(char *));
    char *id, *username;
    for (int i = 0; i < len; i++, field++)
    {
        switch (*field)
        {
        case USER_ID_FIELD:
            id = calloc(64, sizeof(char));
            snprintf(id, 64, "%d", user->id);
            (*fields)[USER_ID_FIELD] = id;
            break;
        case USER_USRNAME_FIELD:
            username = calloc(MODEL_USER_NAME_LEN, sizeof(char));
            snprintf(username, MODEL_USER_NAME_LEN, "%s", user->username);
            (*fields)[USER_USRNAME_FIELD] = username;
            break;
        default:
            break;
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

void escape_model(MYSQL *db, char *dest, size_t max_size,
                  char *format, ser_model_t model)
{
    char *curr;
    while (*(curr = format++))
    {
        if (*curr == '%' && *(curr + 1) == 's')
        {
            format++;
            char *next_str = *model++;
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
    *dest = 0; // NULL terminate string
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
    *dest = 0; // NULL terminate string
}

char *db_escape_varargs(MYSQL *db, const char *sql,
                        int argc, va_list argv)
{
    char *escaped_sql = calloc(1, escape_len(SQL_SIZE));
    int sql_format_len = strlen(sql) + 1;
    char sql_cpy[sql_format_len];
    strncpy(sql_cpy, sql, sql_format_len);

    // We need to convert variadic args to strings for
    // SQL escape. Also change all occurences of %d in
    // SQL format string to %s.
    char escaped_args[argc][64];
    argv_to_strs(argv, argc, sql_cpy, escaped_args, 64);
    // Create a final escaped SQL string from variadic arguments.
    escape_sql_args(db, escaped_sql, SQL_SIZE,
                    sql_cpy, escaped_args);
    return escaped_sql;
}

int db_insert(MYSQL *db, char *sql, int argc, ...)
{
    va_list argv;
    va_start(argv, argc);
    char *escaped_sql = db_escape_varargs(db, sql, argc, argv);
    if (mysql_real_query(db, escaped_sql, strlen(escaped_sql)))
    {
        log_error(mysql_error(db), 0);
        sfree(escaped_sql);
        return ERROR;
    }
    sfree(escaped_sql);
    va_end(argv);
    return mysql_affected_rows(db);
}

int select_static(MYSQL *db,
                  void (*deserialize_cb)(MYSQL_RES *, void *, int, char **),
                  void *results,
                  char *sql,
                  const int argc,
                  ser_model_t model)
{
    char escaped_sql[escape_len(SQL_SIZE)];
    escape_model(db, escaped_sql, escape_len(SQL_SIZE), sql, model);
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
    int rows = mysql_num_rows(result);
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
    return rows;
}

int db_select(MYSQL *db,
              void (*deserialize_cb)(MYSQL_RES *, void *, int, char **),
              void *results,
              char *sql,
              const int argc,
              ...)
{
    va_list argv;
    va_start(argv, argc);
    char *escaped_sql = db_escape_varargs(db, sql, argc, argv);
    if (mysql_real_query(db, escaped_sql, strlen(escaped_sql)))
    {
        log_error(mysql_error(db), 0);
        sfree(escaped_sql);
        return ERROR;
    }

    MYSQL_RES *result = mysql_store_result(db);
    if (result == NULL)
    {
        log_error(mysql_error(db), 0);
        sfree(escaped_sql);
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
    sfree(escaped_sql);
    va_end(argv);
    return fcount;
}

int db_delete(MYSQL *db, const char *sql, const int argc, ...)
{
    va_list argv;
    va_start(argv, argc);
    char *escaped_sql = db_escape_varargs(db, sql, argc, argv);
    if (mysql_real_query(db, escaped_sql, strlen(escaped_sql)))
    {
        // log_error(mysql_error(db), 0);
        printf("%s\n", mysql_error(db));
        sfree(escaped_sql);
        return ERROR;
    }
    va_end(argv);
    int affected_rows = mysql_affected_rows(db);
    sfree(escaped_sql);
    return affected_rows;
}

void db_close(MYSQL *db)
{
    mysql_close(db);
}

void db_thread_quit()
{
    mysql_thread_end();
}
