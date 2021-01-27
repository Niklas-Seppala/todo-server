#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "lib/common.h"
#include "lib/protocol.h"

static const int CMD_STR_LEN = 3;

int str_to_cmd(char *cmd)
{
    int result = 0;
    for (int i = 0; i < CMD_STR_LEN; i++) {
        result += cmd[i];
    }
    return result;
}

char *cmd_to_str(int cmd)
{
    char * result = malloc(CMD_STR_LEN+1);
    char *str;
    switch (cmd) {
    case ADD:
        str = CMD_ADD;
        break;
    case LOG:
        str = CMD_LOG;
        break;
    case RMV:
        str = CMD_RMV;
        break;
    case VAL:
        str = CMD_VAL;
        break;
    case INV:
        str = CMD_INV;
        break;
    default:
        return NULL;
    }
    strncpy(result, str, CMD_STR_LEN);
    result[CMD_STR_LEN] = 0; // NULL terminate
    return result;
}

size_t main_pkg_size(char *pkg)
{
    return strlen(pkg)+1;
}

int alloc_main_pkg(char **main_pkg, size_t *curr_size,
    const size_t new_size)
{
    size_t new_memory = 0;
    if (new_size > *curr_size) {
        size_t diff = abs(*curr_size - new_size);
        if (diff < 64) {
            // When you are at it, take the whole word
            new_memory = diff + 64;
            *curr_size = new_size + 64;
            *main_pkg = realloc(*main_pkg, *curr_size);
        } else {
            new_memory = diff;
            *curr_size = new_size;
            *main_pkg = realloc(*main_pkg, *curr_size);
        }
    }
    if (!main_pkg) {
        return ERROR;
    }
    return new_memory;
}

void header_from_nw(struct header* header)
{
    // Correct endianness
    header->cmd = ntohs(header->cmd);
    header->size = ntohs(header->size);
}

struct header *read_header(char *buffer)
{
    // Read the buffer as header struct
    struct header *temp = (struct header*) buffer;

    // Correct endianness
    temp->cmd = ntohs(temp->cmd);
    temp->size = ntohs(temp->size);

    // Copy buffer content to heap allocated
    // struct and return pointer.
    size_t size = sizeof(struct header);
    struct header *copy = malloc(size);
    memcpy(copy, temp, size);
    return copy;
}

void header_to_nw(struct header *header)
{
    // Correct endianness
    header->cmd = htons(header->cmd);
    header->size = htons(header->size);
}

int create_header(struct header *header, uint16_t cmd,
    char *sender, uint16_t content_size)
{
    memset(header, 0, sizeof(struct header));
    header->cmd = cmd;
    header->size = content_size;

    if (strlen(sender) > SENDER_SIZE)
        return ERROR;

    strncpy(header->sender, sender, SENDER_SIZE);
    return SUCCESS;
}
