#include <stdlib.h>
#include <unistd.h>
#include "lib/common.h"
#include "server/buffpool.h"

void drain_pool(struct buffer_node **pool)
{
    struct buffer_node* head = *pool;
    pool = NULL;
    while (head != NULL)
    {
        void* this = head;
        head = head->next;

        safe_free((void **)&this);
    }
}

void free_buffer(struct buffer_node *buffer) {
    buffer->state = FREE;
}

struct buffer_node *get_free_buffer(struct buffer_node *pool) {
    if (!pool)
        return NULL;

    struct buffer_node *head = pool;
    while (head->state != FREE)
    {
        head = head->next;
        if (head == NULL)
            head = pool;
    }
    head->state = INUSE;
    return head;
}

int init_pool(struct buffer_node **pool, const size_t depth)
{
    safe_free((void **)pool);

    size_t node_size = sizeof(struct buffer_node);
    *pool = calloc(1, node_size);
    (*pool)->state = FREE;

    struct buffer_node *node = *pool;
    for (int i = 0; i < depth-1; i++) {
        node->next = calloc(1, node_size);
        if (!node->next)
            return ERROR;

        node->state = FREE;
        node = node->next;
    }
    return SUCCESS;
}
