/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/ioctl.h>

#include "bsdsocket_intern.h"

static int do_ifconf(struct bsdsocketBase *SocketBase, struct ifconf *ifc);

/*****************************************************************************

    NAME */

        AROS_LH3(int, IoctlSocket,

/*  SYNOPSIS */
        AROS_LHA(int,           s,       D0),
        AROS_LHA(unsigned long, request, D1),
        AROS_LHA(char *,        argp,    A0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 19, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct bsd_fd *fd;
    LONG err = 0;
    ULONG tmp;
    struct ifreq *ifr;
    struct ifaliasreq *ifra;
    ULONG domain = AF_LINK;
    D(CONST_STRPTR reqname = "unknown");
    APTR as;

    D(bug("%s: s=%d, request=%d, argp=%p\n", __func__, s, request, argp));

    fd = BSD_GET_FD(SocketBase, s);

    ASocketGet(fd->asocket, AS_TAG_SOCKET_DOMAIN, &domain, TAG_END);

    switch (request) {
    case FIOASYNC:
        D(reqname = "FIOASYNC");
        tmp = fd->flags;
        switch (*(long *)argp) {
        case 0: fd->flags &= ~O_ASYNC; break;
        case 1: fd->flags |=  O_ASYNC; break;
        default: err = EINVAL; break;
        }
        if (tmp != fd->flags) {
            if (fd->flags & O_ASYNC) {
                /* Fail if no owner for sigio */
                if (fd->sigioTask == NULL) {
                    fd->flags = tmp;
                    err = EINVAL;
                    break;
                }
                /* Put in async mode - use the global async monitor task */
                err = ASocketSet(as,
                                 AS_TAG_NOTIFY_MSGPORT, SocketBase->lib_BSDSocketBase->bs_MsgPort,
                                 AS_TAG_NOTIFY_FD_MASK, fd->fd_mask,
                                 AS_TAG_NOTIFY_NAME, fd,
                                 TAG_END);
            } else {
                /* Stop async mode */
                err = ASocketSet(as,
                                 AS_TAG_NOTIFY_MSGPORT, NULL,
                                 AS_TAG_NOTIFY_FD_MASK, 0,
                                 AS_TAG_NOTIFY_NAME, NULL,
                                 TAG_END);
            }
            if (err != 0)
                fd->flags = tmp;
        }
        break;
    case FIOCLEX:
    case FIONCLEX:
        /* Valid, but ignored */
        break;
    case FIOGETOWN:
        D(reqname = "FIOGETOWN");
        *(struct Task **)argp = fd->sigioTask;
        break;
    case SIOCGPGRP:
        D(reqname = "SIOCGPGRP");
        *(struct Task **)argp = fd->sigurgTask;
        break;
    case FIONBIO:
        D(reqname = "FIONBIO");
        switch (*(long *)argp) {
        case 0: fd->flags &= ~O_NONBLOCK; break;
        case 1: fd->flags |=  O_NONBLOCK; break;
        default: err = EINVAL; break;
        }
        break;
    case FIONREAD:
        D(reqname = "FIONREAD");
        err = ASocketGet(as, AS_TAG_STATUS_READABLE, &tmp, TAG_END);
        if (err == 0) {
            *(long *)argp = (long)tmp;
        }
        break;
    case FIOSETOWN:
        D(reqname = "FIOSETOWN");
        fd->sigioTask = (struct Task *)argp;
        break;
    case SIOCGIFFLAGS:
        D(reqname = "SIOCGIFFLAGS");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            ULONG utmp;
            err = ASocketGet(as, AS_TAG_IFACE_IFF_MASK, &utmp, TAG_END);
            if (err == 0)
                ifr->ifr_flags = (short)utmp;
            ASocketDispose(as);
        }
        break;
    case SIOCGIFMETRIC:
        D(reqname = "SIOCGIFMETRIC");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            ULONG utmp;
            err = ASocketGet(as, AS_TAG_IFACE_METRIC, &utmp, TAG_END);
            if (err == 0)
                ifr->ifr_metric = (int)utmp;
            ASocketDispose(as);
        }
        break;
    case SIOCGIFMTU:
        D(reqname = "SIOCGIFMTU");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            ULONG utmp;
            err = ASocketGet(as, AS_TAG_IFACE_MTU, &utmp, TAG_END);
            if (err == 0)
                ifr->ifr_mtu = (int)utmp;
            ASocketDispose(as);
        }
        break;
    case SIOCSPGRP:
        D(reqname = "SIOCSPGRP");
        fd->sigurgTask = (struct Task *)argp;
        break;
    case SIOCGIFCONF:
        D(reqname = "SIOCGIFCONF");
        err = do_ifconf(SocketBase, (struct ifconf *)argp);
        break;
    case SIOCGIFADDR:
        D(reqname = "SIOGAIFADDR");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            struct ASocket_Address addr;
            addr.asa_Length = sizeof(struct sockaddr);
            addr.asa_Address = &ifr->ifr_addr;
            err = ASocketGet(as, AS_TAG_IFACE_ADDRESS, &addr, TAG_END);
            ASocketDispose(as);
        }
        break;
    case SIOCGIFNETMASK:
        D(reqname = "SIOCGIFNETMASK");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            struct ASocket_Address addr;
            addr.asa_Length = sizeof(struct sockaddr);
            addr.asa_Address = &ifr->ifr_addr;
            err = ASocketGet(as, AS_TAG_IFACE_NETMASK, &addr, TAG_END);
            ASocketDispose(as);
        }
        break;
    case SIOCGIFBRDADDR:
        D(reqname = "SIOCGIFBRDADDR");
        ifr = (struct ifreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifr->ifr_name,
                              TAG_END);
        if (err == 0) {
            struct ASocket_Address addr;
            addr.asa_Length = sizeof(struct sockaddr);
            addr.asa_Address = &ifr->ifr_addr;
            err = ASocketGet(as, AS_TAG_IFACE_BROADCAST, &addr, TAG_END);
            ASocketDispose(as);
        }
        break;
    case SIOCAIFADDR:
        D(reqname = "SIOCAIFADDR");
        ifra = (struct ifaliasreq *)argp;
        err = ASocketNew(&as, AS_TAG_SOCKET_DOMAIN, domain,
                              AS_TAG_SOCKET_TYPE, SOCK_RAW,
                              AS_TAG_SOCKET_PROTOCOL, 0,
                              AS_TAG_IFACE_NAME, ifra->ifra_name,
                              TAG_END);
        if (err == 0) {
            struct ASocket_Address addr, mask, brdc;
            struct TagItem tags[4] = {
                { AS_TAG_IFACE_ADDRESS,   (IPTR)&addr },
                { AS_TAG_IFACE_NETMASK,   (IPTR)&mask },
                { AS_TAG_IFACE_BROADCAST, (IPTR)&brdc },
                { TAG_END }
            };

            if (ifra->ifra_addr.sa_len) {
                addr.asa_Address = &ifra->ifra_addr;
                addr.asa_Length = ifra->ifra_addr.sa_len;
            } else {
                tags[0].ti_Tag = TAG_IGNORE;
            }
            if (ifra->ifra_mask.sa_len) {
                mask.asa_Address = &ifra->ifra_mask;
                mask.asa_Length = ifra->ifra_mask.sa_len;
            } else {
                tags[1].ti_Tag = TAG_IGNORE;
            }
            if (ifra->ifra_broadaddr.sa_len) {
                brdc.asa_Address = &ifra->ifra_broadaddr;
                brdc.asa_Length = ifra->ifra_broadaddr.sa_len;
            } else {
                tags[2].ti_Tag = TAG_IGNORE;
            }

            err = ASocketGetA(as, &tags[0]);
            ASocketDispose(as);
        }
        break;
    default:
        err = EINVAL;
        break;
    }

    D(bug("%s: 0x%x(%s) %p => ERRNO %d\n", __func__, request, reqname, argp, err));
    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT

} /* IoctlSocket */

