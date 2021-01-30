#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "server/IO.h"
#include "server/app.h"
#include "server/todo.h"
#include "server/threads.h"
#include "server/network.h"

/**
 * @brief Main server socket file descriptor.
 */
static SOCKET SERVER_SOCK = -1;

/**
 * @brief Main server socket address.
 */
static struct sockaddr *SERVER_ADDRESS = NULL;

// /**
//  * @brief Queue data structure, where incoming client
//  *        connections are pushed. Client handler threads will
//  *        pick up these connections and handle them.
//  */
//  server_work_t *client_queue = NULL;

/**
 * @brief Cleans up all server resources.
 */
static void cleanup(void)
{
    log_info("Cleaning up...");
    sfree(SERVER_ADDRESS);

    close(SERVER_SOCK);
    fclose(IN_STREAM);
    fclose(OUT_STREAM);

    shutdown_client_threads();

    log_info("Cleanup complete.");
    fclose(LOG_STREAM);
}

/**
 * @brief Interrupt signal handler. Activates graceful
 *        shutdown, and the exits program with
 *        EXIT_SUCCESS code
 * 
 * @param sig SIGINT
 */
void sigint_handler(int sig)
{
    if (sig == SIGINT) {
        log_warn("Received signal: [SIGINT] Server shutting down!");
        vflog_info("Connections handled: %d", conn_num);
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

/**
 * @brief Graceful shutdown with return code.
 * 
 * @param r_code program exit code.
 * @param log optional final info log
 * @return int program exit code.
 */
int shutdown_server(const int r_code, const char *log)
{
    if (log) {
        log_warn(log);
    }

    vflog_info("Connections handled", "%ld", conn_num);
    cleanup();
    exit(r_code);
}

/**
 * @brief Sets up all needed resources.
 * 
 * @param argc CLI arg count
 * @param argv CLI args
 * @return int SUCCESS for ok, else ERROR
 */
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
    if (init_connection(PORT, &SERVER_SOCK, &SERVER_ADDRESS, SERVER_QUEUE_SIZE) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    io_setup_success(SERVER_ADDRESS);

    return SUCCESS;
}

/**
 * @brief Runs the server main thread loop. Main thread
 *        accepts client connections, and delegates them to
 *        client handling threads.
 */
static void run()
{

    server_work_t *client_queue = NULL;
    printf("%p\n", &client_queue);
    
    init_threads(delegate_conn, (void *)&client_queue);

    for (;;) {
        struct server_conn *conn = malloc(sizeof(struct server_conn));
        if (wait_connection(conn, SERVER_SOCK) == SUCCESS) {

            // Push work to queue
            work_enqueue(&client_queue, conn, conn_num++);

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
