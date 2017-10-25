#include "fcntl.h"
#include "fd.h"

#include <fcntl.h>
#include <errno.h>

#include <psp2/kernel/clib.h>
#include <psp2/net/net.h>

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

    else if (cmd == F_SETFL)
    {
        // TODO: implement rest of operations
        // see man page
        int blocking = (arg & O_NONBLOCK) != 0;

        if (f->type == VITA_DESCRIPTOR_SOCKET)
        {
            sceNetSetsockopt(f->sce_uid, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &blocking, sizeof(blocking));
            __vita_fd_drop(f);
            return 0;
        }
    }

    sceClibPrintf("musl: unhandled fcntl: %i cmd=%i %i\n", fd, cmd, arg);
    __vita_fd_drop(f);
    return -ENOSYS;
}
