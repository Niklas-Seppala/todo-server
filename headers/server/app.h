#ifndef APP_H
#define APP_H

#include "lib/network.h"

#define SERVER_QUEUE_SIZE 5

SOCKET SERVER_SOCK;
SOCKET CLIENT_SOCK;
struct sockaddr *SERVER_ADDRESS;

#endif // APP_H
