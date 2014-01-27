/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

static inline void sa_copy(struct sockaddr *sa, struct ASocket_Address *aa)
{
    int minlen = (sizeof(*sa) < aa->asa_Length) ? sizeof(*sa) : aa->asa_Length;
    if (aa->asa_Address)
        CopyMem(sa, aa->asa_Address, minlen);
}


/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH2(LONG, ASocketGetA,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 10, ASocket)

/*  FUNCTION

        Get the attributes of an ASocket handle.
        
        Valid tags for this function are:

        AS_TAG_SOCKET_DOMAIN        ULONG *
            Address domain - see the <sys/socket.h> AF_* constants.

        AS_TAG_SOCKET_TYPE          ULONG *
            Socket type - see the SOCK_* type constants in <sys/socket.h>
                        
        AS_TAG_SOCKET_PROTOCOL      ULONG *
            Socket protocol - see the PF_* constants in <sys/socket.h>

        AS_TAG_SOCKET_ADDRESS       struct ASocket_Address *
            Address the socket is bound to. The asa_Length is
            modified to the actual address length if a truncation
            occurred.

        AS_TAG_SOCKET_ENDPOINT      struct ASocket_Address *
            Endpoint the socket is connected to. The asa_Length is
            modified to the actual address length if a truncation
            occurred.

        AS_TAG_LISTEN_BACKLOG       ULONG *
            Backlog for sockets that need to listen.

        AS_TAG_NOTIFY_MSGPORT       struct MsgPort **
            MsgPort that will receive struct ASocket_Notify messages.

        AS_TAG_NOTIFY_FD_MASK       ULONG *
            Mask of notify events - see FD_* in <sys/socket.h>

        AS_TAG_NOTIFY_NAME          APTR *
            Value to put in the asn_Message.mn_Node.ln_Name field of
            the ASocket_Notify messages sent to the AS_TAG_NOTIFY_MSGPORT.

            Use this to supply per-socket private data to your
            notification mechanism.

        Tags specific to AF_LINK/SOCK_RAW/0 sockets:

        AS_TAG_IFACE_INDEX          ULONG *
            Interface id (1...N)

        AS_TAG_IFACE_NAME           CONST_STRPTR *
            Interface name (ie "lo", "eth0", etc)

        AS_TAG_IFACE_IFF_MASK       ULONG *
            Mask of interface flags - see IFF_* in <net/if.h>

        AS_TAG_IFACE_METRIC         ULONG *
            Metric of the interface

    INPUTS

        as - ASocket handle to inspect
        tags - List of tagitems

    RESULT

        0 on success, or an 'errno' value on failue.

        Valid tags will be modified to ti_Tag |= AS_TAGF_COMPLETE

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct ASocket *as = s;
    struct bsd *bsd = ASocketBase->ab_bsd;
    struct TagItem *tag, *tptr = tags;

    D(bug("%s: as=%p, tags=%p\n", __func__, as, tags));

    if (as->as_Node.ln_Type != NT_AS_SOCKET &&
        as->as_Node.ln_Type != NT_AS_OBTAINED)
        return EINVAL;
    
    while ((tag = LibNextTagItem(&tptr))) {
        int err = 0;
        struct ASocket_Address *addr;

        D(bug("%s: Tag 0x%08lx, 0x%p\n", __func__, (unsigned long)tag->ti_Tag, (APTR)tag->ti_Data));

        switch (tag->ti_Tag) {
        case AS_TAG_SOCKET_DOMAIN:
            *(ULONG *)tag->ti_Data = as->as_Socket.domain;
            break;
        case AS_TAG_SOCKET_TYPE:
            *(ULONG *)tag->ti_Data = as->as_Socket.type;
            break;
        case AS_TAG_SOCKET_PROTOCOL:
            *(ULONG *)tag->ti_Data = as->as_Socket.protocol;
            break;
        case AS_TAG_SOCKET_ADDRESS:
            addr = (struct ASocket_Address *)tag->ti_Data;
            err = bsd_getsockname(bsd, as->as_bsd, addr->asa_Address, &addr->asa_Length);
            break;
        case AS_TAG_SOCKET_ENDPOINT:
            addr = (struct ASocket_Address *)tag->ti_Data;
            err = bsd_getpeername(bsd, as->as_bsd, addr->asa_Address, &addr->asa_Length);
            break;
        case AS_TAG_SOCKET_LISTEN:
            *(BOOL *)tag->ti_Data = (as->as_Listen.enabled ? TRUE : FALSE);
            break;
        case AS_TAG_LISTEN_BACKLOG:
            *(ULONG *)tag->ti_Data = as->as_Listen.backlog;
            break;
        case AS_TAG_NOTIFY_MSGPORT:
            *(struct MsgPort **)tag->ti_Data = as->as_Notify.asn_Message.mn_ReplyPort;
            break;
        case AS_TAG_NOTIFY_FD_MASK:
            *(ULONG *)tag->ti_Data = as->as_Notify.asn_NotifyEvents;
            break;
        case AS_TAG_NOTIFY_NAME:
            *(APTR *)tag->ti_Data = as->as_Notify.asn_Message.mn_Node.ln_Name;
            break;
        case AS_TAG_IFACE_INDEX:
            if (as->as_Socket.type == SOCK_RAW) {
                *(ULONG *)tag->ti_Data = as->as_IFace.index;
            } else {
                D(bug("%s: AS_TAG_IFACE_INDEX not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_NAME:
            if (as->as_Socket.type == SOCK_RAW) {
                *(CONST_STRPTR *)tag->ti_Data = as->as_IFace.name;
            } else {
                D(bug("%s: AS_TAG_IFACE_NAME not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_IFF_MASK:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCGIFFLAGS, &ifr);
                if (err == 0)
                    *(ULONG *)tag->ti_Data = ifr.ifr_flags;
            } else {
                D(bug("%s: AS_TAG_IFACE_IFF_MASK not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_METRIC:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCGIFMETRIC, &ifr);
                if (err == 0)
                    *(ULONG *)tag->ti_Data = ifr.ifr_metric;
            } else {
                D(bug("%s: AS_TAG_IFACE_METRIC not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_ADDRESS:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCGIFADDR, &ifr);
                if (err == 0)
                    sa_copy(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
            } else {
                D(bug("%s: AS_TAG_IFACE_ADDRESS not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_NETMASK:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCGIFNETMASK, &ifr);
                if (err == 0)
                    sa_copy(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
            } else {
                D(bug("%s: AS_TAG_IFACE_NETMASK not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_BROADCAST:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCGIFBRDADDR, &ifr);
                if (err == 0)
                    sa_copy(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
            } else {
                D(bug("%s: AS_TAG_IFACE_BROADCAST not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        default:
            err = EINVAL;
        }

        if (err)
            return err;

        tag->ti_Tag |= AS_TAGF_COMPLETE;
    }

    return 0;

    AROS_LIBFUNC_EXIT
}

