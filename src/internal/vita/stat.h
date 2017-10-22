#ifndef STAT_H
#define STAT_H

#include <sys/stat.h>

int __vita_stat64(const char *path, struct stat *buf);

#endif // STAT_H
