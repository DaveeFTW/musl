#include "ioctl.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>
#include <sys/ioctl.h>

#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>

int __vita_ioctl(int fd, unsigned long request, int arg1)
{
    int res = 0;
    DescriptorTranslation *f = __vita_fd_grab(fd);

    if (!f)
        return -EBADF;

    if (request == FIONREAD)
    {
        if (f->type == VITA_DESCRIPTOR_SOCKET)
        {
            SceNetSockInfo info;
            int res = sceNetGetSockInfo(f->sce_uid, &info, 1, 0);

            if (res < 0)
            {
                res = -__vita_sce_errno_to_errno(res);
            }
            else
            {
                *(unsigned int *)arg1 = info.recv_queue_length;
            }
        }
    }
    else
    {
        sceClibPrintf("musl: got unsup ioctl: %i 0x%X\n", fd, request);
        res = -ENOSYS;
    }

    __vita_fd_drop(f);
    return res;
}
