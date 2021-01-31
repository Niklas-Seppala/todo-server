#ifndef THREADS_H
#define THREADS_H
#include <pthread.h>

#define EXECUTE_CLEANUP 1
#define THREAD_READ_BUFF_SIZE 512
#define POOL_SIZE 15

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
int init_threads(int poolsize, thread_routine_t routine, void *arg);

/**
 * @brief Calls for all client handling threads in the
 *        threadpool to finish work and die.
 */
void shutdown_client_threads();

/**
 * @brief 
 * 
 * @param arg 
 */
void thread_cleanup(void *arg);

#endif // !THREADS_H