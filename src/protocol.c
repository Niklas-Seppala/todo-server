#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "common.h"
#include "protocol.h"

static const int CMD_STR_LEN = 3;

int str_to_enum(char *cmd) {
    int result = 0;
    for (int i = 0; i < CMD_STR_LEN; i++) {
        result += cmd[i];
    }
    return result;
}

char *enum_to_str(int cmd) {
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
    case OK:
        str = CMD_OK;
        break;
    case INV:
        str = CMD_INV;
        break;
    default:
        return NULL;
    }
    strncpy(result, str, CMD_STR_LEN);
    return result;
}

struct header *read_header(char *buffer) {
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

void header_to_network(struct header *header) {
    // Correct endianness
    header->cmd = htons(header->cmd);
    header->size = htons(header->size);
}

int create_header(struct header *header, uint16_t cmd, char *sender, uint16_t content_size) {
    memset(header, 0, sizeof(struct header));
    header->cmd = cmd;
    header->size = content_size;
    if (strlen(sender) > SENDER_SIZE)
        return ERROR;
    strncpy(header->sender, sender, SENDER_SIZE);
    return SUCCESS;
}
