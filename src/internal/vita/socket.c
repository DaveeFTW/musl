#include "socket.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>

#include <psp2/net/net.h>

int __vita_socket(int domain, int type, int protocol)
{
    int s = sceNetSocket("musl-socket", domain, type, protocol);

    if (s < 0)
    {
        return -__vita_sce_errno_to_errno(s);
    }

    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        sceNetSocketClose(s);
        return -EMFILE;
    }

    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    fdmap->sce_uid = s;
    fdmap->type = VITA_DESCRIPTOR_SOCKET;
    fdmap->flags = 0;

    __vita_fd_drop(fdmap);
    return fd;
}
