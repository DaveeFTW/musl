#include "open.h"
#include "fd.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/clib.h>

static int _fcntl2sony(int flags)
{
    int out = 0;
    if (flags & O_RDWR)
        out |= SCE_O_RDWR;
    else if (flags & O_WRONLY)
        out |= SCE_O_WRONLY;
    else
        out |= SCE_O_RDONLY;
    if (flags & O_NONBLOCK)
        out |= SCE_O_NBLOCK;
    if (flags & O_APPEND)
        out |= SCE_O_APPEND;
    if (flags & O_CREAT)
        out |= SCE_O_CREAT;
    if (flags & O_TRUNC)
        out |= SCE_O_TRUNC;
    if (flags & O_EXCL)
        out |= SCE_O_EXCL;
    return out;
}

static int open_directory(const char *path, int oflags)
{
    int res  = sceIoDopen(path);

    if (res < 0)
    {
        // TODO: check if this makes any sense
        return -(res & 0xFF);
    }

    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        sceIoDclose(res);
        return -EMFILE;
    }

    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    fdmap->sce_uid = res;
    fdmap->type = VITA_DESCRIPTOR_DIRECTORY;
    fdmap->flags = oflags;

    __vita_fd_drop(fdmap);
    return fd;
}

int __vita_open(const char *path, int oflag)
{
    int is_tty = 0;
    sceClibPrintf("trying to open %s\n", path);

    if (oflag & O_DIRECTORY)
    {
        return open_directory(path, oflag);
    }

    if (strcmp(path, "/dev/tty") == 0)
    {
        path = "tty0:";
        is_tty = 1;
    }

    int ret;
    int flags = _fcntl2sony(oflag);

    ret = sceIoOpen(path, flags, 0666);

    if (ret < 0)
    {
        return -(ret & 0xFF);
    }

    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        sceIoClose(ret);
        return -EMFILE;
    }

    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    fdmap->sce_uid = ret;
    fdmap->type = is_tty ? VITA_DESCRIPTOR_TTY : VITA_DESCRIPTOR_FILE;
    fdmap->flags = oflag;

    __vita_fd_drop(fdmap);
    return fd;
}
