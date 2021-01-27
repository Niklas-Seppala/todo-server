#include <stdlib.h>
#include <assert.h>
#include "lib/common.h"

void safe_free(void **ptr)
{
    assert(ptr);
    if (ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}