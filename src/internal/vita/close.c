#include "close.h"
#include "fd.h"

int __vita_close(int fd)
{
    int res = __vita_release_descriptor(fd);

    if (res < 0)
    {
        return res;
    }

    return 0;
}
