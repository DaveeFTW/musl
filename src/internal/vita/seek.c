#include "seek.h"
#include "fd.h"

#include <fcntl.h>
#include <errno.h>

#include <psp2/io/fcntl.h>

int __vita__llseek(unsigned int fd, unsigned long offset_high, unsigned long offset_low, off_t *result, unsigned int whence)
{
    off_t res;
    off_t offset = ((off_t)offset_high << 32) | offset_low;
    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    switch (fdmap->type)
    {
    case VITA_DESCRIPTOR_FILE:
        res = sceIoLseek(fdmap->sce_uid, offset, whence);
        break;
    case VITA_DESCRIPTOR_TTY:
    case VITA_DESCRIPTOR_SOCKET:
        res = EBADF;
        break;
    }

    __vita_fd_drop(fdmap);

    if (res < 0)
    {
        return -(res & 0xFF);
    }
    else
    {
        *result = res;
    }

    return 0;
}