static int do_ifconf(struct bsdsocketBase *SocketBase, struct ifconf *ifc)
{
    struct List *iflist;
    struct Node *ifnode;
    struct ifreq *ifbuf = &ifc->ifc_req[0];
    int len = 0;

    iflist = ASocketListObtain(AS_TAG_SOCKET_DOMAIN, AF_LINK,
                               AS_TAG_SOCKET_TYPE, SOCK_RAW,
                               AS_TAG_SOCKET_PROTOCOL, 0,
                               TAG_END);
    if (iflist == NULL) {
        BSD_SET_ERRNO(SocketBase, ENOMEM);
        return -1;
    }

    ForeachNode(iflist, ifnode) {
        len += sizeof(struct ifreq);
        if (len <= ifc->ifc_len) {
            CONST_STRPTR name = "";
            struct sockaddr sa = { };
            struct ASocket_Address asa = { .asa_Address = &sa, .asa_Length = sizeof(sa) };
            ASocketGet(ifnode, AS_TAG_IFACE_NAME, &name,
                               AS_TAG_IFACE_ADDRESS, &asa,
                               TAG_END);
            strncpy(ifbuf->ifr_name, name, sizeof(ifbuf->ifr_name));
            CopyMem(&sa, &ifbuf->ifr_addr, sizeof(sa));
        }
        ifbuf++;
    }
    ifc->ifc_len = len;

    ASocketListRelease(iflist);

    return 0;
}
