#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "server/todo.h"
#include "server/app.h"
#include "server/server.h"
#include "server/server_IO.h"
#include "lib/common.h"

void *handle_connection(void *unused);

#define EXECUTE_CLEANUP 1

SOCKET SERVER_SOCK = -1;

struct sockaddr *SERVER_ADDRESS = NULL;
static volatile int join_flag = 0;
static serverwork_t *client_queue = NULL;
static pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t idle_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

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

    fprintf(LOG_STREAM, "\n");
    log_info("Starting new session");

    const char *PORT = argv[1];
    if (init_connection(PORT) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    io_setup_success();

    init_threads(handle_connection);

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

static void thread_cleanup(void *arg)
{
    serverwork_t *work = (serverwork_t *)arg;
    if (work) {
        if (work->connection->sock > 0) {
            close(work->connection->sock);
        }
        safe_free((void **)&work->connection);
        safe_free((void **)&work);
    }
}

void shutdown_workers()
{
    join_flag = 1;
    pthread_cond_broadcast(&idle);
}

void *handle_connection(void *unused)
{
    char sbuffer[STATIC_BUFF_SIZE];
    for (;;)
    {
        // All threads wait here, untill there
        // main thread pushes work to queue and
        // releases one thread at a time.

        // In case of SIGINT, all threads are released,
        // and they will all handle remaining work in queue
        // and the exit.
        pthread_mutex_lock(&idle_lock);
        pthread_cond_wait(&idle, &idle_lock);
        pthread_mutex_unlock(&idle_lock);

        // thread safety
        pthread_mutex_lock(&queue_lock);
        serverwork_t *work = conn_dequeue(&client_queue);
        pthread_mutex_unlock(&queue_lock);

        if (work) {
            pthread_cleanup_push(thread_cleanup, work);

            vflog_info("Accepted client [%ld] at address: %s:%s",
                work->id, work->connection->readable.ip_addr,
                work->connection->readable.port
            );

            struct header header_pkg;
            int read_rc = read_socket(work->connection->sock,
                sbuffer,
                &header_pkg,
                STATIC_BUFF_SIZE,
                HEADER_SIZE
            );

            pthread_testcancel();                   // cancel point
            if (read_rc != SUCCESS) {
                if (read_rc & READ_OVERFLOW) {
                    // The protocol dictates that communications
                    // begin with 16-byte size header package.
                    // Here client sent rubish
                    log_error("Package overflow while reading socket", 0);

                    // Send error code to client
                    send_code(work->connection->sock, INV);
                    return NULL;
                }
            } else {
                header_from_nw(&header_pkg);
                pthread_testcancel();               // cancel point
                handle_request(work->connection, &header_pkg, sbuffer);
            }

            vflog_info("Client [%ld] handled.", work->id);
            pthread_cleanup_pop(EXECUTE_CLEANUP);
        }
        // If flag is set, threads will exit and join cleanup
        if (join_flag) {
            return NULL;
        }
    }
    return NULL;
}

static void run()
{
    for (;;) {
        struct server_conn *conn = malloc(sizeof(struct server_conn));
        if (wait_connection(conn) == SUCCESS) {

            // Push work to queue
            pthread_mutex_lock(&queue_lock);
            work_enqueue(&client_queue, conn, conn_num++);
            pthread_mutex_unlock(&queue_lock);

            // Let one thread access the task queue
            pthread_cond_signal(&idle);
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
    conn_num = 0;
    start(argc, argv);
    fprintf(OUT_STREAM, "%s\n", "Server running..");
    run(); // Blocks indefinitely.
}
