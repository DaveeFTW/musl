#include "getpeername.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>

#include <psp2/net/net.h>

int __vita_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    int res = sceNetGetpeername(f->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

    if (res < 0)
    {
        res = -__vita_sce_errno_to_errno(res);
    }

    __vita_fd_drop(f);
    return res;
}
