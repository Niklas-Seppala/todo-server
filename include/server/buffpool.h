/**
 * @file buffpool.h
 * @author Niklas Seppälä
 * @brief Module that provides pooling buffers. Buffers are
 *        allocated to heap, and linked to list. Don't lose the
 *        head!
 * 
 *        After using the borrowed buffer, it must be returned back
 *        to pool using free_buffer().
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 */

#if !defined(BUFFPOOL_H)
#define BUFFPOOL_H
#include <stdlib.h>

/**
 * @brief Size of the buffers in the pool.
 */
#define STATIC_BUFF_SIZE 512

/**
 * @brief Buffer status code
 */
#define FREE 1
/**
 * @brief Buffer status code
 */
#define INUSE -1

struct buffer_node {
    struct buffer_node* next;
    int state;
    char buffer[STATIC_BUFF_SIZE];
};

/**
 * @brief Return buffer back to pool.
 * 
 * @param buffer buffer
 */
void free_buffer(struct buffer_node *buffer);

/**
 * @brief Get a free buffer from the pool.
 * 
 * @param pool pointer to pool
 * @return struct buffer_node* pointer to free buffer.
 */
struct buffer_node *get_free_buffer(struct buffer_node *pool);

/**
 * @brief Creates the buffer pool. ALLOCATES HEAP MEMORY!
 * 
 * @param pool pointer to pool
 * @param depth buffer count in the pool
 */
int init_pool(struct buffer_node **pool, const size_t depth);

/**
 * @brief Frees all nodes from the pool and sets
 *        the linked list head to NULL.
 * 
 * @param pool buffer pool head
 */
void drain_pool(struct buffer_node **pool);

#endif // THREADS_H
