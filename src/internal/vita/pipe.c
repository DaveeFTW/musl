#include "pipe.h"
#include "fd.h"

#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>

int __vita_pipe2(int pipefd[], int flags)
{
    int fd = sceKernelCreateMsgPipe("musl-pipe", 0x40, 4 | 8, 0x1000, NULL);

    if (fd < 0)
    {
        // TODO: better error
        sceClibPrintf("error 0x%08X creating msg pipe\n", fd);
        return -1;
    }

    int fd1 = __vita_acquire_descriptor();

    if (fd1 < 0)
    {
        sceKernelDeleteMsgPipe(fd);
        return -EMFILE;
    }

    DescriptorTranslation *fdmap = __vita_fd_grab(fd1);

    fdmap->sce_uid = fd;
    fdmap->type = VITA_DESCRIPTOR_PIPE;
    fdmap->flags = flags;

    int fd2 = __vita_duplicate_descriptor(fd1);

    pipefd[0] = fd1;
    pipefd[1] = fd2;

    __vita_fd_drop(fdmap);
    return 0;
}
