/**
 * @file buffpool.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 */


#if !defined(BUFFPOOL_H)
#define BUFFPOOL_H
#include <stdlib.h>

#define STATIC_BUFF_SIZE 512
#define FREE 1
#define INUSE -1

struct buffer_node {
    struct buffer_node* next;
    int state;
    char buffer[STATIC_BUFF_SIZE];
};

/**
 * @brief 
 * 
 * @param buffer 
 */
void free_buffer(struct buffer_node *buffer);

/**
 * @brief Get the free buffer object
 * 
 * @param pool 
 * @return struct buffer_node* 
 */
struct buffer_node *get_free_buffer(struct buffer_node *pool);

/**
 * @brief 
 * 
 * @param pool 
 * @param depth 
 */
int init_pool(struct buffer_node **pool, const size_t depth);

/**
 * @brief 
 * 
 * @param pool 
 */
void drain_pool(struct buffer_node* pool);

#endif // THREADS_H
