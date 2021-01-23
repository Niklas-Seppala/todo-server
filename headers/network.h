/**
 * @file network.h
 * @author Niklas Seppälä
 * @brief C module for creating and handling sockets.
 * @version 0.1
 * @date 2021-01-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#if !defined NETWORK_H
#define NETWORK_H
#include <arpa/inet.h>

/**
 * @brief port is uint16, so
 *        max 5 chars
 */
#define PORT_LEN 5

/**
 * @brief Readable sockaddr fields
 *        for bot ipv4 and ipv6
 */
struct readable_addr {
    char port[PORT_LEN];
    char ip_addr[INET6_ADDRSTRLEN];
};

/**
 * @brief  Get socket address info as generic
 *         socket address. Works with both IPV4/IPV6.
 *         ALLOCATES HEAP MEMORY!
 * 
 * @param sock socket file descriptor
 * @return struct sockaddr* pointer to generic sockaddr in heap
 */
struct sockaddr *generic_addr(const int sock);

/**
 * @brief Get socket address of the peer as generic
 *         socket address. Works with both IPV4/IPV6.
 *         ALLOCATES HEAP MEMORY!
 * 
 * @param sock socket
 * @return struct sockaddr* pointer to generic sockaddr in heap
 */
struct sockaddr *generic_peer_addr(const int sock);

/**
 * @brief Creates a ready to use server socket.
 * 
 * @param service port/servicename
 * @param queue_size max queue size
 * @return int socket file descriptor
 */
int server_socket(const char *service, int queue_size);

/**
 * @brief Creates a ready to use client socket.
 * 
 * @param host name of the host
 * @param service service name / port
 * @return int socket file descriptor
 */
int client_socket(const char *host, const char *service);

/**
 * @brief Copies address and port to readable address
 *        struct.
 * 
 * @param address generic address struct ptr
 * @param result readable address struct ptr
 * @return return code
 */
int addr_to_readable(const struct sockaddr *address,
    struct readable_addr *result);

#endif