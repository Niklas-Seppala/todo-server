#include <stdlib.h>
#include <pthread.h>

#include "server/queue.h"

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void work_enqueue(server_work_t **queue, struct server_conn *conn,
    uint64_t id)
{
    server_work_t *new_node = malloc(sizeof(server_work_t));
    new_node->next = NULL;
    new_node->connection = conn;
    new_node->id = id;

    pthread_mutex_lock(&queue_mutex);

    if (*queue == NULL) {
        *queue = new_node;
        pthread_mutex_unlock(&queue_mutex);
        return;
    }

    server_work_t *tail = *queue;
    while (tail->next != NULL) {
        tail = tail->next;
    }

    tail->next = new_node;
    pthread_mutex_unlock(&queue_mutex);
}

server_work_t *conn_dequeue(server_work_t **queue)
{
    pthread_mutex_lock(&queue_mutex);

    if (*queue == NULL) {
        pthread_mutex_unlock(&queue_mutex);
        return NULL;
    }

    server_work_t *popped = *queue;
    *queue = (*queue)->next;

    pthread_mutex_unlock(&queue_mutex);
    return popped;
}
