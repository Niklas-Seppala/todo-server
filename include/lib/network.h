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
#include <sys/socket.h>
#include <netdb.h>
#include "lib/IO.h"

/**
 * @brief Socket should be socket
 */
#define SOCKET int

/**
 * @brief Socket reading has failed, look for more details
 */
#define READ_ERR      0x2
/**
 * @brief User has sent more bytes than promised,
 *        result buffer overflow.
 */
#define READ_OVERFLOW 0x4

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
    // String representation of port
    char port[PORT_LEN];

    // String representation of ipv4 or ipv6
    char ip_addr[INET6_ADDRSTRLEN];
};

/**
 * @brief Connection data structure
 */
struct server_conn {
    // Connection socket.
    SOCKET sock;

    // Address size in bytes.
    socklen_t addrsize;

    // Generic storage for ipv4 or ipv6
    struct sockaddr_storage addr;

    // String representation of address
    struct readable_addr readable;
};

/**
 * @brief Starts to receive data to main buffer. Bytes are copied
 *        to result buffer. 
 * 
 * @param sock open socket
 * @param main_buffer Reuseable buffer with static size
 * @param pkg_buffer Result buffer, where bytes are collected
 * @param main_size Main buffer size in bytes
 * @param pkg_size result buffer size
 * @return int SUCCESS if OK, else READ_ERR with OR'd details
 */
int read_socket(SOCKET sock, char *main_buffer,
    void *pkg_buffer, const size_t main_size,
    const size_t pkg_size);

/**
 * @brief  Get socket address info as generic
 *         socket address. Works with both IPV4/IPV6.
 *         ALLOCATES HEAP MEMORY!
 * 
 * @param sock socket file descriptor
 * @return struct sockaddr* pointer to generic sockaddr in heap
 */
struct sockaddr *generic_addr(const SOCKET sock);

/**
 * @brief Get socket address of the peer as generic
 *         socket address. Works with both IPV4/IPV6.
 *         ALLOCATES HEAP MEMORY!
 * 
 * @param sock socket
 * @return struct sockaddr* pointer to generic sockaddr in heap
 */
struct sockaddr *generic_peer_addr(const SOCKET sock);

/**
 * @brief Creates a ready to use server socket.
 * 
 * @param service port/servicename
 * @param queue_size max queue size
 * @return int socket file descriptor
 */
int server_socket(const char *service, int queue_size,
    struct sockaddr_storage *out_addr);

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