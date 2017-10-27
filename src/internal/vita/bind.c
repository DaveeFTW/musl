#include "bind.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>

#include <psp2/net/net.h>

int __vita_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    int res = sceNetBind(f->sce_uid, (const SceNetSockaddr *)addr, addrlen);

    __vita_fd_drop(f);

    if (res < 0)
        return -__vita_sce_errno_to_errno(res);

    return 0;
}
