#include <stdlib.h>
#include <string.h>

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

void db_create_info(db_info_t *db_info, const char *user,
    const char *db_name, const char *host, const char *pw,
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
    MYSQL * db = mysql_init(NULL);
    mysql_real_connect(db, info->host, info->user, info->pw,
        info->db_name, 0, NULL, info->flags
    );

    if (!db) {
        log_error(mysql_error(db), SYS_ERROR);
        mysql_close(db);
    }

    return db;
}

void db_thread_quit()
{
    mysql_thread_end();
}

void db_close(MYSQL *db)
{
    mysql_close(db);
}
