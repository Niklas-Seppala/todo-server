#include "server/network.h"

int init_connection(const char *port, SOCKET* sock,
    struct sockaddr **addr, int queue_size)
{
    struct sockaddr_storage *serv_addr = calloc(1,
        sizeof(struct sockaddr_storage));

    *sock = server_socket(port, queue_size, serv_addr);
    if (sock < 0) {
        sfree(serv_addr);
        log_error("Could not create valid server socket", 0);
        return ERROR;
    }

    *addr = (struct sockaddr *)serv_addr;
    return SUCCESS;
}

int send_code(SOCKET sock, char *name, int cmd)
{
    struct header header_pkg;
    create_header(&header_pkg, cmd, name, 0);
    header_to_nw(&header_pkg);
    return send(sock, &header_pkg, HEADER_SIZE, 0);
}

int handle_request(const struct server_conn *conn,
    const struct header *header_pkg, char *sbuff,
    size_t sbuff_len)
{
    switch (header_pkg->cmd) {
    case ADD:
        return handle_ADD(conn->sock, header_pkg, sbuff, sbuff_len);
    case LOG:
        return handle_LOG(conn->sock, header_pkg);
    case RMV:
        return handle_RMV(conn->sock, header_pkg);
    default:
        log_error("Unsupported client command.", 0);
        return ERROR;
        break;
    }
}

int wait_connection(struct server_conn *conn, SOCKET sock)
{
    conn->addrsize = sizeof(struct sockaddr_storage);
    conn->sock = accept(sock, (struct sockaddr *)&conn->addr,
        &conn->addrsize
    );

    if (conn->sock < 0) {
        log_error(NULL, SYS_ERROR);
        close(conn->sock);
        return ERROR;
    } else {
        addr_to_readable((struct sockaddr *)&conn->addr,
            &conn->readable
        );
    }

    return SUCCESS;
}
