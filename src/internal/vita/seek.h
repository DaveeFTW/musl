#ifndef SEEK_H
#define SEEK_H

#include <sys/types.h>

int __vita__llseek(unsigned int fd, unsigned long offset_high, unsigned long offset_low, off_t *result, unsigned int whence);

#endif // SEEK_H
