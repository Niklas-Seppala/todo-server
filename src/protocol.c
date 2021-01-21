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
    struct header *header = (struct header*) buffer;
    header->cmd = ntohs(header->cmd);
    header->size = ntohs(header->size);
    return header;
}

char * header_to_network(struct header *header) {
    header->cmd = htons(header->cmd);
    header->size = htons(header->size);
    return (char *)header;
}

int create_header(struct header *header, uint8_t cmd, char *sender, uint8_t content_size) {
    memset(header, 0, sizeof(struct header));
    header->cmd = cmd;
    header->size = content_size;
    if (strlen(sender) > H_SENDER_SIZE)
        return ERROR;
    strncpy(header->sender, sender, H_SENDER_SIZE);
    return SUCCESS;
}

