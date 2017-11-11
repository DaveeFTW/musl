#include "write.h"
#include "fd.h"

#include <fcntl.h>
#include <errno.h>

#include <psp2/io/fcntl.h>
#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/threadmgr.h>

ssize_t __vita_writev(int fd, const struct iovec *iov, int count)
{
    ssize_t size = 0;
    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    if (!fdmap)
        return -EBADF;

    if (fdmap->type == VITA_DESCRIPTOR_SOCKET || fdmap->type == VITA_DESCRIPTOR_PIPE)
    {
        sceClibPrintf("musl: writev does not support SOCKET or PIPE descriptors\n");
        return -ENOSYS;
    }

    for (int i = 0; i < count; ++i)
    {
        int res = sceIoWrite(fdmap->sce_uid, iov[i].iov_base, iov[i].iov_len);

        if (res < 0)
        {
            __vita_fd_drop(fdmap);
            return -(res & 0xFF);
        }

        size += res;
    }

    __vita_fd_drop(fdmap);
    return size;
}

ssize_t __vita_write(int fd, const void *buf, size_t count)
{
    int res = 0;
    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    if (!fdmap)
        return -EBADF;


    switch (fdmap->type)
    {
    case VITA_DESCRIPTOR_TTY:
    case VITA_DESCRIPTOR_FILE:
        res = sceIoWrite(fdmap->sce_uid, buf, count);

        // TODO: check this is still correct
        if (res < 0)
            res = -(res & 0xFF);
        break;
    case VITA_DESCRIPTOR_SOCKET:
        res = sceNetSend(fdmap->sce_uid, buf, count, 0);

        // TODO: check this is still correct
        if (res < 0)
            res = -(res & 0xFF);
        break;
    case VITA_DESCRIPTOR_PIPE:
        {
            int blocking = (fdmap->flags & O_NONBLOCK) ? (0x10) : (0);

            // TODO: shout at SDK for const-ness... again
            res = sceKernelSendMsgPipe(fdmap->sce_uid, (void *)buf, count, blocking, NULL, NULL);

            // TODO: check this is still correct
            if (res < 0)
                res = -(res & 0xFF);
            break;
        }
    }

    __vita_fd_drop(fdmap);
    return res;
}
