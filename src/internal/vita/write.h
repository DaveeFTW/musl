#ifndef WRITE_H
#define WRITE_H

#include <unistd.h>
#include <sys/uio.h>

ssize_t __vita_writev(int fd, const struct iovec *iov, int count);

#endif // WRITE_H
