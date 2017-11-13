#include "stat.h"

#undef st_atime
#undef st_ctime
#undef st_mtime

#include <errno.h>
#include <string.h>

#include <psp2/io/stat.h>
#include <psp2/rtc.h>

static mode_t convert_mode(SceMode sce_mode, unsigned int attr)
{
    mode_t mode = 0;

    if (attr & SCE_SO_IXOTH || sce_mode & SCE_S_IXOTH)
        mode |= S_IXOTH;
    if (attr & SCE_SO_IWOTH || sce_mode & SCE_S_IWOTH)
        mode |= S_IWOTH;
    if (attr & SCE_SO_IROTH || sce_mode & SCE_S_IROTH)
        mode |= S_IROTH;
    if (sce_mode & SCE_S_IXUSR)
        mode |= S_IXUSR;
    if (sce_mode & SCE_S_IWUSR)
        mode |= S_IWUSR;
    if (sce_mode & SCE_S_IRUSR)
        mode |= S_IRUSR;
    if (sce_mode & SCE_S_IXGRP)
        mode |= S_IXGRP;
    if (sce_mode & SCE_S_IWGRP)
        mode |= S_IWGRP;
    if (sce_mode & SCE_S_IRGRP)
        mode |= S_IRGRP;
    if (sce_mode & SCE_S_ISVTX)
        mode |= S_ISVTX;
    if (sce_mode & SCE_S_ISGID)
        mode |= S_ISGID;
    if (sce_mode & SCE_S_ISUID)
        mode |= S_ISUID;
    if (attr & SCE_SO_IFLNK || sce_mode & SCE_S_IFLNK)
        mode |= S_IFLNK;
    if (attr & SCE_SO_IFDIR || sce_mode & SCE_S_IFDIR)
        mode |= S_IFDIR;
    if (attr & SCE_SO_IFREG || sce_mode & SCE_S_IFREG)
        mode |= S_IFREG;

    return mode;
}

static void convert_time(struct timespec *time1, const SceDateTime *time2)
{
    time1->tv_nsec = time2->microsecond*1000;
    sceRtcGetTime_t(time2, &time1->tv_sec);
}

static void convert_stat(struct stat *buf, const SceIoStat *stat)
{
    memset(buf, 0, sizeof(struct stat));

    buf->st_mode = convert_mode(stat->st_mode, stat->st_attr);
    buf->st_size = stat->st_size;
    buf->st_nlink = 1;
    convert_time(&buf->st_atim, &stat->st_atime);
    convert_time(&buf->st_mtim, &stat->st_mtime);
    convert_time(&buf->st_ctim, &stat->st_ctime);
}

int __vita_stat64(const char *path, struct stat *buf)
{
    SceIoStat stat;
    int res = sceIoGetstat(path, &stat);

    if (res < 0)
    {
        return -(res & 0xFF);
    }

    convert_stat(buf, &stat);
    return 0;
}
