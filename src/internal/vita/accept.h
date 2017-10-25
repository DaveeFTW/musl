#ifndef ACCEPT_H
#define ACCEPT_H

#include <sys/socket.h>

int __vita_accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);

#endif // ACCEPT_H
