#ifndef GETSOCKNAME_H
#define GETSOCKNAME_H

#include <sys/socket.h>

int __vita_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

#endif // GETSOCKNAME_H
