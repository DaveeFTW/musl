#include "sceerrno.h"

#include <errno.h>
#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>

typedef struct
{
    int code;
    const char *name;
} SceErrnoTable;

static SceErrnoTable g_sce_errno_table[] =
{
    { SCE_NET_ERROR_EPERM, "SCE_NET_ERROR_EPERM" },
    { SCE_NET_ERROR_ENOENT, "SCE_NET_ERROR_ENOENT" },
    { SCE_NET_ERROR_ESRCH, "SCE_NET_ERROR_ESRCH" },
    { SCE_NET_ERROR_EINTR, "SCE_NET_ERROR_EINTR" },
    { SCE_NET_ERROR_EIO, "SCE_NET_ERROR_EIO" },
    { SCE_NET_ERROR_ENXIO, "SCE_NET_ERROR_ENXIO" },
    { SCE_NET_ERROR_E2BIG, "SCE_NET_ERROR_E2BIG" },
    { SCE_NET_ERROR_ENOEXEC, "SCE_NET_ERROR_ENOEXEC" },
    { SCE_NET_ERROR_EBADF, "SCE_NET_ERROR_EBADF" },
    { SCE_NET_ERROR_ECHILD, "SCE_NET_ERROR_ECHILD" },
    { SCE_NET_ERROR_EDEADLK, "SCE_NET_ERROR_EDEADLK" },
    { SCE_NET_ERROR_ENOMEM, "SCE_NET_ERROR_ENOMEM" },
    { SCE_NET_ERROR_EACCES, "SCE_NET_ERROR_EACCES" },
    { SCE_NET_ERROR_EFAULT, "SCE_NET_ERROR_EFAULT" },
    { SCE_NET_ERROR_ENOTBLK, "SCE_NET_ERROR_ENOTBLK" },
    { SCE_NET_ERROR_EBUSY, "SCE_NET_ERROR_EBUSY" },
    { SCE_NET_ERROR_EEXIST, "SCE_NET_ERROR_EEXIST" },
    { SCE_NET_ERROR_EXDEV, "SCE_NET_ERROR_EXDEV" },
    { SCE_NET_ERROR_ENODEV, "SCE_NET_ERROR_ENODEV" },
    { SCE_NET_ERROR_ENOTDIR, "SCE_NET_ERROR_ENOTDIR" },
    { SCE_NET_ERROR_EISDIR, "SCE_NET_ERROR_EISDIR" },
    { SCE_NET_ERROR_EINVAL, "SCE_NET_ERROR_EINVAL" },
    { SCE_NET_ERROR_ENFILE, "SCE_NET_ERROR_ENFILE" },
    { SCE_NET_ERROR_EMFILE, "SCE_NET_ERROR_EMFILE" },
    { SCE_NET_ERROR_ENOTTY, "SCE_NET_ERROR_ENOTTY" },
    { SCE_NET_ERROR_ETXTBSY, "SCE_NET_ERROR_ETXTBSY" },
    { SCE_NET_ERROR_EFBIG, "SCE_NET_ERROR_EFBIG" },
    { SCE_NET_ERROR_ENOSPC, "SCE_NET_ERROR_ENOSPC" },
    { SCE_NET_ERROR_ESPIPE, "SCE_NET_ERROR_ESPIPE" },
    { SCE_NET_ERROR_EROFS, "SCE_NET_ERROR_EROFS" },
    { SCE_NET_ERROR_EMLINK, "SCE_NET_ERROR_EMLINK" },
    { SCE_NET_ERROR_EPIPE, "SCE_NET_ERROR_EPIPE" },
    { SCE_NET_ERROR_EDOM, "SCE_NET_ERROR_EDOM" },
    { SCE_NET_ERROR_ERANGE, "SCE_NET_ERROR_ERANGE" },
    { SCE_NET_ERROR_EAGAIN, "SCE_NET_ERROR_EAGAIN" },
    { SCE_NET_ERROR_EWOULDBLOCK, "SCE_NET_ERROR_EWOULDBLOCK" },
    { SCE_NET_ERROR_EINPROGRESS, "SCE_NET_ERROR_EINPROGRESS" },
    { SCE_NET_ERROR_EALREADY, "SCE_NET_ERROR_EALREADY" },
    { SCE_NET_ERROR_ENOTSOCK, "SCE_NET_ERROR_ENOTSOCK" },
    { SCE_NET_ERROR_EDESTADDRREQ, "SCE_NET_ERROR_EDESTADDRREQ" },
    { SCE_NET_ERROR_EMSGSIZE, "SCE_NET_ERROR_EMSGSIZE" },
    { SCE_NET_ERROR_EPROTOTYPE, "SCE_NET_ERROR_EPROTOTYPE" },
    { SCE_NET_ERROR_ENOPROTOOPT, "SCE_NET_ERROR_ENOPROTOOPT" },
    { SCE_NET_ERROR_EPROTONOSUPPORT, "SCE_NET_ERROR_EPROTONOSUPPORT" },
    { SCE_NET_ERROR_ESOCKTNOSUPPORT, "SCE_NET_ERROR_ESOCKTNOSUPPORT" },
    { SCE_NET_ERROR_EOPNOTSUPP, "SCE_NET_ERROR_EOPNOTSUPP" },
    { SCE_NET_ERROR_EPFNOSUPPORT, "SCE_NET_ERROR_EPFNOSUPPORT" },
    { SCE_NET_ERROR_EAFNOSUPPORT, "SCE_NET_ERROR_EAFNOSUPPORT" },
    { SCE_NET_ERROR_EADDRINUSE, "SCE_NET_ERROR_EADDRINUSE" },
    { SCE_NET_ERROR_EADDRNOTAVAIL, "SCE_NET_ERROR_EADDRNOTAVAIL" },
    { SCE_NET_ERROR_ENETDOWN, "SCE_NET_ERROR_ENETDOWN" },
    { SCE_NET_ERROR_ENETUNREACH, "SCE_NET_ERROR_ENETUNREACH" },
    { SCE_NET_ERROR_ENETRESET, "SCE_NET_ERROR_ENETRESET" },
    { SCE_NET_ERROR_ECONNABORTED, "SCE_NET_ERROR_ECONNABORTED" },
    { SCE_NET_ERROR_ECONNRESET, "SCE_NET_ERROR_ECONNRESET" },
    { SCE_NET_ERROR_ENOBUFS, "SCE_NET_ERROR_ENOBUFS" },
    { SCE_NET_ERROR_EISCONN, "SCE_NET_ERROR_EISCONN" },
    { SCE_NET_ERROR_ENOTCONN, "SCE_NET_ERROR_ENOTCONN" },
    { SCE_NET_ERROR_ESHUTDOWN, "SCE_NET_ERROR_ESHUTDOWN" },
    { SCE_NET_ERROR_ETOOMANYREFS, "SCE_NET_ERROR_ETOOMANYREFS" },
    { SCE_NET_ERROR_ETIMEDOUT, "SCE_NET_ERROR_ETIMEDOUT" },
    { SCE_NET_ERROR_ECONNREFUSED, "SCE_NET_ERROR_ECONNREFUSED" },
    { SCE_NET_ERROR_ELOOP, "SCE_NET_ERROR_ELOOP" },
    { SCE_NET_ERROR_ENAMETOOLONG, "SCE_NET_ERROR_ENAMETOOLONG" },
    { SCE_NET_ERROR_EHOSTDOWN, "SCE_NET_ERROR_EHOSTDOWN" },
    { SCE_NET_ERROR_EHOSTUNREACH, "SCE_NET_ERROR_EHOSTUNREACH" },
    { SCE_NET_ERROR_ENOTEMPTY, "SCE_NET_ERROR_ENOTEMPTY" },
    { SCE_NET_ERROR_EPROCLIM, "SCE_NET_ERROR_EPROCLIM" },
    { SCE_NET_ERROR_EUSERS, "SCE_NET_ERROR_EUSERS" },
    { SCE_NET_ERROR_EDQUOT, "SCE_NET_ERROR_EDQUOT" },
    { SCE_NET_ERROR_ESTALE, "SCE_NET_ERROR_ESTALE" },
    { SCE_NET_ERROR_EREMOTE, "SCE_NET_ERROR_EREMOTE" },
    { SCE_NET_ERROR_EBADRPC, "SCE_NET_ERROR_EBADRPC" },
    { SCE_NET_ERROR_ERPCMISMATCH, "SCE_NET_ERROR_ERPCMISMATCH" },
    { SCE_NET_ERROR_EPROGUNAVAIL, "SCE_NET_ERROR_EPROGUNAVAIL" },
    { SCE_NET_ERROR_EPROGMISMATCH, "SCE_NET_ERROR_EPROGMISMATCH" },
    { SCE_NET_ERROR_EPROCUNAVAIL, "SCE_NET_ERROR_EPROCUNAVAIL" },
    { SCE_NET_ERROR_ENOLCK, "SCE_NET_ERROR_ENOLCK" },
    { SCE_NET_ERROR_ENOSYS, "SCE_NET_ERROR_ENOSYS" },
    { SCE_NET_ERROR_EFTYPE, "SCE_NET_ERROR_EFTYPE" },
    { SCE_NET_ERROR_EAUTH, "SCE_NET_ERROR_EAUTH" },
    { SCE_NET_ERROR_ENEEDAUTH, "SCE_NET_ERROR_ENEEDAUTH" },
    { SCE_NET_ERROR_EIDRM, "SCE_NET_ERROR_EIDRM" },
    { SCE_NET_ERROR_ENOMS, "SCE_NET_ERROR_ENOMS" },
    { SCE_NET_ERROR_EOVERFLOW, "SCE_NET_ERROR_EOVERFLOW" },
    { SCE_NET_ERROR_EILSEQ, "SCE_NET_ERROR_EILSEQ" },
    { SCE_NET_ERROR_ENOTSUP, "SCE_NET_ERROR_ENOTSUP" },
    { SCE_NET_ERROR_ECANCELED, "SCE_NET_ERROR_ECANCELED" },
    { SCE_NET_ERROR_EBADMSG, "SCE_NET_ERROR_EBADMSG" },
    { SCE_NET_ERROR_ENODATA, "SCE_NET_ERROR_ENODATA" },
    { SCE_NET_ERROR_ENOSR, "SCE_NET_ERROR_ENOSR" },
    { SCE_NET_ERROR_ENOSTR, "SCE_NET_ERROR_ENOSTR" },
    { SCE_NET_ERROR_ETIME, "SCE_NET_ERROR_ETIME" },
    { SCE_NET_ERROR_EADHOC, "SCE_NET_ERROR_EADHOC" },
    { SCE_NET_ERROR_EDISABLEDIF, "SCE_NET_ERROR_EDISABLEDIF" },
    { SCE_NET_ERROR_ERESUME, "SCE_NET_ERROR_ERESUME" },
    { SCE_NET_ERROR_ENOTINIT, "SCE_NET_ERROR_ENOTINIT" },
    { SCE_NET_ERROR_ENOLIBMEM, "SCE_NET_ERROR_ENOLIBMEM" },
    { SCE_NET_ERROR_ERESERVED202, "SCE_NET_ERROR_ERESERVED202" },
    { SCE_NET_ERROR_ECALLBACK, "SCE_NET_ERROR_ECALLBACK" },
    { SCE_NET_ERROR_EINTERNAL, "SCE_NET_ERROR_EINTERNAL" },
    { SCE_NET_ERROR_ERETURN, "SCE_NET_ERROR_ERETURN" }
};

