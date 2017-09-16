#ifndef READ_H
#define READ_H

#include <stddef.h>
#include <unistd.h>

ssize_t __vita_read(int fd, void *buf, size_t count);

#endif // READ_H
