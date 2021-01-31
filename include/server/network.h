#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <unistd.h>
#include <stdlib.h>

#include "lib/protocol.h"
#include "lib/network.h"
#include "lib/common.h"

#define SERVER_NAME "SERVER"
#define SERVER_QUEUE_SIZE 50

/**
 * @brief Creates socket for the server, that listens
 *        specified port.
 * 
 * @param port server port
 * @return int SUCCESS if OK, else ERROR
 */
int init_connection(const char *port, SOCKET *sock,
    struct sockaddr **addr, int queue_size);

/**
 * @brief Sends response code to client using
 *        open client socket.
 * 
 * @param sock client socket
 * @param cmd see commands in protocol.h
 * @return int -1 for error, else sent amount of bytes.
 */
int send_code(SOCKET sock, char *name, int cmd);

/**
 * @brief Client message is received and parsed. This function
 *        tries to serve clients request.
 * 
 * @param conn open server-client connection
 * @param header_pkg received header package
 * @param sbuff static read buffer
 * @return int SUCCESS for ok, else ERROR
 */
int handle_request(const struct server_conn *conn,
    const struct header *header_pkg, char *sbuff,
    size_t sbuff_len);

/**
 * @brief Waits for client connections. When connection is
 *        accepted, it is stored in server_conn parameter.
 * 
 * @param conn connection structure where open connection is stored.
 * @return int SUCCESS for ok, else ERROR
 */
int wait_connection(struct server_conn *conn, SOCKET sock);

#endif // !SERVER_NETWORK_H