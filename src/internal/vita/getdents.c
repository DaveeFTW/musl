#define _GNU_SOURCE
#include "getdents.h"
#include "fd.h"

#include <psp2/io/dirent.h>

#include <errno.h>
#include <string.h>
#include <dirent.h>

static unsigned char convert_d_type(SceIoDirent *dirent)
{
    if (SCE_SO_ISLNK(dirent->d_stat.st_mode))
        return DT_LNK;
    else if (SCE_SO_ISREG(dirent->d_stat.st_mode))
        return DT_REG;
    else if (SCE_SO_ISDIR(dirent->d_stat.st_mode))
        return DT_DIR;

    return DT_UNKNOWN;
}

int __vita_getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
{
    DescriptorTranslation *f = __vita_fd_grab(fd);

    if (!f)
        return -EBADF;

    SceIoDirent dirent;
    memset(&dirent, 0, sizeof(dirent));

    int read = 0;

    // don't risk an overflow
    for (int i = 0; i < count/sizeof(struct dirent); ++i, ++read)
    {
        int res = sceIoDread(f->sce_uid, &dirent);

        if (res < 0)
        {
            // TODO: handle error
            return -(res & 0xFF);
        }

        dirp[i].d_ino = 0xDEADBEEF;
        dirp[i].d_off = (i+1)*sizeof(struct dirent);
        dirp[i].d_reclen = sizeof(struct dirent);
        dirp[i].d_type = convert_d_type(&dirent);
        strncpy(dirp[i].d_name, dirent.d_name, sizeof(dirp[i].d_name));

        // check if we're at the last directory
        if (res == 0)
        {
            break;
        }
    }

    return read*sizeof(struct dirent);
}
