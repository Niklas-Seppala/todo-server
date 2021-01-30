#include <pthread.h>
#include <unistd.h>

#include "lib/common.h"
#include "lib/network.h"
#include "lib/protocol.h"
#include "server/queue.h"
#include "server/threads.h"
#include "server/network.h"

pthread_t threadpool[POOL_SIZE];

/**
 * @brief Flag that signals client threads to die.
 */
static volatile int thread_exit_flag = 0;

pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t idle_lock = PTHREAD_MUTEX_INITIALIZER;

void thread_cleanup(void *arg)
{
    server_work_t *work = (server_work_t *)arg;
    if (work) {
        if (work->connection->sock > 0) {
            close(work->connection->sock);
        }
        sfree(work->connection);
        sfree(work);
    }
}

void init_threads(thread_routine_t routine, void *arg)
{
    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_create(&threadpool[i], NULL, routine, arg);
    }
}

void shutdown_client_threads(void)
{
    thread_exit_flag = 1;
    pthread_cond_broadcast(&idle);
    for (size_t i = 0; i < POOL_SIZE; i++) {
        pthread_join(threadpool[i], NULL);
    }
}

void *delegate_conn(void *work_queue)
{
    server_work_t **client_queue = (server_work_t **)work_queue;
    char sbuffer[STATIC_BUFF_SIZE];
    for (;;)
    {
        // All threads wait here, untill there
        // main thread pushes work to queue and
        // releases one thread at a time.

        // In case of SIGINT, all threads are released,
        // and they will all handle remaining work in queue
        // and the exit.
        pthread_mutex_lock(&idle_lock);
        pthread_cond_wait(&idle, &idle_lock);
        pthread_mutex_unlock(&idle_lock);

        // Thread pops work from queue.
        server_work_t *work = conn_dequeue(client_queue);

        if (work) {
            pthread_cleanup_push(thread_cleanup, work);

            // TODO: delegate logging IO to single thread, so handling
            //       client requests is smoother.
            vflog_info("Accepted client [%ld] at address: %s:%s",
                work->id, work->connection->readable.ip_addr,
                work->connection->readable.port
            );

            // Read the header
            struct header header_pkg;
            int read_rc = read_socket(work->connection->sock, sbuffer,
                &header_pkg, STATIC_BUFF_SIZE, HEADER_SIZE
            );

            if (read_rc != SUCCESS) {
                if (read_rc & READ_OVERFLOW) {
                    // The protocol dictates that communications
                    // begin with 16-byte size header package.
                    // Here client sent rubish

                    // TODO: Separate IO logging thread
                    log_error("Package overflow while reading socket", 0);

                    // Send error code to client
                    send_code(work->connection->sock, "SERVER", INV);
                }
            } else {
                header_from_nw(&header_pkg);
                pthread_testcancel();            // cancellation point
                handle_request(work->connection, &header_pkg,
                    sbuffer, STATIC_BUFF_SIZE
                );
            }

            vflog_info("Client [%ld] handled.", work->id);
            pthread_cleanup_pop(EXECUTE_CLEANUP);
        }
        // If flag is set, threads will exit and join cleanup
        if (thread_exit_flag) {
            return NULL;
        }
    }
}