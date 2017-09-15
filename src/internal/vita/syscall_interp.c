#include <errno.h>

extern int __vita_external_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6) __attribute__((weak));

int __vita_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6)
{
    switch (n)
    {
    default:
        if (__vita_external_syscall_interp)
            return __vita_external_syscall_interp(n, r1, r2, r3, r4, r5, r6);
    }

    return -ENOSYS;
}
