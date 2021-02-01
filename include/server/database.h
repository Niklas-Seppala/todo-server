#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>

typedef struct db_info {
    char host[32];
    char user[32];
    char pw[32];
    char db_name[32];
    unsigned long flags;
} db_info_t;


int init_db(const db_info_t *db_info);
void db_thread_quit();
void db_close(MYSQL *db);

MYSQL *db_open();

#endif // DATABASE_H
