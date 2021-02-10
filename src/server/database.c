#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lib/protocol.h"
#include "lib/common.h"
#include "lib/IO.h"
#include "server/database.h"

db_info_t *info;
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

void db_create_info(db_info_t *db_info, const char *db_name,
                    const char *host, const char *user, const char *pw,
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

    if (!db)
    {
        log_error(mysql_error(db), SYS_ERROR);
        mysql_close(db);
    }
    return db;
}

int db_insert_user(MYSQL *db, user_model_t *model)
{
    char query[SQL_SIZE];
    char escaped_uname[escape_len(USERNAME_LEN)];

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

void deserialize_users(MYSQL_RES *db_results,
                       void *real_results,
                       int fcount,
                       char **fnames)
{
    user_model_node_t **head = (user_model_node_t **)real_results;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(db_results)))
    {
        user_model_node_t *node = calloc(1, sizeof(user_model_node_t));
        user_model_t *model = calloc(1, sizeof(user_model_t));
        for (int i = 0; i < fcount; i++)
        {
            if (strcmp(fnames[i], "id") == 0)
            {
                model->id = atoi(row[i]);
            }
            else if (strcmp(fnames[i], "username") == 0)
            {
                stpncpy(model->username, row[i], USERNAME_LEN);
            }
        }
        node->model = model;
        node->next = NULL;

        *head = (user_model_node_t *)link_node(head, node);
    }
}

int db_select(MYSQL *db,
              void (*deserialize_cb)(MYSQL_RES *, void *, int, char **),
              void *results,
              const char *sql,
              const int argc,
              ...)
{
    char query[SQL_SIZE];
    char real_query[escape_len(SQL_SIZE)];
    va_list argv;
    va_start(argv, argc);
    vsnprintf(query, SQL_SIZE, sql, argv);
    mysql_real_escape_string(db, real_query, query, strlen(query));

    if (mysql_real_query(db, query, strlen(query)))
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
        char **fnames = malloc(sizeof(char *) * fcount); // multiply by field count
        for (int i = 0; i < fcount; i++)
            fnames[i] = mysql_fetch_field(result)->name;

        deserialize_cb(result, results, fcount, fnames);
        safe_free((void **)&fnames);
        mysql_free_result(result);
        va_end(argv);
    }
    return fcount;
}

void db_thread_quit()
{
    mysql_thread_end();
}
