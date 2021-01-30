#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "lib/common.h"
#include "lib/protocol.h"
#include "lib/network.h"
#include "server/server.h"
#include "server/server_IO.h"
#include "server/app.h"
#include "server/todo.h"

static pthread_t threadpool[POOL_SIZE];

void init_threads(void *(*thread_func)(void *))
{
    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_create(&threadpool[i], NULL, thread_func, NULL);
    }
}

static void cleanup(void)
{
    log_info("Cleaning up...");
    safe_free((void **)&SERVER_ADDRESS);

    close(SERVER_SOCK);
    fclose(IN_STREAM);
    fclose(OUT_STREAM);

    shutdown_workers();
    for (size_t i = 0; i < POOL_SIZE; i++) {
        pthread_join(threadpool[i], NULL);
    }

    log_info("Cleanup complete.");
    fclose(LOG_STREAM);
}



void sigint_handler(int sig)
{
    if (sig == SIGINT) {
        log_warn("Received signal: [SIGINT] Server shutting down!");
        vflog_info("Connections handled: %d", conn_num);
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

int init_connection(const char *port)
{
    struct sockaddr_storage *serv_addr = calloc(1,
        sizeof(struct sockaddr_storage));

    SERVER_SOCK = server_socket(port, SERVER_QUEUE_SIZE, serv_addr);
    if (SERVER_SOCK < 0) {
        safe_free((void **)&serv_addr);
        log_error("Could not create valid server socket", 0);
        return ERROR;
    }

    SERVER_ADDRESS = (struct sockaddr *)serv_addr;
    return SUCCESS;
}

int send_code(SOCKET sock, int cmd)
{
    struct header h;
    create_header(&h, cmd, SERVER_NAME, 0);
    header_to_nw(&h);
    return send(sock, &h, HEADER_SIZE, 0);
}

int handle_request(const struct server_conn *conn,
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


int shutdown_server(const int r_code, const char *log)
{
    if (log) {
        log_warn(log);
    }

    vflog_info("Connections handled", "%ld", conn_num);
    cleanup();
    exit(r_code);
}
