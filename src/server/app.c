#include <stdlib.h>
#include <signal.h>

#include "server/app.h"
#include "lib/common.h"
#include "server/flow.h"
#include "server/server_IO.h"

SOCKET SERVER_SOCK = -1;
SOCKET CLIENT_SOCK = -1;
struct sockaddr *SERVER_ADDRESS = NULL;

static int start(int argc, const char **argv) {
    io_set_default_streams();
    signal(SIGINT, signal_handler);

    if (validate_args(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE);
    }
    // Set LOG_STREAM
    if (io_setup() != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    log_new_session();
    // Try to open a main server socket
    const char *PORT = argv[1];
    if (init_connection(PORT) != SUCCESS) {
        io_setup_fail();
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    io_setup_success();
    return SUCCESS;
}

static void serve_clients() {
    log_info("Ready to serve clients");
    for (;;) {
        // TODO: accept connections
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
