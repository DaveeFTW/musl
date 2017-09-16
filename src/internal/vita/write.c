#include "write.h"
#include "fd.h"

#include <errno.h>

#include <psp2/io/fcntl.h>

ssize_t __vita_writev(int fd, const struct iovec *iov, int count)
{
    ssize_t size = 0;
    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    if (!fdmap)
        return -EBADF;

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
