#include "listen.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>

#include <psp2/net/net.h>

int __vita_listen(int sockfd, int backlog)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    int res = sceNetListen(f->sce_uid, backlog);

    __vita_fd_drop(f);

    if (res < 0)
        return -__vita_sce_errno_to_errno(res);

    return 0;
}
