#include "tls.h"
#include "pthread_impl.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>

// at this index we will store the expected pointer to the musl
// TLS memory
#define TLS_PTR_INDEX (0x88)

int __vita_set_tls(void *p)
{
    sceClibPrintf("pthread size: 0x%X\n", sizeof(struct pthread));
    void **key = sceKernelGetTLSAddr(TLS_PTR_INDEX);
    *key = p;
    return 0;
}
