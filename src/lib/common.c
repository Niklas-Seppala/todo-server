#include <stdlib.h>
#include <assert.h>
#include "lib/common.h"

linked_node_t *ll_link(linked_node_t **list, linked_node_t *item)
{
    linked_node_t *head;
    head = *list;

    if (head == NULL || head->next == NULL)
    {
        *list = item;
        return item;
    }

    while (head->next != NULL)
        head = head->next;
    head->next = item;
    return item;
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