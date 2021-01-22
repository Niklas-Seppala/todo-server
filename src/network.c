#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "network.h"

struct sockaddr *generic_addr(const int sock) {
    // Allocate enough memory for both ipv4 and ipv6
    socklen_t size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage *storage = malloc(size);

    // get socket info stored to storage.
    getsockname(sock, ((struct sockaddr *)storage), &size);

    // Cast storage to generic sockaddr struct
    return (struct sockaddr *)storage;
}

int server_socket(const char *service, int queue_size,
    int socktype, int protocol) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    // Specify what kind of socket we want.
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = socktype;
    hints.ai_protocol = protocol;

    // Get address info linked list
    struct addrinfo *head;
    int rc = getaddrinfo(NULL, service, &hints, &head);
    if (rc != 0) {
        // TODO: error handling
        printf("%s\n", gai_strerror(rc));
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

int addr_to_readable(const struct sockaddr * address,
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
