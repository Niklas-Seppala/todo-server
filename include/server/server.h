#if !defined(SERVER_H)
#define SERVER_H
#include "lib/network.h"

#define STATIC_BUFF_SIZE 512
#define POOL_SIZE 30

/**
 * @brief Graceful shutdown with return code.
 * 
 * @param r_code program exit code.
 * @param log optional final info log
 * @return int program exit code.
 */
int shutdown_server(const int r_code, const char *log);

/**
 * @brief Creates socket for the server, that listens
 *        specified port.
 * 
 * @param port server port
 * @return int SUCCESS if OK, else ERROR
 */
int init_connection(const char *port);

void init_threads(void *(*thread_func)(void *));


int handle_request(const struct server_conn *conn,
    const struct header *header_pkg, char *sbuff);

/**
 * @brief Sends a reply code to socket.
 * 
 * @param sock target
 * @param cmd VAL / INV
 * @return int SUCCESS if ok ELSE ERROR
 */
int send_code(SOCKET sock, int cmd);

/**
 * @brief Interrupt signal handler. Activates graceful
 *        shutdown, and the exits program with
 *        EXIT_SUCCESS code
 * 
 * @param sig SIGINT
 */
void sigint_handler(int sig);

#endif // FLOW_H
