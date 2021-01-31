#include <pthread.h>
#include <unistd.h>

#include "lib/common.h"
#include "lib/network.h"
#include "lib/protocol.h"
#include "server/queue.h"
#include "server/threads.h"
#include "server/network.h"

static pthread_t *threadpool;
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

int init_threads(int poolsize, thread_routine_t routine, void *arg)
{
    if (poolsize < 0) {
        log_error("Invalid poolsize", 0);
        return ERROR;
    }

    threadpool = malloc(poolsize * sizeof(pthread_t));

    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_create(&threadpool[i], NULL, routine, arg);
    }
    return SUCCESS;
}

void shutdown_client_threads()
{
    for (size_t i = 0; i < POOL_SIZE; i++) {
        pthread_join(threadpool[i], NULL);
    }
}
