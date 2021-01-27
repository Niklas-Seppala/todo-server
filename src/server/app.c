#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "server/todo.h"
#include "server/app.h"
#include "server/server.h"
#include "server/server_IO.h"
#include "lib/common.h"
#include "server/buffpool.h"

// Globals
SOCKET SERVER_SOCK = -1;
struct sockaddr *SERVER_ADDRESS = NULL;
long int conn_num = 0;
// static char recv_buffer[RECV_BUFF_SIZE];

#define BUFFER_POOL_DEPTH 10
static struct buffer_node *SBUFFER_POOL;

int start(int argc, const char **argv)
{
    io_set_default_streams();
    signal(SIGINT, sigint_handler);

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

    init_pool(&SBUFFER_POOL, BUFFER_POOL_DEPTH);

    return SUCCESS;
}

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

int serve_client_request(const struct server_conn *conn,
    const struct header *header_pkg, char *sbuff)
{
    switch (header_pkg->cmd) {
    case ADD:
        return handle_ADD(conn->sock, header_pkg, sbuff, STATIC_BUFF_SIZE);
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

void *handle_connection(void *connection)
{
    struct server_conn *conn = (struct server_conn *)connection;
    struct buffer_node * sbuff = get_free_buffer(SBUFFER_POOL);

    vflog_info("Accepted client %ld at address: %s:%s",
        conn_num, conn->readable.ip_addr, conn->readable.port
    );

    struct header header_pkg;
    int read_rc = read_socket(conn->sock, sbuff->buffer, &header_pkg,
        STATIC_BUFF_SIZE, HEADER_SIZE
    );

    if (read_rc != SUCCESS) {
        if (read_rc & READ_OVERFLOW) {
            // The protocol dictates that communications
            // begin with 16-byte size header package.
            // Here client sent rubish >:(
            log_error("Package overflow while reading socket", 0);
            log_warn("Aborting read!");
            send_code(conn->sock, INV); // Send error code to client
            return NULL;
        }
    } else {
        header_from_nw(&header_pkg);
        serve_client_request(conn, &header_pkg, sbuff->buffer);
    }

    free_buffer(sbuff);
    close(conn->sock);
    safe_free((void **)&conn);
    vflog_info("Connection %ld closed.", conn_num++);

    return NULL;
}

static void run()
{
    for (;;) {
        struct server_conn *conn = malloc(sizeof(struct server_conn));
        if (wait_connection(conn) == SUCCESS) {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&thread, &attr, handle_connection, conn);
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
int main(int argc, char const *argv[])
{
    start(argc, argv);
    fprintf(OUT_STREAM, "%s\n", "Server running..");
    run(); // Blocks indefinitely.
}
