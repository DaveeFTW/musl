#ifndef MMAP_H
#define MMAP_H

#include <stddef.h>
#include <sys/types.h>

void *__vita_mmap(void *start, size_t len, int prot, int flags, int fd, off_t off);
int  __vita_munmap(void *start, size_t len);

#endif // MMAP_H
