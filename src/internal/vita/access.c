#include "access.h"

#include <unistd.h>
#include <errno.h>

#include <psp2/io/stat.h>

int __vita_access(const char *path, int mode)
{
    // TODO: handle properly
    SceIoStat stat;

    int res = sceIoGetstat(path, &stat);

    if (res < 0)
    {
        return -(res & 0xFF);
    }

    if (mode == F_OK)
        return 0;

    mode &= ~X_OK;

    if (mode & R_OK)
    {
        if (stat.st_mode & SCE_S_IROTH)
            mode &= ~R_OK;
    }

    if (mode & W_OK)
    {
        if (stat.st_mode & SCE_S_IWOTH)
            mode &= ~W_OK;
    }

    if (mode)
        return -EACCES;

    return 0;
}
