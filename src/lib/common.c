#include <stdlib.h>
#include <assert.h>
#include "lib/common.h"

linked_node_t *ll_link(linked_node_t **list, linked_node_t *item)
{
    linked_node_t *head;
    head = *list;

    if (head == NULL)
    {
        *list = item;
        return item;
    }
    while (head->next != NULL)
        head = head->next;
    head->next = item;
    return item;
}

void ll_free(linked_node_t **list)
{
    linked_node_t *next_node, *head;
    head = (*list);
    do
    {
        next_node = head->next;  // Store reference to next node
        sfree(head->value);      // Free the node value
        sfree(head);             // Free the node
    } while ((head = next_node));
    *list = NULL;
}

void safe_free(void **ptr)
{
    assert(ptr);
    if (ptr != NULL)
    {
        free(*ptr);
        *ptr = NULL;
    }
}