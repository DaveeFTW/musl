#ifndef READ_H
#define READ_H

#include <stddef.h>
#include <unistd.h>
#include <sys/uio.h>

ssize_t __vita_read(int fd, void *buf, size_t count);
ssize_t __vita_readv(int fd, const struct iovec *iov, int iovcnt);

#endif // READ_H
