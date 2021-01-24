#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "lib/common.h"
#include "lib/network.h"
#include "server/server_IO.h"
#include "server/app.h"

static void cleanup(void) {
    fclose(ERR_STREAM);
    fclose(IN_STREAM);
    fclose(OUT_STREAM);

    close(SERVER_SOCK);
    close(CLIENT_SOCK);
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        // TODO: log signal
        cleanup();
        return;
    }
    if (sig == SIGQUIT) {
        // TODO: log signal
        cleanup();
        return;
    }
}

int init_connection(const char *port) {
    SERVER_SOCK = server_socket(port, SERVER_QUEUE_SIZE);
    if (SERVER_SOCK < 0) {
        error(NULL, SYS_ERROR);
        return ERROR;
    }
    return SUCCESS;
}

int shutdown_server(int r_code) {
    cleanup();
    exit(r_code);
}
