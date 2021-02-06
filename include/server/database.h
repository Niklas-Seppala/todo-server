#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#define DB_INFO_STR_LEN 32
#define SQL_SIZE 256
#define USERNAME_LEN 15

/**
 * @brief 
 * 
 */
#define escape_len(src) src*2

/**
 * @brief 
 * 
 */
typedef struct user_model {
    char username[15];
} user_model_t;

/**
 * @brief 
 * 
 */
typedef struct db_info {
    char host[DB_INFO_STR_LEN];
    char user[DB_INFO_STR_LEN];
    char pw[DB_INFO_STR_LEN];
    char db_name[DB_INFO_STR_LEN];
    unsigned long flags;
} db_info_t;

/**
 * @brief 
 * 
 * @param db_info 
 * @param db_name 
 * @param host 
 * @param user 
 * @param pw 
 * @param flags 
 */
void db_create_info(db_info_t *db_info, const char *db_name,
    const char *host, const char *user, const char *pw,
    const unsigned long flags);

/**
 * @brief 
 * 
 * @param db_info 
 * @return int 
 */
int db_init(db_info_t *db_info);

/**
 * @brief 
 * 
 */
void db_thread_quit();

/**
 * @brief 
 * 
 * @param db 
 */
void db_close(MYSQL *db);

/**
 * @brief 
 * 
 * @return MYSQL* 
 */
MYSQL *db_open();

/**
 * @brief 
 * 
 * @param db 
 * @param model 
 * @return int 
 */
int db_insert_user(MYSQL *db, user_model_t *model);

/**
 * @brief 
 * 
 * @param db 
 * @param id 
 * @param model 
 * @return int 
 */
int db_select_user(MYSQL *db, int id, user_model_t *model);


#endif // DATABASE_H
