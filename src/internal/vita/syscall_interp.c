#include "syscall.h"
#include "syscall_interface.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>

#include <errno.h>

extern int __vita_external_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6) __attribute__((weak));

int __vita_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6)
{
    switch (n)
    {
    case SYS_exit:
        return sceKernelExitDeleteThread(r1);
    case SYS_exit_group:
        return sceKernelExitProcess(r1);
    case SYS_geteuid:
    case SYS_getuid:
    case SYS_getegid32:
    case SYS_getpid:
        return sceKernelGetProcessId();
    case SYS_gettid:
        return sceKernelGetThreadId();
    case SYS_tkill:
        sceClibPrintf("got tkill for: %i %i\n", r1, r2);
        return 0;
    default:
        if (__vita_external_syscall_interp)
            return __vita_external_syscall_interp(n, r1, r2, r3, r4, r5, r6);
        else
            sceClibPrintf("musl: unhandled syscall called: %i: %s\n", n, __lookup_syscall_name(n));
    }

    return -ENOSYS;
}
