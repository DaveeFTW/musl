#ifndef FUTEX_H
#define FUTEX_H

#include <time.h>

int __vita_futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);

#endif // FUTEX_H
