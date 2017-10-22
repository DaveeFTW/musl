#ifndef POLL_H
#define POLL_H

#include <poll.h>

int __vita_poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif // POLL_H
