#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

/**
 * @brief 
 */
#define STATIC_BUFF_SIZE 512
/**
 * @brief 
 */
#define EXECUTE_CLEANUP 1
/**
 * @brief 
 */
#define POOL_SIZE 15


pthread_cond_t idle;

pthread_t threadpool[POOL_SIZE];

/**
 * @brief Thread startup function pointer.
 */
typedef void *(*thread_routine_t)(void *);

/**
 * @brief Fills threadpool with threads that
 *        execute specified routine.
 * 
 * @param routine start function
 * @param arg routine arg
 */
void init_threads(thread_routine_t routine, void *arg);

/**
 * @brief Calls for all client handling threads in the
 *        threadpool to finish work and die.
 */
void shutdown_client_threads(void);

void thread_cleanup(void *arg);

/**
 * @brief Client handler thread start routine. Threads run
 *        forever in a loop, handling client requestsm unless
 *        they are cancelled by invoking shutdown_client_threads().
 * 
 * @param args
 * @return void* NULL
 */
void *delegate_conn(void *args);

#endif // !THREADS_H