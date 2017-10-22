#include "getcwd.h"

#include <string.h>
#include <errno.h>

int __vita_getcwd(char *buf, size_t size)
{
    // TODO: emulate this layer?
    const char *cwd = "app0:";

    if (strlen(cwd)+1 > size)
    {
        return -ERANGE;
    }

    strncpy(buf, cwd, size);
    return 0;
}
