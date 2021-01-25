#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "server/app.h"
#include "lib/common.h"
#include "server/flow.h"
#include "server/server_IO.h"

// Globals
SOCKET SERVER_SOCK = -1;
struct sockaddr *SERVER_ADDRESS = NULL;
long int conn_num = 0;

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

/**
 * @brief Waits for connections and handles
 *        them. After handling is complete,
 *        does some cleanup, and starts waiting
 *        for new client.
 */
static void serve_clients() {
    for (;;) {
        struct server_conn *conn = wait_connection();
        if (conn) {
            // Log client address info
            vflog_info("Accepted client %ld at address: %s:%s",
                conn_num,
                conn->readable.ip_addr,
                conn->readable.port
            );
            // Connection handled
            conn_num++;
            close(conn->sock);
            safe_free((void **)&conn);
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
