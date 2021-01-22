/**
 * @file network.h
 * @author Niklas Seppälä (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#if !defined NETWORK_H
#define NETWORK_H

struct readble_addr {
    char port[16];
    char ip_addr[INET6_ADDRSTRLEN];
};

/**
 * @brief  Get socket address info as generic
 *         socket address. Works with both IPV4/IPV6.
 *         ALLOCATES HEAP MEMORY!
 * 
 * @param sock 
 * @param generic_addr 
 * @return struct sockaddr* 
 */
struct sockaddr *generic_addr(const int sock);

/**
 * @brief Creates a ready to use server socket.
 * 
 * @param service port/servicename
 * @param queue_size max queue size
 * @param socktype STREAM / DGRAM
 * @param protocol TCP / UPD
 * @return int socket file descriptor
 */
int server_socket(const char *service, int queue_size,
    int socktype, int protocol);


/**
 * @brief Copies address and port to readable address
 *        struct. ALLOCATES HEAP MEMORY!
 * 
 * @param address generic address struct.
 * @return struct addr_text* pointer to readable struct located in heap
 */
struct readble_addr *addr_to_readable(const struct sockaddr * address);

#endif