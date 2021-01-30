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
#include "lib/common.h"
#include "server/queue.h"

/**
 * @brief 
 */
#define STATIC_BUFF_SIZE 512

/**
 * @brief 
 */
#define POOL_SIZE 15

/**
 * @brief 
 */
#define EXECUTE_CLEANUP 1

/**
 * @brief Maximum connection in queue.
 */
#define SERVER_QUEUE_SIZE 50

/**
 * @brief Name of the server in
 *        server-client communications.
 */
#define SERVER_NAME "SERVER"


/**
 * @brief Rolling number of handled connections.
 */
long int conn_num;

#endif // APP_H
