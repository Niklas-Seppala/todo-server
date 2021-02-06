#include <stdlib.h>
#include <signal.h>

#include "server/IO.h"
#include "server/todo.h"
#include "server/queue.h"
#include "server/threads.h"
#include "server/network.h"
#include "server/database.h"

// SERVER STUFF
static SOCKET SERVER_SOCK = -1;
static struct sockaddr *SERVER_ADDRESS = NULL;

// THREADS
pthread_mutex_t idle_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
int thread_exit_flag = 0;

static void cleanup(void)
{
    log_info("Cleaning up...");
    sfree(SERVER_ADDRESS);

    close(SERVER_SOCK);
    fclose(IN_STREAM);
    fclose(OUT_STREAM);

    thread_exit_flag = 1;
    pthread_cond_broadcast(&idle);
    shutdown_client_threads();

    log_info("Cleanup complete.");
    fclose(LOG_STREAM);
}

void sigint_handler(int sig)
{
    if (sig == SIGINT) {
        log_warn("Received signal: [SIGINT] Server shutting down!");
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

int shutdown_server(const int r_code, const char *log)
{
    if (log) {
        log_warn(log);
    }
    cleanup();
    exit(r_code);
}

int setup(int argc, const char **argv, db_info_t *db_info)
{
    io_set_default_streams();
    signal(SIGINT, sigint_handler);

    if (validate_args(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        query_db_info(db_info);
    } else if (argc > 5) {
        const char *host    = argv[2];
        const char *user    = argv[3];
        const char *pw      = argv[4];
        const char *db_name = argv[5];
        db_create_info(db_info, db_name, host, user, pw, 0);
    } else {
        log_error("Database parameters invalid", 0);
        return ERROR;
    }
    db_init(db_info);

    if (io_setup() != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }

    fprintf(LOG_STREAM, "\n");
    log_info("Starting new session");

    const char *PORT = argv[1];
    if (init_connection(PORT, &SERVER_SOCK, &SERVER_ADDRESS,
        SERVER_QUEUE_SIZE) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    io_setup_success(SERVER_ADDRESS);

    return SUCCESS;
}

void *delegate_conn(void *work_queue)
{
    server_work_t **client_queue = (server_work_t **)work_queue;
    char sbuffer[THREAD_READ_BUFF_SIZE];
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

        // Thread pops work from queue.
        server_work_t *work = conn_dequeue(client_queue);

        if (work) {
            pthread_cleanup_push(thread_cleanup, work);

            // TODO: delegate logging IO to single thread, so handling
            //       client requests is smoother.
            vflog_info("Accepted client [%ld] at address: %s:%s",
                work->id, work->connection->readable.ip_addr,
                work->connection->readable.port
            );

            // Read the header
            struct header header_pkg;
            int read_rc = read_socket(work->connection->sock, sbuffer,
                &header_pkg, THREAD_READ_BUFF_SIZE, HEADER_SIZE
            );

            if (read_rc != SUCCESS) {
                if (read_rc & READ_OVERFLOW) {
                    // The protocol dictates that communications
                    // begin with 16-byte size header package.
                    // Here client sent rubish

                    // TODO: Separate IO logging thread
                    log_error("Package overflow while reading socket", 0);

                    // Send error code to client
                    send_code(work->connection->sock, "SERVER", INV);
                }
            } else {
                header_from_nw(&header_pkg);
                pthread_testcancel();            // cancellation point
                handle_request(work->connection, &header_pkg,
                    sbuffer, THREAD_READ_BUFF_SIZE
                );
            }

            vflog_info("Client [%ld] handled.", work->id);
            pthread_cleanup_pop(EXECUTE_CLEANUP);
        }
        // If flag is set, threads will exit and join cleanup
        if (thread_exit_flag) {
            return NULL;
        }
    }
}

/**
 * @brief Runs the server main thread loop. Main thread
 *        accepts client connections, and delegates them to
 *        client handling threads.
 */
static void run(void)
{
    uint64_t client_num = 0;
    server_work_t *client_queue = NULL;
    init_threads(POOL_SIZE, delegate_conn, (void *)&client_queue);
    for (;;) {
        struct server_conn *conn = malloc(sizeof(struct server_conn));
        if (wait_connection(conn, SERVER_SOCK) == SUCCESS) {

            // Push work to queue
            work_enqueue(&client_queue, conn, client_num++);

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
int main(int argc, char const **argv)
{
    db_info_t db_info;
    setup(argc, argv, &db_info);
    fprintf(OUT_STREAM, "%s\n", "Server running..");
    run(); // Blocks indefinitely.
}
