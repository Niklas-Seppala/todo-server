#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>

#define DB_INFO_STR_LEN 32

typedef struct db_info {
    char host[DB_INFO_STR_LEN];
    char user[DB_INFO_STR_LEN];
    char pw[DB_INFO_STR_LEN];
    char db_name[DB_INFO_STR_LEN];
    unsigned long flags;
} db_info_t;


void db_create_info(db_info_t *db_info, const char *user,
    const char *db_name, const char *host, const char *pw,
    const unsigned long flags);

int db_init(db_info_t *db_info);
void db_thread_quit();
void db_close(MYSQL *db);

MYSQL *db_open();

#endif // DATABASE_H
