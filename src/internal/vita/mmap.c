#include "mmap.h"

#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/clib.h>

static int to_block_flags(int prot, int flags)
{
    sceClibPrintf("prot: 0x%X, flags: 0x%X\n", prot, flags);
    if (((prot & (PROT_WRITE | PROT_READ)) == (PROT_WRITE | PROT_READ)) && (flags & MAP_PRIVATE))
    {
        return SCE_KERNEL_MEMBLOCK_TYPE_USER_RW;
    }

    sceClibPrintf("invalid flags\n");
    return -1;
}

void *__vita_mmap(void *start, size_t len, int prot, int flags, int fd, off_t off)
{
    // we don't support file mapping yet
    if (fd != -1)
    {
        sceClibPrintf("mmap: cant create file mappings\n");
        return (void *)-ENOSYS;
    }

    // we don't support WX mappings
    if ((prot & (PROT_EXEC | PROT_WRITE)) == (PROT_EXEC | PROT_WRITE))
    {
        sceClibPrintf("mmap: cant create WX mappings\n");
        return (void *)-EPERM;
    }

    // we don't support shared mappings
    if (flags & MAP_SHARED)
    {
        sceClibPrintf("mmap: cant create shared mappings\n");
        return (void *)-ENOSYS;
    }

    // allocate memory
    SceUID blkid = sceKernelAllocMemBlock("mmap", to_block_flags(prot, flags), len, 0);

    if (blkid < 0)
    {
        sceClibPrintf("blkid 0x%08X\n", blkid);
        return (void *)-ENOMEM;
    }

    void *addr = NULL;
    sceKernelGetMemBlockBase(blkid, &addr);

    if (flags & (MAP_ANON | MAP_ANONYMOUS))
        memset(addr, 0, len);

    sceClibPrintf("got addr 0x%08X\n", addr);
    return addr;
}

int __vita_munmap(void *start, size_t len)
{
    SceUID block = sceKernelFindMemBlockByAddr(start, len);

    if (block < 0)
        return -EINVAL;

    sceKernelFreeMemBlock(block);
    return 0;
}
