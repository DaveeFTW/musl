#include "time.h"

#include <errno.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/rtc.h>

int __vita_clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    if (clk_id == CLOCK_MONOTONIC)
    {
        SceKernelSysClock ticks;
        sceKernelGetProcessTime(&ticks);

        tp->tv_sec = ticks/(1000*1000);
        tp->tv_nsec = (ticks * 1000) % (1000*1000*1000);

        return 0;
    }

    else if (clk_id == CLOCK_REALTIME)
    {
        time_t seconds;
        SceDateTime time;
        sceRtcGetCurrentClockLocalTime(&time);

        sceRtcGetTime_t(&time, &seconds);

        tp->tv_sec = seconds;
        tp->tv_nsec = time.microsecond * 1000;
        return 0;
    }

    return -ENOSYS;
}
