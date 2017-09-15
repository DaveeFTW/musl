#include "tls.h"

#define TLS_KEY (0x1F4)

int __vita_set_tls(void *p)
{
    volatile char **tls = 0;
    __asm__ __volatile__ ( "mrc p15,0,%0,c13,c0,3" : "=r"(tls) );
    tls = (volatile char **)((unsigned int)tls - 0x30);
    *tls = p;
    return 0;
}
