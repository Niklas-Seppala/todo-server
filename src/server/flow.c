#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "lib/common.h"
#include "lib/network.h"
#include "server/server_IO.h"
#include "server/app.h"

static void cleanup(void) {
    log_info("Cleaning up...");
    safe_free((void **)&SERVER_ADDRESS);

    close(SERVER_SOCK);

    fclose(IN_STREAM);
    fclose(OUT_STREAM);

    log_info("Cleanup complete.");
    fclose(LOG_STREAM);
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        log_warn("Received signal: [SIGINT] Server shutting down!");
        vflog_info("Connections handled: %d", conn_num);
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

int init_connection(const char *port) {
    struct sockaddr_storage *server_address = calloc(1,
        sizeof(struct sockaddr_storage));
    SERVER_SOCK = server_socket(port, SERVER_QUEUE_SIZE, server_address);
    if (SERVER_SOCK < 0) {
        safe_free((void **)&server_address);
        log_error("Could not create valid server socket", 0);
        return ERROR;
    }
    SERVER_ADDRESS = (struct sockaddr *)server_address;
    return SUCCESS;
}

int shutdown_server(const int r_code, const char *log) {
    if (log) {
        log_warn(log);
    }
    vflog_info("Connections handled", "%ld", conn_num);
    cleanup();
    exit(r_code);
}
