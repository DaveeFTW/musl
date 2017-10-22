#include "socket.h"
#include "fd.h"

#include <errno.h>

#include <psp2/net/net.h>

static int convert_sce_to_errno(int err)
{
    switch (err)
    {
    case SCE_NET_EPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    }

    // default we keep it the same
    return err;
}

int __vita_socket(int domain, int type, int protocol)
{
    int s = sceNetSocket("musl-socket", domain, type, protocol);

    if (s < 0)
    {
        return -convert_sce_to_errno(s & 0xFF);
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
