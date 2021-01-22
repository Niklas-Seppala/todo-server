#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "network.h"

struct sockaddr *generic_addr(const int sock) {
    socklen_t size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage *storage = malloc(size);
    getsockname(sock, ((struct sockaddr *)storage), &size);
    return (struct sockaddr *)storage;
}

int server_socket(const char *service, int queue_size,
    int socktype, int protocol) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = socktype;
    hints.ai_protocol = protocol;

    struct addrinfo *addr_head;
    int rc = getaddrinfo(NULL, service, &hints, &addr_head);
    if (rc != 0) {
        // TODO: error handling
        printf("%s\n", gai_strerror(rc));
    }

    int sock = -1;
    for (struct addrinfo *addr = addr_head; addr != NULL; addr = addr->ai_next) {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0) {
            continue;
        }

        // Sock created, now bind it to address.
        if ((bind(sock, addr->ai_addr, addr->ai_addrlen) == 0) &&
            listen(sock, queue_size) == 0) {
            // Socket is ready.
            break;
        }

        // Failed, try next.
        close(sock);
        sock = -1;
    }
    freeaddrinfo(addr_head);
    return sock;
}

struct readble_addr *addr_to_readable(const struct sockaddr * address) {
    // Allocate memory where to strore strings.
    struct readble_addr *text = malloc(sizeof(struct readble_addr));

    void * bin_addr;
    switch (address->sa_family) {
    case AF_INET: // IPV4
        bin_addr = &(((struct sockaddr_in *)address)->sin_addr);
        snprintf(text->port, sizeof(text->port), "%d", ntohs(
            ((struct sockaddr_in *)address)->sin_port)
        );
        break;
    case AF_INET6: // IPV6
        bin_addr = &(((struct sockaddr_in6 *)address)->sin6_addr);
        snprintf(text->port, sizeof(text->port), "%d", ntohs(
            ((struct sockaddr_in *)address)->sin_port)
        );
        break;
    default:
        break;
    }
    inet_ntop(address->sa_family, bin_addr,
        text->ip_addr,
        sizeof(text->ip_addr)
    );
    return text;
}
