#include "write.h"
#include "fd.h"

#include <errno.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>

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
