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

static int start(int argc, const char **argv) {
    io_set_default_streams();
    signal(SIGINT, signal_handler);

    if (validate_args(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE);
    }
    // Set LOG_STREAM
    if (io_setup() != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE,
            "Aborting server setup!"
        );
    }
    log_new_session();

    // Try to open a main server socket
    const char *PORT = argv[1];
    if (init_connection(PORT) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE,
            "Aborting server setup!"
        );
    }
    io_setup_success();
    return SUCCESS;
}

/**
 * @brief Server waits and potentially accepts
 *        connection. It then creates server_conn
 *        datastruct and fills it's fields.
 *        ALLOCATES HEAP MEMORY!
 * 
 * @return struct server_conn* pointer to connection
 *         struct. NULL if failed.
 */
static struct server_conn *wait_connection() {
    struct server_conn *conn = calloc(1,
        sizeof(struct server_conn)
    );
    // size for both ipv4 and ipv6
    conn->size = sizeof(struct sockaddr_storage);

    // Server socket waits for connections
    conn->sock = accept(SERVER_SOCK,
        (struct sockaddr *)&conn->addr,
        &conn->size
    );
    // If Connection was rejected, cleanup and return NULL
    if (conn->sock < 0) {
        log_error(NULL, SYS_ERROR);
        safe_free((void **)&conn);
    } else {
        // Make connection address readable
        addr_to_readable((struct sockaddr *)&conn->addr,
            &conn->readable
        );
    }
    return conn;
}

int read_header_pkg(void *h_buffer, struct server_conn* conn) {
    size_t total_bytes = 0;
    ssize_t read = 0;
    char *pkg_head = h_buffer;

    while (total_bytes < HEADER_SIZE) {

        // Reset buffer, only HEADER_SIZE
        memset(recv_buffer, 0, HEADER_SIZE);

        // Read all available bytes from socket to recv buffer
        read = recv(conn->sock, recv_buffer, RECV_BUFF_SIZE, 0);
        if (read < 0) {
            // Socket read failed, abort!
            log_error(NULL, SYS_ERROR);
            return ERROR;
        }
        else {
            // Keep track of acculminated network message length
            // aka. what client has sent.
            total_bytes += read;
        }
        if (total_bytes > HEADER_SIZE) {
            // Protocol dictates that client must start communications
            // by sending HEADER package of 16-bytes.
            log_error("Client is not following potocol.", 0);
            vflog_info("Message length: %lu", total_bytes);
            log_info("Sending error message...");

            // TODO: send error message to client.
            return ERROR;
        } else {
            // Copy available data from recv buffer to
            // header package buffer.
            memcpy(pkg_head, recv_buffer, read);
            // Keep track of the current pointer after multiple
            // writes
            pkg_head += read;
        }
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
    // size_t main_pkg_size = 128;
    // char m_pkg_buffer[main_pkg_size];

    for (;;) {
        struct server_conn *conn = wait_connection();
        if (conn) {
            // Log client address info
            vflog_info("Accepted client %ld at address: %s:%s",
                conn_num,
                conn->readable.ip_addr,
                conn->readable.port
            );

            struct header *h_pkg = calloc(1, sizeof(struct header));
            if (read_header_pkg(h_pkg, conn) == SUCCESS) {
                header_from_network(h_pkg);
                // TODO: reallocate more/less memory for main_pkg-buffer
                //       based on size specified in header

                // TODO: send VAL msg to client, and wait for main package
            } else {
                // TODO: send Error msg to client ??
            }

            // Connection handled
            close(conn->sock);
            safe_free((void **)&h_pkg);
            safe_free((void **)&conn);
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
