#include "brk.h"
#include <psp2/kernel/sysmem.h>

// compatibility with newlib SDK
extern unsigned int _newlib_heap_size_user __attribute__((weak));

// we have a default heap size of 128 MB
static size_t g_heap_size = 128 * 1024 * 1024;
static SceUID g_heap_uid = -1;
static char *g_heap_base = NULL, *g_heap_end = NULL;

static void init_heap(void)
{
    // check if user has specified a custom heap size
    if (&_newlib_heap_size_user != NULL)
    {
        g_heap_size = _newlib_heap_size_user;
    }

    // TODO: check and report
    g_heap_uid = sceKernelAllocMemBlock("musl_heap", 0x0C20D060, g_heap_size, 0);
    sceKernelGetMemBlockBase(g_heap_uid, (void *)&g_heap_base);

    g_heap_end = g_heap_base + g_heap_size;
}

void *__vita_brk(void *addr)
{
    static char *brk = 0;

    if (!brk)
    {
        init_heap();
        brk = g_heap_base;
    }

    if ((char *)addr >= g_heap_end || (char *)addr < g_heap_base)
    {
            return brk;
    }

    brk = addr;
    return brk;
}
