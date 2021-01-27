#include <stdlib.h>
#include "server/todo.h"
#include "server/app.h"
#include "server/server.h"
#include "lib/common.h"

int handle_ADD(const SOCKET sock, const struct header *header,
    char *sbuff, const size_t sbuff_size)
{
    char *main_pkg = malloc(header->size);
    if (!main_pkg) {
        log_error(NULL, SYS_ERROR);
        log_warn("uh oh :D");
        // TODO: exit thread
    }

    send_code(sock, VAL);

    read_socket(sock, sbuff, main_pkg, sbuff_size, header->size);

    vflog_info("Main package: %s", main_pkg);
    // TODO: Store main_pkg contents to database

    safe_free((void **)&main_pkg);
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
