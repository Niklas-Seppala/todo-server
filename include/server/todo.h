/**
 * @file todo.h
 * @author Niklas Seppälä
 * @brief Module that provides todo-list functionality
 *        to the server.
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef TODO_H
#define TODO_H

#include <stdlib.h>
#include "lib/network.h"
#include "lib/protocol.h"

/**
 * @brief Sends a reply code to socket.
 * 
 * @param sock target
 * @param cmd VAL / INV
 * @return int SUCCESS if ok ELSE ERROR
 */
int send_code(SOCKET sock, int cmd);

/**
 * @brief Serves the client ADD command.
 * 
 * @param sock client socket
 * @param header header received from client
 * @param main_pkg main package buffer
 * @param curr_size current size of the main package buffer
 * @param static_buff static read buffer
 * @param static_buff_size size of the static read buffer.
 * @return int SUCCESS if ok, else ERROR
 */
int handle_ADD(const SOCKET sock, const struct header *header, char **main_pkg,
    size_t *curr_size, char *static_buff, const size_t static_buff_size);

/**
 * @brief Serves the client RMV command.
 * 
 * @param sock client socket
 * @param header header received from client
 * @return int SUCCESS if ok, else ERROR
 */
int handle_RMV(const SOCKET sock, const struct header *header);

/**
 * @brief Serves the client LOG command.
 * 
 * @param sock client socket
 * @param header header package received from client.
 * @return int SUCCESS if ok, else ERROR
 */
int handle_LOG(const SOCKET sock, const struct header *header);

#endif // !TODO_H

