#ifndef SOCKOPT_H
#define SOCKOPT_H

#include <sys/socket.h>

int __vita_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int __vita_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

#endif // SOCKOPT_H
