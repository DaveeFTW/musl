#include "fcntl.h"
#include "fd.h"

#include <fcntl.h>
#include <errno.h>

#include <psp2/kernel/clib.h>

int __vita_fcntl64(int fd, int cmd, int arg)
{
    DescriptorTranslation *f = __vita_fd_grab(fd);

    if (!f)
    {
        return -EBADF;
    }
    if (cmd == F_SETFD)
    {
        f->flags = arg;
        __vita_fd_drop(f);
        return 0;
    }

    sceClibPrintf("musl: unhandled fcntl: %i cmd=%i %i\n", fd, cmd, arg);
    __vita_fd_drop(f);
    return -ENOSYS;
}
