#if !defined COMMON_H
#define COMMON_H

#define ERROR -1
#define SUCCESS 1
#define FATAL 1

#define sfree(ptr) safe_free((void **)&ptr)

typedef struct linked_node {
    struct linked_node *next;
} linked_node_t;

linked_node_t *ll_link(linked_node_t **list, linked_node_t *item);

/**
 * @brief 
 * 
 */
#define link_node(list, item) \
    ll_link((linked_node_t **)list, (linked_node_t *)item)

/**
 * @brief Prevents dangling pointers
 *        and double free.
 * 
 * @param ptr pointer to be freed
 */
void safe_free(void **ptr);

#endif
