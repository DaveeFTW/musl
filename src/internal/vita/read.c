#include "read.h"
#include "fd.h"
#include "sceerrno.h"

#include <fcntl.h>
#include <errno.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>

static ssize_t read_pipe(DescriptorTranslation *f, void *buf, size_t count)
{
    size_t readSize = 0;
    int blocking = (f->flags & O_NONBLOCK) ? (0x10) : (0);
    int res = sceKernelReceiveMsgPipe(f->sce_uid, buf, count, blocking, &readSize, NULL);

    if (res < 0)
    {
        return -EINVAL;
    }

    return readSize;
}

static ssize_t read_file(DescriptorTranslation *f, void *buf, size_t count)
{
    int res = sceIoRead(f->sce_uid, buf, count);

    if (res < 0)
    {
        return -EINVAL;
    }

    return res;
}

static ssize_t read_socket(DescriptorTranslation *f, void *buf, size_t count)
{
    int res = sceNetRecv(f->sce_uid, buf, count, 0);

    sceClibPrintf("sceNetRecv: 0x%08X\n", res);

    if (res < 0)
    {
        return -__vita_sce_errno_to_errno(res);
    }

    return res;
}

ssize_t __vita_read(int fd, void *buf, size_t count)
{
    ssize_t read = -1;
    DescriptorTranslation *f = __vita_fd_grab(fd);

    if (!f)
        return -EBADF;

    switch (f->type)
    {
    case VITA_DESCRIPTOR_PIPE:
        read = read_pipe(f, buf, count);
        break;
    case VITA_DESCRIPTOR_FILE:
        read = read_file(f, buf, count);
        break;
    case VITA_DESCRIPTOR_SOCKET:
        read = read_socket(f, buf, count);
        break;
    default:
        sceClibPrintf("musl: read: unhandled descriptor %i, %i\n", fd, f->type);
        break;
    }

    __vita_fd_drop(f);
    return read;
}

ssize_t __vita_readv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t size = 0;
    DescriptorTranslation *f = __vita_fd_grab(fd);

    if (!f)
        return -EBADF;

    for (int i = 0; i < iovcnt; ++i)
    {
        int read = 0;

        switch (f->type)
        {
        case VITA_DESCRIPTOR_PIPE:
            read = read_pipe(f, iov[i].iov_base, iov[i].iov_len);
            break;
        case VITA_DESCRIPTOR_FILE:
            read = read_file(f, iov[i].iov_base, iov[i].iov_len);
            break;
        case VITA_DESCRIPTOR_SOCKET:
            read = read_socket(f, iov[i].iov_base, iov[i].iov_len);
            break;
        default:
            sceClibPrintf("musl: read: unhandled descriptor %i, %i\n", fd, f->type);
            break;
        }

        if (read < 0)
        {
            __vita_fd_drop(f);
            return -(read & 0xFF);
        }

        size += read;
    }

    __vita_fd_drop(f);
    return size;
}
