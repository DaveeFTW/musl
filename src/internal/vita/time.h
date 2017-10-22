#ifndef TIME_H
#define TIME_H

#include <time.h>

int __vita_clock_gettime(clockid_t clk_id, struct timespec *tp);

#endif // TIME_H
