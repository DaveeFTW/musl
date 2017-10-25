#include "fd.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>

#define SCE_ERRNO_MASK 0xFF

static DescriptorTranslation *__vita_fdmap[MAX_OPEN_FILES];
static DescriptorTranslation __vita_fdmap_pool[MAX_OPEN_FILES];

static SceKernelLwMutexWork g_musl_fd_mutex __attribute__ ((aligned (8)));

static inline int is_fd_valid(int fd)
{
    return (fd > 0) && (fd < MAX_OPEN_FILES) && (__vita_fdmap[fd] != NULL);
}

void _init_vita_io(void)
{
    int ret;
    sceKernelCreateLwMutex(&g_musl_fd_mutex, "fd conversion table mutex", 0, 1, NULL);

    memset(__vita_fdmap, 0, sizeof(__vita_fdmap));
    memset(__vita_fdmap_pool, 0, sizeof(__vita_fdmap_pool));

    // TODO: do we prefer sceKernelStdin and friends?
    ret = sceIoOpen("tty0:", SCE_O_RDONLY, 0);

    if (ret >= 0) {
        __vita_fdmap[STDIN_FILENO] = &__vita_fdmap_pool[STDIN_FILENO];
        __vita_fdmap[STDIN_FILENO]->sce_uid = ret;
        __vita_fdmap[STDIN_FILENO]->type = VITA_DESCRIPTOR_TTY;
        __vita_fdmap[STDIN_FILENO]->ref_count = 1;
    }

    ret = sceIoOpen("tty0:", SCE_O_WRONLY, 0);

    if (ret >= 0) {
        __vita_fdmap[STDOUT_FILENO] = &__vita_fdmap_pool[STDOUT_FILENO];
        __vita_fdmap[STDOUT_FILENO]->sce_uid = ret;
        __vita_fdmap[STDOUT_FILENO]->type = VITA_DESCRIPTOR_TTY;
        __vita_fdmap[STDOUT_FILENO]->ref_count = 1;
    }

    ret = sceIoOpen("tty0:", SCE_O_WRONLY, 0);

    if (ret >= 0) {
        __vita_fdmap[STDERR_FILENO] = &__vita_fdmap_pool[STDERR_FILENO];
        __vita_fdmap[STDERR_FILENO]->sce_uid = ret;
        __vita_fdmap[STDERR_FILENO]->type = VITA_DESCRIPTOR_TTY;
        __vita_fdmap[STDERR_FILENO]->ref_count = 1;
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
}

void _free_vita_io(void)
{
    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    if (__vita_fdmap[STDIN_FILENO]) {
        sceIoClose(__vita_fdmap[STDIN_FILENO]->sce_uid);
        memset(__vita_fdmap[STDIN_FILENO], 0, sizeof(DescriptorTranslation));
        __vita_fdmap[STDIN_FILENO] = NULL;
    }
    if (__vita_fdmap[STDOUT_FILENO]) {
        sceIoClose(__vita_fdmap[STDOUT_FILENO]->sce_uid);
        memset(__vita_fdmap[STDOUT_FILENO], 0, sizeof(DescriptorTranslation));
        __vita_fdmap[STDOUT_FILENO] = NULL;
    }
    if (__vita_fdmap[STDERR_FILENO]) {
        sceIoClose(__vita_fdmap[STDERR_FILENO]->sce_uid);
        memset(__vita_fdmap[STDERR_FILENO], 0, sizeof(DescriptorTranslation));
        __vita_fdmap[STDERR_FILENO] = NULL;
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    sceKernelDeleteLwMutex(&g_musl_fd_mutex);
}

int __vita_acquire_descriptor(void)
{
    int fd = -1;
    int i = 0;
    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    // get free descriptor
    // only allocate descriptors after stdin/stdout/stderr -> aka 0/1/2
    for (fd = 3; fd < MAX_OPEN_FILES; ++fd)
    {
        if (__vita_fdmap[fd] == NULL)
        {
            // get free pool
            for (i = 0; i < MAX_OPEN_FILES; ++i)
            {
                if (__vita_fdmap_pool[i].ref_count == 0)
                {
                    __vita_fdmap[fd] = &__vita_fdmap_pool[i];
                    __vita_fdmap[fd]->ref_count = 1;
                    __vita_fdmap[fd]->flags = 0;
                    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
                    return fd;
                }
            }
        }
    }

    // no mores descriptors available...
    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return -1;
}

int __vita_release_descriptor(int fd)
{
    DescriptorTranslation *map = NULL;
    int res = -1;

    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    if (is_fd_valid(fd) && __vita_fd_drop(__vita_fdmap[fd]) >= 0)
    {
        __vita_fdmap[fd] = NULL;
        res = 0;
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return res;
}

int __vita_duplicate_descriptor(int fd)
{
    int fd2 = -1;

    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    if (is_fd_valid(fd))
    {
        // get free descriptor
        // only allocate descriptors after stdin/stdout/stderr -> aka 0/1/2
        for (fd2 = 3; fd2 < MAX_OPEN_FILES; ++fd2)
        {
            if (__vita_fdmap[fd2] == NULL)
            {
                __vita_fdmap[fd2] = __vita_fdmap[fd];
                __vita_fdmap[fd2]->ref_count++;
                sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
                return fd2;
            }
        }
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return -1;
}

int __vita_descriptor_ref_count(int fd)
{
    int res = 0;
    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);
    res = __vita_fdmap[fd]->ref_count;
    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return res;
}

DescriptorTranslation *__vita_fd_grab(int fd)
{
    DescriptorTranslation *map = NULL;

    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    if (is_fd_valid(fd))
    {
        map = __vita_fdmap[fd];

        if (map)
            map->ref_count++;
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return map;
}

int __vita_fd_drop(DescriptorTranslation *map)
{
    sceKernelLockLwMutex(&g_musl_fd_mutex, 1, 0);

    if (map->ref_count == 1)
    {
        int ret = 0;

        switch (map->type)
        {
        case VITA_DESCRIPTOR_FILE:
        case VITA_DESCRIPTOR_TTY:
        {
            ret = sceIoClose(map->sce_uid);
            break;
        }
        case VITA_DESCRIPTOR_PIPE:
            sceClibPrintf("musl: close not implemented for PIPE\n");
            ret = 0;
            break;
        case VITA_DESCRIPTOR_SOCKET:
            ret = sceNetSocketClose(map->sce_uid);
            break;
        }

        if (ret < 0) {
            sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
            return -(ret & SCE_ERRNO_MASK);
        }

        map->ref_count--;
        memset(map, 0, sizeof(DescriptorTranslation));
    }
    else
    {
        map->ref_count--;
    }

    sceKernelUnlockLwMutex(&g_musl_fd_mutex, 1);
    return 0;
}
