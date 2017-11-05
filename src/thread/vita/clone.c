#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <sched.h>

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

int __clone(int (*func)(void *), void *stack, int flags, void *arg, pid_t *ptid, void *newtls, pid_t *ctid)
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

    // ensure that CLONE_VM is set
    if (!(flags & CLONE_VM))
    {
        sceClibPrintf("musl: cannot create thread without CLONE_VM\n");
        return -ENOSYS;
    }

    // ensure that CLONE_FS is set
    if (!(flags & CLONE_FS))
    {
        sceClibPrintf("musl: cannot create thread without CLONE_FS\n");
        return -ENOSYS;
    }

    // ensure that CLONE_FILES is set
    if (!(flags & CLONE_FILES))
    {
        sceClibPrintf("musl: cannot create thread without CLONE_FILES\n");
        return -ENOSYS;
    }

    // ensure that CLONE_THREAD is set
    if (!(flags & CLONE_THREAD))
    {
        sceClibPrintf("musl: cannot create thread without CLONE_THREAD\n");
        return -ENOSYS;
    }

    SceKernelMemBlockInfo info;
    info.size = sizeof(SceKernelMemBlockInfo);

    int res = sceKernelGetMemBlockInfoByAddr(stack, &info);

    res = sceKernelCreateThread("muslclone", __vita_clone_entry, 0x10000100, info.mappedSize, 0, 0, NULL);

    if (res < 0)
        return -EINVAL;

    // check if there is a TLS value to be used
    if (flags & CLONE_SETTLS)
    {
        // TODO: change fixed value
        void **p = sceKernelGetThreadTLSAddr(res, 0x88);
        *p = newtls;
    }

    // set the thread id in parent memory (same VA)
    if (flags & CLONE_PARENT_SETTID)
    {
        *ptid = res;
    }

    if (flags & CLONE_CHILD_CLEARTID)
    {
        sceClibPrintf("## MUSL: TODO: CLONE_CHILD_CLEARTID\n");
    }

    // CLONE_DETACHED is a no-op

    unsigned int *params[2];
    params[0] = (unsigned int *)func;
    params[1] = (unsigned int *)arg;

    res = sceKernelStartThread(res, sizeof(params), params);
    return 0;
}
