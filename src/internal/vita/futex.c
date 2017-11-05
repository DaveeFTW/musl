#include "futex.h"
#include "../futex.h"
#include "pthread_impl.h"

#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>

typedef struct futex_waiter
{
    struct futex_waiter *next;
    SceUID lock;
} futex_waiter;

#define WAITER_TABLE_N   (128)

static futex_waiter g_waiter_table[WAITER_TABLE_N] = { 0 };
static futex_waiter *g_waiter_reserved = NULL;
static futex_waiter *g_waiter_free = NULL;
static SceKernelLwMutexWork g_waiter_lock;

typedef struct uaddr_entry
{
    struct uaddr_entry *next;
    int *uaddr;
    futex_waiter *waiters;
    SceKernelLwMutexWork lock;
} uaddr_entry;

#define UADDR_TABLE_N   (512)

static uaddr_entry g_uaddr_table[UADDR_TABLE_N] = { 0 };
static uaddr_entry *g_uaddr_reserved = NULL;
static uaddr_entry *g_uaddr_free = NULL;
static SceKernelLwMutexWork g_uaddr_lock;

void __vita_init_futex(void)
{
    sceKernelCreateLwMutex(&g_uaddr_lock, "musl-uaddr", 0, 1, NULL);

    // by default no uaddr is going to be reserved
    g_uaddr_reserved = NULL;

    // which implies that every uaddr is free
    g_uaddr_free = &g_uaddr_table[0];

    for (int i = 0; i < UADDR_TABLE_N; ++i)
    {
        // initialise as a linked list
        g_uaddr_table[i].next = (i == UADDR_TABLE_N-1) ? (NULL) : (&g_uaddr_table[i+1]);
        g_uaddr_table[i].uaddr = NULL;
        g_uaddr_table[i].waiters = NULL;
    }

    sceKernelUnlockLwMutex(&g_uaddr_lock, 1);
    sceKernelCreateLwMutex(&g_waiter_lock, "musl-waiter", 0, 1, NULL);

    // by default no waiters will be reserved
    g_waiter_reserved = NULL;

    // which implies that every uaddr is free
    g_waiter_free = &g_waiter_table[0];

    for (int i = 0; i < WAITER_TABLE_N; ++i)
    {
        // initialise as a linked list
        g_waiter_table[i].next = (i == WAITER_TABLE_N-1) ? (NULL) : (&g_waiter_table[i+1]);
    }

    sceKernelUnlockLwMutex(&g_waiter_lock, 1);
}

static uaddr_entry *get_or_create_entry(int *uaddr)
{
    sceKernelLockLwMutex(&g_uaddr_lock, 1, NULL);

    // check for existing uaddr
    uaddr_entry *reserved = g_uaddr_reserved;

    while (reserved)
    {
        if (reserved->uaddr == uaddr)
            break;

        reserved = reserved->next;
    }

    // check if we need to reserve a new uaddr
    if (!reserved)
    {
        // get new uaddr block
        reserved = g_uaddr_free;

        if (!reserved)
        {
            // we ran out of blocks...
            // TODO: handle
        }

        // set new free value
        g_uaddr_free = reserved->next;

        reserved->uaddr = uaddr;
        reserved->waiters = NULL;
        reserved->next = g_uaddr_reserved;

        // TODO: rethink this one
        sceKernelCreateLwMutex(&reserved->lock, "musl-uaddr", 0, 0, NULL);
        g_uaddr_reserved = reserved;
    }

    sceKernelUnlockLwMutex(&g_uaddr_lock, 1);
    return reserved;
}

static void try_release_entry(uaddr_entry *entry)
{
    sceKernelLockLwMutex(&g_uaddr_lock, 1, NULL);

    if (entry->waiters == NULL)
    {
        uaddr_entry *prev_reserved = NULL;
        uaddr_entry *reserved = g_uaddr_reserved;

        while (reserved != entry)
        {
            prev_reserved = reserved;
            reserved = reserved->next;
        }

        if (prev_reserved == NULL)
        {
            g_uaddr_reserved = NULL;
        }
        else
        {
            // remove our waiter from the linked list
            prev_reserved->next = entry->next;
        }

        entry->next = g_uaddr_free;
        g_uaddr_free = entry;
    }

    sceKernelUnlockLwMutex(&g_uaddr_lock, 1);
}

