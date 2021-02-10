/**
 * @file database.h
 * @author Niklas Seppala
 * @brief Module for todo-server database functions.
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>

// Size of the SQL statement
#define SQL_SIZE 256

// Doubles the size of escaped SQL statment
#define escape_len(init_size) init_size*2

/*******************************************************************/
/********************   Database model structs  ********************/
/*******************************************************************/

#define MODEL_USER_NAME_LEN 15
#define MODEL_TASK_CNT_LEN 2000

/**
 * @brief Holds data from task table
 */
typedef struct task_model
{
    int id;
    int user_id;
    char content[MODEL_TASK_CNT_LEN]; // TODO: decide some length
} task_model_t;

/**
 * @brief Holds data from user table
 */
typedef struct user_model {
    int id;
    char username[MODEL_USER_NAME_LEN];
} user_model_t;


#define DB_INFO_STR_LEN 32
/**
 * @brief Holds data required for database connection creation.
 */
typedef struct db_info {
    char host[DB_INFO_STR_LEN];
    char user[DB_INFO_STR_LEN];
    char pw[DB_INFO_STR_LEN];
    char db_name[DB_INFO_STR_LEN];
    unsigned long flags;
} db_info_t;

/**
 * @brief Copies parameter strings to db_info_t struct.
 * 
 * @param db_info destination struct address.
 * @param db_name database name
 * @param host host
 * @param user username
 * @param pw password
 * @param flags additional flags. @see mysql_real_connect()
 */
void db_create_info(db_info_t *db_info, const char *db_name,
    const char *host, const char *user, const char *pw,
    const unsigned long flags);

/**
 * @brief Initializes application database.
 * 
 * @param db_info Connection information
 * @return int SUCCESS if ok, else ERROR
 */
int db_init(db_info_t *db_info);

/**
 * @brief When quiting thread, call this function to let mysql library
 *        clear internal things up.
 */
void db_thread_quit();

/**
 * @brief Closes MySQL connection and frees allocated memory.
 * 
 * @param db MySQL connection to be closed.
 */
void db_close(MYSQL *db);

/**
 * @brief Initializes and connects to MySQL database. Make sure to
 *        call db_init() before this function. The resulting connection
 *        object must be freed after use with db_close().
 *        ALLOCATES HEAP MEMORY!
 * 
 * @return MYSQL* open MySQL database connection.
 */
MYSQL *db_open();

/**
 * @brief Performs a INSERT SQL statement to set provided user model
 *        to database. Passed databasse connection MUST be initialized
 *        and open.
 * 
 * @param db open MySQL database connection
 * @param model user model to be inserted
 * @return int affected row count
 */
int db_insert_user(MYSQL *db, user_model_t *model);

/**
 * @brief Preforms a SELECT SQL statement to fetch data from database.
 *        Fetched data is aggregated to heap allocated linked list.
 *        Passed database connection MUST be initialized and opened.
 *        ALLOCATES HEAP MEMORY!
 * 
 * @param db open MySQL database connection
 * @param deserialize_cb callback function to handle deserialization
 *                       from MYSQL_RES object to table model
 * @param results linked list head for results
 * @param sql SQL statement format string
 * @param argc count of variadic arguments
 * @param ... variadic args for SQL format string
 * @return int row count
 */
int db_select(MYSQL *db,
              void(*deserialize_cb)(MYSQL_RES *, void *, int, char **),
              void *results,
              const char *sql,
              const int argc,
              ...);


/*******************************************************************/
/**************   DEFAULT DESERIALIZATION CALLBACKS   **************/
/*******************************************************************/

/**
 * @brief Default deserializer callback for user data models.
 * 
 * @param db_results MySQL result set pointer.
 * @param real_results linked list head where result models are gathered.
 * @param fcount field count in result set
 * @param fnames names of the fields in result set.
 */
void db_deserialize_users(MYSQL_RES *db_results,
                       void *real_results,
                       int fcount,
                       char **fnames);

/**
 * @brief Default deserializer callback for task data models.
 * 
 * @param db_results MySQL result set pointer
 * @param real_results linked list head where result models are gathered.
 * @param fcount field count in result set
 * @param fnames name of the fields in result set
 */
void db_deserialize_tasks(MYSQL_RES *db_results,
                       void *real_results,
                       int fcount,
                       char **fnames);

#endif // DATABASE_H