static const char *lookup_sce_errno_name(int sce_errno)
{
    for (int i = 0; i < sizeof(g_sce_errno_table)/sizeof(SceErrnoTable); ++i)
    {
        if (g_sce_errno_table[i].code == sce_errno)
        {
            return g_sce_errno_table[i].name;
        }
    }

    return "unrecognised sce errno";
}

int __vita_sce_errno_to_errno(int sce_errno)
{
    sceClibPrintf("got errno: %s\n", lookup_sce_errno_name(sce_errno));

    switch (sce_errno)
    {
    case SCE_NET_ERROR_EPERM:
        return EPERM;
    case SCE_NET_ERROR_ENOENT:
        return ENOENT;
    case SCE_NET_ERROR_ESRCH:
        return ESRCH;
    case SCE_NET_ERROR_EINTR:
        return EINTR;
    case SCE_NET_ERROR_EIO:
        return EIO;
    case SCE_NET_ERROR_ENXIO:
        return ENXIO;
    case SCE_NET_ERROR_E2BIG:
        return E2BIG;
    case SCE_NET_ERROR_ENOEXEC:
        return ENOEXEC;
    case SCE_NET_ERROR_EBADF:
        return EBADF;
    case SCE_NET_ERROR_ECHILD:
        return ECHILD;
    case SCE_NET_ERROR_EDEADLK:
        return EDEADLK;
    case SCE_NET_ERROR_ENOMEM:
        return ENOMEM;
    case SCE_NET_ERROR_EACCES:
        return EACCES;
    case SCE_NET_ERROR_EFAULT:
        return EFAULT;
    case SCE_NET_ERROR_ENOTBLK:
        return ENOTBLK;
    case SCE_NET_ERROR_EBUSY:
        return EBUSY;
    case SCE_NET_ERROR_EEXIST:
        return EEXIST;
    case SCE_NET_ERROR_EXDEV:
        return EXDEV;
    case SCE_NET_ERROR_ENODEV:
        return ENODEV;
    case SCE_NET_ERROR_ENOTDIR:
        return ENOTDIR;
    case SCE_NET_ERROR_EISDIR:
        return EISDIR;
    case SCE_NET_ERROR_EINVAL:
        return EINVAL;
    case SCE_NET_ERROR_ENFILE:
        return ENFILE;
    case SCE_NET_ERROR_EMFILE:
        return EMFILE;
    case SCE_NET_ERROR_ENOTTY:
        return ENOTTY;
    case SCE_NET_ERROR_ETXTBSY:
        return ETXTBSY;
    case SCE_NET_ERROR_EFBIG:
        return EFBIG;
    case SCE_NET_ERROR_ENOSPC:
        return ENOSPC;
    case SCE_NET_ERROR_ESPIPE:
        return ESPIPE;
    case SCE_NET_ERROR_EROFS:
        return EROFS;
    case SCE_NET_ERROR_EMLINK:
        return EMLINK;
    case SCE_NET_ERROR_EPIPE:
        return EPIPE;
    case SCE_NET_ERROR_EDOM:
        return EDOM;
    case SCE_NET_ERROR_ERANGE:
        return ERANGE;
    case SCE_NET_ERROR_EAGAIN:
        return EAGAIN;
    // these values are the same as EAGAIN
//    case SCE_NET_ERROR_EWOULDBLOCK:
//        return EWOULDBLOCK;
    case SCE_NET_ERROR_EINPROGRESS:
        return EINPROGRESS;
    case SCE_NET_ERROR_EALREADY:
        return EALREADY;
    case SCE_NET_ERROR_ENOTSOCK:
        return ENOTSOCK;
    case SCE_NET_ERROR_EDESTADDRREQ:
        return EDESTADDRREQ;
    case SCE_NET_ERROR_EMSGSIZE:
        return EMSGSIZE;
    case SCE_NET_ERROR_EPROTOTYPE:
        return EPROTOTYPE;
    case SCE_NET_ERROR_ENOPROTOOPT:
        return ENOPROTOOPT;
    case SCE_NET_ERROR_EPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    case SCE_NET_ERROR_ESOCKTNOSUPPORT:
        return ESOCKTNOSUPPORT;
    case SCE_NET_ERROR_EOPNOTSUPP:
        return EOPNOTSUPP;
    case SCE_NET_ERROR_EPFNOSUPPORT:
        return EPFNOSUPPORT;
    case SCE_NET_ERROR_EAFNOSUPPORT:
        return EAFNOSUPPORT;
    case SCE_NET_ERROR_EADDRINUSE:
        return EADDRINUSE;
    case SCE_NET_ERROR_EADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case SCE_NET_ERROR_ENETDOWN:
        return ENETDOWN;
    case SCE_NET_ERROR_ENETUNREACH:
        return ENETUNREACH;
    case SCE_NET_ERROR_ENETRESET:
        return ENETRESET;
    case SCE_NET_ERROR_ECONNABORTED:
        return ECONNABORTED;
    case SCE_NET_ERROR_ECONNRESET:
        return ECONNRESET;
    case SCE_NET_ERROR_ENOBUFS:
        return ENOBUFS;
    case SCE_NET_ERROR_EISCONN:
        return EISCONN;
    case SCE_NET_ERROR_ENOTCONN:
        return ENOTCONN;
    case SCE_NET_ERROR_ESHUTDOWN:
        return ESHUTDOWN;
    case SCE_NET_ERROR_ETOOMANYREFS:
        return ETOOMANYREFS;
    case SCE_NET_ERROR_ETIMEDOUT:
        return ETIMEDOUT;
    case SCE_NET_ERROR_ECONNREFUSED:
        return ECONNREFUSED;
    case SCE_NET_ERROR_ELOOP:
        return ELOOP;
    case SCE_NET_ERROR_ENAMETOOLONG:
        return ENAMETOOLONG;
    case SCE_NET_ERROR_EHOSTDOWN:
        return EHOSTDOWN;
    case SCE_NET_ERROR_EHOSTUNREACH:
        return EHOSTUNREACH;
    case SCE_NET_ERROR_ENOTEMPTY:
        return ENOTEMPTY;
    case SCE_NET_ERROR_EUSERS:
        return EUSERS;
    case SCE_NET_ERROR_EDQUOT:
        return EDQUOT;
    case SCE_NET_ERROR_ESTALE:
        return ESTALE;
    case SCE_NET_ERROR_EREMOTE:
        return EREMOTE;
    case SCE_NET_ERROR_ENOLCK:
        return ENOLCK;
    case SCE_NET_ERROR_ENOSYS:
        return ENOSYS;
    case SCE_NET_ERROR_EIDRM:
        return EIDRM;
    case SCE_NET_ERROR_EOVERFLOW:
        return EOVERFLOW;
    case SCE_NET_ERROR_EILSEQ:
        return EILSEQ;
    case SCE_NET_ERROR_ENOTSUP:
        return ENOTSUP;
    case SCE_NET_ERROR_ECANCELED:
        return ECANCELED;
    case SCE_NET_ERROR_EBADMSG:
        return EBADMSG;
    case SCE_NET_ERROR_ENODATA:
        return ENODATA;
    case SCE_NET_ERROR_ENOSR:
        return ENOSR;
    case SCE_NET_ERROR_ENOSTR:
        return ENOSTR;
    case SCE_NET_ERROR_ETIME:
        return ETIME;
    default:
        sceClibPrintf("could not convert errno 0x%X (%s)\n", sce_errno, lookup_sce_errno_name(sce_errno));
        return EINVAL;
    }
}
