#include <stdlib.h>
#include "server/todo.h"
#include "server/app.h"
#include "server/flow.h"
#include "lib/common.h"

int send_code(SOCKET sock, int cmd)
{
    struct header h;
    create_header(&h, cmd, SERVER_NAME, 0);
    header_to_nw(&h);
    return send(sock, &h, HEADER_SIZE, 0);
}

int handle_ADD(const SOCKET sock, const struct header *header, char **main_pkg,
    size_t *curr_size, char *static_buff, const size_t static_buff_size)
{
    ssize_t n_bytes = alloc_main_pkg(main_pkg, curr_size, header->size);

    if (n_bytes == ERROR) {
        log_error(NULL, SYS_ERROR);
        shutdown_server(EXIT_FAILURE, "Memory allocation failed."); // for now
    }

    send_code(sock, VAL);
    read_socket(sock, static_buff, *main_pkg,
        RECV_BUFF_SIZE, *curr_size
    );

    vflog_info("Main package: %s", *main_pkg); // DEBUG
    // TODO: Store main_pkg contents to database

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
