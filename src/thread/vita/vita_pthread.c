#include "vita_pthread.h"
#include "pthread_impl.h"

void set_pthread_clear_child_tid(int *ctid)
{
    __pthread_self()->clear_child_tid = ctid;
}
