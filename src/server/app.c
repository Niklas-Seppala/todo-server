#include <stdlib.h>
#include <signal.h>

#include "lib/common.h"
#include "server/flow.h"
#include "server/server_IO.h"

int setup(int argc, const char **argv) {

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    if (!validate_args(argc, argv)) {
        return ERROR;
    }

    if (!config_server_io()) {
        shutdown_server(EXIT_FAILURE);
    }

    const char *PORT = argv[1];
    if (!init_connection(PORT)) {
        shutdown_server(EXIT_FAILURE);
    }

    return SUCCESS;
}



int main(int argc, char const *argv[]) {
    if (setup(argc, argv) != SUCCESS) {
        return EXIT_FAILURE;
    }
    

    // Server Connections
    for(;;) { }
}
