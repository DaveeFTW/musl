#include "poll.h"
#include "fd.h"

#include <sys/uio.h>
#include <string.h>
#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/net/net.h>

#include <sys/ioctl.h>

// TODO: add to SDK
#define MSG_PIPE_PEEK   (0x100)

static int poll_peek_in_socket(DescriptorTranslation *f)
{
    SceNetEpollEvent event = { 0 };

    int eid = sceNetEpollCreate("musl-epoll", 0);

    event.events = SCE_NET_EPOLLIN;

    sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, f->sce_uid, &event);
    sceNetEpollWait(eid, &event, 1, 0);
    sceNetEpollDestroy(eid);

    return event.events & SCE_NET_EPOLLIN;
}

static int poll_peek_out_socket(DescriptorTranslation *f)
{
    SceNetEpollEvent event = { 0 };

    int eid = sceNetEpollCreate("musl-epoll", 0);

    event.events = SCE_NET_EPOLLOUT;

    sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, f->sce_uid, &event);
    sceNetEpollWait(eid, &event, 1, 0);
    sceNetEpollDestroy(eid);

    return event.events & SCE_NET_EPOLLOUT;
}

static int poll_peek_pipe(DescriptorTranslation *f)
{
    char tmp;
    return sceKernelTryReceiveMsgPipe(f->sce_uid, &tmp, sizeof(char), MSG_PIPE_PEEK, NULL) == 0;
}

static int is_pollin_ready(DescriptorTranslation *f)
{
    switch (f->type)
    {
    case VITA_DESCRIPTOR_TTY:
        sceClibPrintf("musl: poll() POLLIN not supported for TTY\n");
        return 0;
    case VITA_DESCRIPTOR_FILE:
        sceClibPrintf("musl: poll() POLLIN not supported for FILE\n");
        return 0;
    case VITA_DESCRIPTOR_SOCKET:
        return poll_peek_in_socket(f);
    case VITA_DESCRIPTOR_PIPE:
        return poll_peek_pipe(f);
    default:
        // TODO: error?
        sceClibPrintf("musl: unexpected descriptor type in poll()\n");
        return 0;
    }
}

static int is_pollout_ready(DescriptorTranslation *f)
{
    switch (f->type)
    {
    case VITA_DESCRIPTOR_TTY:
        sceClibPrintf("musl: poll() POLLOUT not supported for TTY\n");
        return 0;
    case VITA_DESCRIPTOR_FILE:
        sceClibPrintf("musl: poll() POLLOUT not supported for FILE\n");
        return 0;
    case VITA_DESCRIPTOR_SOCKET:
        return poll_peek_out_socket(f);
    case VITA_DESCRIPTOR_PIPE:
        sceClibPrintf("musl: poll() POLLOUT not supported for PIPE\n");
        return 0;
    default:
        // TODO: error?
        sceClibPrintf("musl: unexpected descriptor type in poll()\n");
        return 0;
    }
}

int __vita_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    // Number of selected fd's
    int selected = 0;
    int elapsed_time = 0;

    while (elapsed_time < timeout || timeout == -1)
    {
        for (int i = 0; i < nfds; ++i)
        {
            struct pollfd *fd = &fds[i];

            // The field fd contains a file descriptor for an open file. If this
            // field is negative, then the corresponding events field is ignored and
            // the revents field returns zero.
            if (fd->fd < 0)
            {
                fd->revents = 0;
                continue;
            }

            DescriptorTranslation *f = __vita_fd_grab(fd->fd);

            if (!f)
            {
                sceClibPrintf("musl: poll! bad fd\n");
                // we have a bad file descriptor
                return -EBADF;
            }

            fd->revents = 0;

            int poll_in = 0, poll_out = 0;

            // There is data to be read.
            if (fd->events & POLLIN)
            {
                if (is_pollin_ready(f))
                {
                    poll_in = 1;
                    fd->revents |= POLLIN;
                }
            }

            // Writing is now possible, though a write larger than available
            // space in a socket or pipe will still block (unless O_NONBLOCK
            // is set).
            if (fd->events & POLLOUT)
            {
                if (is_pollout_ready(f))
                {
                    poll_out = 1;
                    fd->revents |= POLLOUT;
                }
            }

            __vita_fd_drop(f);

            if (poll_in || poll_out)
            {
                ++selected;
            }
        }

        if (selected)
            break;

        // TODO: improve - this is very crude
        sceKernelDelayThread(100);
        elapsed_time += 100;
    }

    return selected;
}
