#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H
#include <inttypes.h>

typedef struct work_node {
    struct work_node *next;
    struct server_conn *connection;
    uint64_t id;
} server_work_t;

/**
 * @brief Creates a server work node from connection
 *        parameter and pushes it to queue. Queue head
 *        can be NULL. ALLOCATES HEAP MEMORY!
 * 
 * @param queue work queue head
 * @param conn client connection object
 * @param id client id
 */
void work_enqueue(server_work_t **queue, struct server_conn *conn,
    uint64_t id);

/**
 * @brief Pops the head of the queue.
 * 
 * @param queue server work queue head
 * @return serverwork_t* Pointer to work object.
 *         Needs to be freed.
 */
server_work_t *conn_dequeue(server_work_t **queue);

#endif // WORK_QUEUE_H