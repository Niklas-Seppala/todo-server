#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "lib/IO.h"
#include "lib/common.h"
#include "lib/network.h"

struct sockaddr *generic_peer_addr(const SOCKET sock) {
    socklen_t size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage *storage = malloc(size);
    getpeername(sock, (struct sockaddr *)storage, &size);
    return (struct sockaddr *)storage;
}

struct sockaddr *generic_addr(const SOCKET sock) {
    socklen_t size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage *storage = malloc(size);
    getsockname(sock, (struct sockaddr *)storage, &size);
    return (struct sockaddr *)storage;
}

int addr_to_readable(const struct sockaddr *address,
    struct readable_addr *result) {
    void *bin_addr;

    switch (address->sa_family) {
    case AF_INET:
        // Point to ipv4 address
        bin_addr = &(((struct sockaddr_in *)address)->sin_addr);
        // Correct port endianness, store as string
        snprintf(result->port, sizeof(result->port), "%d", ntohs(
            ((struct sockaddr_in *)address)->sin_port)
        );
        break;
    case AF_INET6:
        // Point to ipv6 address
        bin_addr = &(((struct sockaddr_in6 *)address)->sin6_addr);
        // Correct port endianness, store as string
        snprintf(result->port, sizeof(result->port), "%d", ntohs(
            ((struct sockaddr_in *)address)->sin_port)
        );
        break;
    default:
        // Unknown protocol
        return ERROR;
    }
    inet_ntop(address->sa_family, bin_addr,
        result->ip_addr,
        sizeof(result->ip_addr)
    );
    return SUCCESS;
}

int server_socket(const char *service, int queue_size,
    struct sockaddr_storage *out_addr) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    // Specify what kind of socket we want.
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Get address info linked list
    struct addrinfo *head;
    int rc = getaddrinfo(NULL, service, &hints, &head);
    if (rc != 0) {
        log_error(gai_strerror(rc), 0);
        return ERROR;
    }

    int sock = -1;
    for (struct addrinfo *node = head; node != NULL; node = node->ai_next) {
        sock = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
        if (sock < 0) {
            // Socket creation failed, try next node ->
            continue;
        }
        // Socket created, try setup
        if ((bind(sock, node->ai_addr, node->ai_addrlen) == 0) &&
            listen(sock, queue_size) == 0) {
            if (out_addr) {
                memcpy(out_addr, node->ai_addr, node->ai_addrlen);
            }
            // Socket is ready.
            break;
        }
        // Failed, try next ->
        close(sock);
        sock = -1;
    }
    freeaddrinfo(head);
    return sock;
}

int client_socket(const char *host, const char *service) {
    // Specify what kind of socket we want.
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Get address infos based on hostname and service.
    struct addrinfo *head;
    int ret_val = getaddrinfo(host, service, &hints, &head);
    if (ret_val != 0) {
        log_error(NULL, SYS_ERROR | FATAL);
        return ERROR;
    }

    // Search working address from linked address list.
    int sock = -1;
    for (struct addrinfo *node = head; node != NULL; node = node->ai_next)
    {
        // Create a socket based on address info node and connect it.
        sock = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
        if (sock >= 0 && connect(sock, node->ai_addr, node->ai_addrlen) == 0)
            break; // success

        // Socket creation failed, close and continue.
        close(sock);
        sock = -1;
    }
    freeaddrinfo(head);
    return sock;
}
