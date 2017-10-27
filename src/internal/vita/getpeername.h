#ifndef GETPEERNAME_H
#define GETPEERNAME_H

#include <sys/socket.h>

int __vita_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

#endif // GETPEERNAME_H
