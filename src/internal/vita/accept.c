#include "accept.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>

#include <psp2/net/net.h>

int __vita_accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    int newsock = sceNetAccept(f->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

    __vita_fd_drop(f);

    if (newsock < 0)
    {
        return -__vita_sce_errno_to_errno(newsock);
    }

    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        return -EMFILE;
    }

    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    fdmap->sce_uid = newsock;
    fdmap->type = VITA_DESCRIPTOR_SOCKET;
    fdmap->flags = flags;

    __vita_fd_drop(fdmap);
    return fd;
}
