#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>

#include <stdio.h>
#include <errno.h>

// TODO: contribute to SDK
typedef struct
{
    SceSize size; //0
    SceUInt32 attr; // 4
    SceUInt32 unk_8; // 8
    SceUInt32 unk_c; // c
    SceUInt32 unk_10; // 10
    SceUID stack_block_uid; // 14
    SceUInt32 unk_18; // 18
} SceKernelThreadOptParam2; // 0x1c

int __vita_clone_entry(SceSize args, void *argp)
{
    unsigned int **params = (unsigned int **)argp;
    int (* func)(void *) = (int (*)(void *))params[0];
    return func(params[1]);
}

int __clone(int (*func)(void *), void *stack, int flags, void *arg, ...)
{
#if WE_HAD_KERNEL_IMPL
    SceKernelMemBlockInfo info;
    info.size = sizeof(SceKernelMemBlockInfo);

    int res = sceKernelGetMemBlockInfoByAddr(stack, &info);

    SceUID block = sceKernelFindMemBlockByAddr(info.mappedBase, info.mappedSize);

    fprintf(stderr, "sceKernelGetMemBlockInfoByAddr(0x%08X): 0x%08X\n", stack, res);
    fprintf(stderr, "base 0x%08X, size 0x%08X\n", info.mappedBase, info.mappedSize);
    fprintf(stderr, "block: 0x%08X\n", block);

    // we already allocated a stack, let the kernel know to use it
    SceKernelThreadOptParam2 options;
    options.size = sizeof(SceKernelThreadOptParam2);
    options.attr = 8;
    options.stack_block_uid = block;

    res = sceKernelCreateThread("muslpthread", __vita_clone_entry, 0x10000100, info.mappedSize, 0, 0, (SceKernelThreadOptParam *)&options);

    fprintf(stderr, "sceKernelCreateThread: 0x%08X\n", res);
#endif

    SceKernelMemBlockInfo info;
    info.size = sizeof(SceKernelMemBlockInfo);

    int res = sceKernelGetMemBlockInfoByAddr(stack, &info);

    res = sceKernelCreateThread("muslclone", __vita_clone_entry, 0x10000100, info.mappedSize, 0, 0, NULL);

    sceClibPrintf("sceKernelCreateThread: 0x%08X\n", res);

    if (res < 0)
        return -EINVAL;

    unsigned int *params[2];
    params[0] = (unsigned int *)func;
    params[1] = (unsigned int *)arg;

    res = sceKernelStartThread(res, sizeof(params), params);

    sceClibPrintf("sceKernelStartThread 0x%08X\n", res);
    return 0;
}
