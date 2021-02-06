#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "lib/protocol.h"
#include "lib/common.h"
#include "lib/IO.h"
#include "server/database.h"

db_info_t *info;

int db_init(db_info_t *db_info)
{
    if (mysql_library_init(0, NULL, NULL)) {
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
        info->db_name, 0, NULL, info->flags
    );

    if (!db) {
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

    snprintf(query, SQL_SIZE, "INSERT INTO user (username) VALUES(%s)",
        escaped_uname);

    if (mysql_real_query(db, query, strlen(query))) {
        log_error(mysql_error(db), 0);
        return ERROR;
    }
    return mysql_affected_rows(db);
}

int db_select_user(MYSQL *db, int id, user_model_t *model)
{
    char query[SQL_SIZE];
    snprintf(query, SQL_SIZE, "SELECT * FROM user WHERE id = %d", id);
    if (mysql_real_query(db, query, strlen(query))) {
        log_error(mysql_error(db), 0);
        return ERROR;
    }

    MYSQL_RES *result = mysql_store_result(db);
    if (result == NULL) {
        log_error(mysql_error(db), 0);
        return ERROR;
    }

    int fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        for (int i = 0; i < fields; i++) {
            MYSQL_FIELD *field = mysql_fetch_field(result);
            if (strcmp(field->name, "username") == 0) {
                stpncpy(model->username, row[i], USERNAME_LEN);
            }
        }
    }
    return SUCCESS;
}

void db_thread_quit()
{
    mysql_thread_end();
}

void db_close(MYSQL *db)
{
    mysql_close(db);
}
