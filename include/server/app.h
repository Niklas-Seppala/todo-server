/**
 * @file app.h
 * @author Niklas Seppälä
 * @brief Server application constants
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef APP_H
#define APP_H

#include "lib/network.h"

/**
 * @brief Maximum connection in queue.
 */
#define SERVER_QUEUE_SIZE 10


#define MAIN_PKG_START_SIZE 512

/**
 * @brief Name of the server in 
 *        server-client communications.
 */
#define SERVER_NAME "SERVER"

/**
 * @brief Main server socket file descriptor.
 */
SOCKET SERVER_SOCK;

/**
 * @brief Main server socket address.
 */
struct sockaddr *SERVER_ADDRESS;

/**
 * @brief Rolling number of handled connections.
 */
long int conn_num;

#endif // APP_H
