#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "lib/protocol.h"
#include "server/app.h"
#include "lib/common.h"
#include "server/flow.h"
#include "server/server_IO.h"

// Globals
SOCKET SERVER_SOCK = -1;
struct sockaddr *SERVER_ADDRESS = NULL;
long int conn_num = 0;
static char recv_buffer[RECV_BUFF_SIZE];


size_t main_pkg_len = 256;
char *main_pkg;

int start(int argc, const char **argv) {
    io_set_default_streams();
    signal(SIGINT, signal_handler);

    if (validate_args(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    if (io_setup() != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }

    log_new_session();

    const char *PORT = argv[1];
    if (init_connection(PORT) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    io_setup_success();
    return SUCCESS;
}

/**
 * @brief Server waits and potentially accepts
 *        connection. Fills connection data to
 *        provided server_conn struct.
 * 
 * @return int SUCCESS if ok, ERROR if failed
 */
int wait_connection(struct server_conn *conn)
{
    conn->addrsize = sizeof(struct sockaddr_storage);
    conn->sock = accept(SERVER_SOCK,
        (struct sockaddr *)&conn->addr, &conn->addrsize
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

int send_code(SOCKET sock, int cmd)
{
    struct header h;
    create_header(&h, cmd, SERVER_NAME, 0);
    header_to_network(&h);

    return send(sock, &h, HEADER_SIZE, 0);
}

int handle_ADD(const struct server_conn *conn,
    const struct header *header_pkg)
{
    ssize_t n_bytes = alloc_main_pkg(&main_pkg, &main_pkg_len,
        header_pkg->size
    );
    if (n_bytes == ERROR) {
        log_error(NULL, SYS_ERROR);
        shutdown_server(EXIT_FAILURE,
            "Memory allocation failed."); // for now
    }
    send_code(conn->sock, VAL);
    read_socket(conn->sock, recv_buffer, main_pkg,
        RECV_BUFF_SIZE, main_pkg_len
    );
    vflog_info("Main package: %s", main_pkg); // DEBUG
    return SUCCESS;
}

int serve_client_request(const struct server_conn *conn,
    const struct header *header_pkg) 
{
    switch (header_pkg->cmd) {
    case ADD:
        return handle_ADD(conn, header_pkg);
    default:
        return ERROR;
        break;
    }
}

int handle_connection(struct server_conn *conn) 
{
    vflog_info("Accepted client %ld at address: %s:%s",
        conn_num, conn->readable.ip_addr, conn->readable.port
    );

    struct header header_pkg;
    int read_rc = read_socket(conn->sock, recv_buffer, &header_pkg,
        RECV_BUFF_SIZE, HEADER_SIZE
    );

    if (read_rc != SUCCESS) {
        if (read_rc & READ_OVERFLOW) {
            // The protocol dictates that communications
            // begin with 16-byte size header package.
            // Here client sent rubish >:(
            send_code(conn->sock, INV);
            return ERROR;
        } else if (read_rc & READ_VAL_ERR) {
            log_error("NULL buffer", 0);
            shutdown_server(EXIT_FAILURE, NULL);
        }
    } else {
        header_from_network(&header_pkg);
        serve_client_request(conn, &header_pkg);
    }

    return SUCCESS;
}

/**
 * @brief Waits for connections and handles
 *        them. After handling is complete,
 *        does some cleanup, and starts waiting
 *        for new client.
 */
static void serve_clients() {
    main_pkg = malloc(main_pkg_len);
    for (;;) {
        struct server_conn conn;
        if (wait_connection(&conn) == SUCCESS) {
            handle_connection(&conn);

            close(conn.sock);
            vflog_info("Connection %ld closed.", conn_num++);
        }
    }
}

/**
 * @brief Starts the server after configurations
 *        are complete.
 * 
 * @param argc CLI argument count. MIN 2
 * @param argv Array of arguments
 */
int main(int argc, char const *argv[]) {
    start(argc, argv);
    fprintf(OUT_STREAM, "%s\n", "Server running..");
    serve_clients(); // Blocks indefinitely.
}
