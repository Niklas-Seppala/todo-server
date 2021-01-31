#include <stdlib.h>

#include "server/todo.h"
#include "server/network.h"

int handle_ADD(const SOCKET sock, const struct header *header,
    char *sbuff, const size_t sbuff_size)
{
    char *main_pkg = malloc(header->size);

    send_code(sock, SERVER_NAME, VAL);

    read_socket(sock, sbuff, main_pkg, sbuff_size, header->size);

    // TODO: Store main_pkg contents to database

    sfree(main_pkg);
    return SUCCESS;
}

int handle_RMV(const SOCKET sock, const struct header *header)
{
    // TODO: Implement
    log_warn("RMV functionality not implemented!");
    return ERROR;
}

int handle_LOG(const SOCKET sock, const struct header *header)
{
    // TODO: Implement
    log_warn("LOG functionality not implemented!");
    return ERROR;
}
