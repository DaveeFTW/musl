#include "stat.h"

#include <errno.h>

#include <psp2/kernel/clib.h>

int __vita_stat64(const char *path, struct stat *buf)
{
    sceClibPrintf("musl: stat path %s\n", path);
    return -ENOSYS;
}