static futex_waiter *reserve_waiter(void)
{
    sceKernelLockLwMutex(&g_waiter_lock, 1, NULL);
    futex_waiter *reserved = g_waiter_free;

    if (!reserved)
    {
        // we ran out of blocks...
        // TODO: handle
    }

    // set new free value
    g_waiter_free = reserved->next;

    // initialise
    reserved->next = g_waiter_reserved;
    reserved->lock = __pthread_self()->waiter_lock;

    g_waiter_reserved = reserved;
    sceKernelUnlockLwMutex(&g_waiter_lock, 1);
    return reserved;
}

static void release_waiter(futex_waiter *waiter)
{
    sceKernelLockLwMutex(&g_waiter_lock, 1, NULL);
    futex_waiter *prev_reserved = NULL;
    futex_waiter *reserved = g_waiter_reserved;

    while (reserved != waiter)
    {
        prev_reserved = reserved;
        reserved = reserved->next;
    }

    if (prev_reserved == NULL)
    {
        g_waiter_reserved = NULL;
    }
    else
    {
        // remove our waiter from the linked list
        prev_reserved->next = waiter->next;
    }

    waiter->next = g_waiter_free;
    g_waiter_free = waiter;

    sceKernelUnlockLwMutex(&g_waiter_lock, 1);
}

static int do_futex_wait(int *uaddr, int val, const struct timespec *timeout)
{
    unsigned int *wait = NULL;
    unsigned int wait_us = 0;

    if (timeout)
    {
        wait_us = timeout->tv_nsec/1000 + timeout->tv_sec*1000*1000;
        wait = &wait_us;
    }

    sceClibPrintf("doing wait on 0x%08X, got timeout: %us\n", uaddr, wait_us);

    uaddr_entry *entry = get_or_create_entry(uaddr);
    futex_waiter *waiter = reserve_waiter();

    sceKernelLockLwMutex(&entry->lock, 1, NULL);

    futex_waiter *end = NULL;
    futex_waiter *q = entry->waiters;

    while (q)
    {
        end = q;
        q = q->next;
    }

    // add to end of queue
    futex_waiter **endq = (end) ? (&end->next) : (&entry->waiters);
    *endq = waiter;

    sceKernelUnlockLwMutex(&entry->lock, 1);

    int res = sceKernelWaitSema(waiter->lock, 1, wait);
    sceClibPrintf("done lock on 0x%08X, res = 0x%08X\n", waiter->lock, res);
    // TODO: handle errors appropriately
    return 0;
}

static int do_futex_wake(int *uaddr, unsigned int val)
{
    sceClibPrintf("doing wake on 0x%08X, %u\n", uaddr, val);
    uaddr_entry *entry = get_or_create_entry(uaddr);

    unsigned int woken = 0;

    sceKernelLockLwMutex(&entry->lock, 1, NULL);

    futex_waiter *waiter = entry->waiters;

    for (woken = 0; woken < val && waiter; ++woken)
    {
        int res = sceKernelSignalSema(waiter->lock, 1);
        sceClibPrintf("woken up: 0x%08X -> 0x%08X\n", waiter->lock, res);
        release_waiter(waiter);
        waiter = waiter->next;
    }

    entry->waiters = waiter;
    sceKernelUnlockLwMutex(&entry->lock, 1);
    try_release_entry(entry);

    return woken;
}

int __vita_futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
{
    // remove futex private
    futex_op -= FUTEX_PRIVATE;

    switch (futex_op)
    {
    case FUTEX_WAIT:
        return do_futex_wait(uaddr, val, timeout);
    case FUTEX_WAKE:
        return do_futex_wake(uaddr, val);
    default:
        sceClibPrintf("musl: UNSUPPORTED FUTEX OP %i\n", futex_op);
        break;
    }

    return -ENOSYS;
}
