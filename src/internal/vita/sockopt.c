#include "sockopt.h"
#include "fd.h"
#include "sceerrno.h"

#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#include <psp2/net/net.h>
#include <psp2/kernel/clib.h>

static int translate_ip_opt(int musl_opt)
{
    switch (musl_opt)
    {
    case IP_HDRINCL:
        return SCE_NET_IP_HDRINCL;
    case IP_TOS:
        return SCE_NET_IP_TOS;
    case IP_TTL:
        return SCE_NET_IP_TTL;
    case IP_MULTICAST_IF:
        return SCE_NET_IP_MULTICAST_IF;
    case IP_MULTICAST_TTL:
        return SCE_NET_IP_MULTICAST_TTL;
    case IP_MULTICAST_LOOP:
        return SCE_NET_IP_MULTICAST_LOOP;
    case IP_ADD_MEMBERSHIP:
        return SCE_NET_IP_ADD_MEMBERSHIP;
    case IP_DROP_MEMBERSHIP:
        return SCE_NET_IP_DROP_MEMBERSHIP;
    default:
        sceClibPrintf("musl: could not translate IP opt %i\n", musl_opt);
        return musl_opt;
    }
}

static int translate_tcp_opt(int musl_opt)
{
    switch (musl_opt)
    {
    case TCP_NODELAY:
        return SCE_NET_TCP_NODELAY;
    case TCP_MAXSEG:
        return SCE_NET_TCP_MAXSEG;
    default:
        sceClibPrintf("musl: could not translate TCP opt %i\n", musl_opt);
        return musl_opt;
    }
}

static int translate_socket_opt(int musl_opt)
{
    switch (musl_opt)
    {
    case SO_REUSEADDR:
        return SCE_NET_SO_REUSEADDR;
    case SO_KEEPALIVE:
        return SCE_NET_SO_KEEPALIVE;
    case SO_BROADCAST:
        return SCE_NET_SO_BROADCAST;
    case SO_LINGER:
        return SCE_NET_SO_LINGER;
    case SO_OOBINLINE:
        return SCE_NET_SO_OOBINLINE;
    case SO_REUSEPORT:
        return SCE_NET_SO_REUSEPORT;
    case SO_SNDBUF:
        return SCE_NET_SO_SNDBUF;
    case SO_RCVBUF:
        return SCE_NET_SO_RCVBUF;
    case SO_SNDLOWAT:
        return SCE_NET_SO_SNDLOWAT;
    case SO_RCVLOWAT:
        return SCE_NET_SO_RCVLOWAT;
    case SO_SNDTIMEO:
        return SCE_NET_SO_SNDTIMEO;
    case SO_RCVTIMEO:
        return SCE_NET_SO_RCVTIMEO;
    case SO_ERROR:
        return SCE_NET_SO_ERROR;
    case SO_TYPE:
        return SCE_NET_SO_TYPE;
    default:
        sceClibPrintf("musl: could not translate SOCKET opt %i\n", musl_opt);
        return musl_opt;
    }
}

int __vita_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    switch (level)
    {
    case SOL_SOCKET:
        level = SCE_NET_SOL_SOCKET;
        optname = translate_socket_opt(optname);
        break;
    case IPPROTO_IP:
        level = SCE_NET_IPPROTO_IP;
        optname = translate_ip_opt(optname);
        break;
    case IPPROTO_TCP:
        level = SCE_NET_IPPROTO_TCP;
        optname = translate_tcp_opt(optname);
        break;
    default:
        sceClibPrintf("musl: could not translate level %i, optname %i\n", level, optname);
        break;
    }

    int res = sceNetGetsockopt(f->sce_uid, level, optname, optval, optlen);

    if (res < 0)
        res = -(__vita_sce_errno_to_errno(res));

    __vita_fd_drop(f);
    return res;
}

int __vita_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    DescriptorTranslation *f = __vita_fd_grab(sockfd);

    if (!f)
        return -EBADF;

    switch (level)
    {
    case SOL_SOCKET:
        level = SCE_NET_SOL_SOCKET;
        optname = translate_socket_opt(optname);
        break;
    case IPPROTO_IP:
        level = SCE_NET_IPPROTO_IP;
        optname = translate_ip_opt(optname);
        break;
    case IPPROTO_TCP:
        level = SCE_NET_IPPROTO_TCP;
        optname = translate_tcp_opt(optname);
        break;
    default:
        sceClibPrintf("musl: could not translate level %i, optname %i\n", level, optname);
        break;
    }

    int res = sceNetSetsockopt(f->sce_uid, level, optname, optval, optlen);

    if (res < 0)
        res = -(__vita_sce_errno_to_errno(res));

    __vita_fd_drop(f);
    return res;
}
