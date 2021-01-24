#include <stdlib.h>
#include <signal.h>

#include "server/app.h"
#include "lib/common.h"
#include "server/flow.h"
#include "server/server_IO.h"

SOCKET SERVER_SOCK = -1;
SOCKET CLIENT_SOCK = -1;
struct sockaddr *SERVER_ADDRESS = NULL;

int setup(int argc, const char **argv) {
    // Set default streams
    OUT_STREAM = stdout;
    IN_STREAM = stdin;

    signal(SIGINT, signal_handler);

    if (validate_args(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    if (config_server_io() != SUCCESS) {
        fprintf(OUT_STREAM, "%s\n", "Could not start the server, see log for errors");
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }

    const char *PORT = argv[1];
    if (init_connection(PORT) != SUCCESS) {
        fprintf(OUT_STREAM, "%s\n", "Could not start the server, see log for errors");
        shutdown_server(EXIT_FAILURE, "Aborting server setup!");
    }
    
    // struct readable_addr *addr;
    // addr_to_readable()

    return SUCCESS;
}


int main(int argc, char const *argv[]) {
    setup(argc, argv);
    fprintf(OUT_STREAM, "%s\n", "Server configured");


    // Server Connections
    fprintf(OUT_STREAM, "%s\n", "Waiting clients");
    for(;;) { }
}
