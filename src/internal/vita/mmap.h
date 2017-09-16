#ifndef MMAP_H
#define MMAP_H

#include <stddef.h>
#include <sys/types.h>

void *__vita_mmap(void *start, size_t len, int prot, int flags, int fd, off_t off);

#endif // MMAP_H
