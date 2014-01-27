/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

static inline void copy_sa(struct sockaddr *sa, struct ASocket_Address *aa)
{
    int minlen = (sizeof(*sa) < aa->asa_Length) ? sizeof(*sa) : aa->asa_Length;
    if (aa->asa_Address)
        CopyMem(aa->asa_Address, sa, minlen);
}

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH2(LONG, ASocketSetA,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 11, ASocket)

/*  FUNCTION
 
        Sets parameters for the socket.

        The following tags are valid for modification.

        Some options may be 'sticky' for some protocols.
        (Ie AS_TAG_SOCKET_ENDPOINT can't be modified for
         AF_INET/SOCK_STREAM/IPPROTO_TCP)

        AS_TAG_SOCKET_ADDRESS       struct ASocket_Address *
            Address to bind the socket to.

        AS_TAG_SOCKET_ENDPOINT      struct ASocket_Address *
            Address to connect the socket to.

        AS_TAG_SOCKET_LISTEN        BOOL
            Set the socket to listen for an inbound connections.

        AS_TAG_LISTEN_BACKLOG       ULONG
            Backlog for sockets that need to listen.

        AS_TAG_NOTIFY_MSGPORT       struct MsgPort *
            MsgPort that will receive struct ASocket_Notify messages.

        AS_TAG_NOTIFY_FD_MASK       ULONG
            Mask of notify events - see FD_* in <sys/socket.h>

        AS_TAG_NOTIFY_NAME          APTR
            Value to put in the asn_Message.mn_Node.ln_Name field of
            the ASocket_Notify messages sent to the AS_TAG_NOTIFY_MSGPORT.

            Use this to supply per-socket private data to your
            notification mechanism.

        Tags specific to AF_LINK/SOCK_RAW/0 sockets:

        AS_TAG_IFACE_IFF_MASK       ULONG
            Mask of interface flags - see IFF_* in <net/if.h>

        AS_TAG_IFACE_METRIC         ULONG
            Metric of the interface

    INPUTS

        as      - ASocket handle
        tags    - struct TagItem * array

    RESULT

        0 on success, 'errno' on failure.

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

    struct bsd *bsd = ASocketBase->ab_bsd;
    struct TagItem *tag, *tptr = tags;
    struct ASocket_Address *addr;
    ULONG tmp;
    struct ASocket *as = s;

    D(bug("%s: as=%p, tags=%p\n", __func__, as, tags));

    if (as->as_Node.ln_Type != NT_AS_SOCKET)
        return EINVAL;
    
    while ((tag = LibNextTagItem(&tptr))) {
        int err = 0;

        switch (tag->ti_Tag) {
        case AS_TAG_SOCKET_ADDRESS:
            addr = (struct ASocket_Address *)tag->ti_Data;
            if (addr) {
                err = bsd_bind(bsd, as->as_bsd, addr->asa_Address, addr->asa_Length);
            } else {
                err = EFAULT;
            }
            break;
        case AS_TAG_SOCKET_ENDPOINT:
            addr = (struct ASocket_Address *)tag->ti_Data;
            if (addr) {
                err = bsd_connect(bsd, as->as_bsd, addr->asa_Address, addr->asa_Length);
            } else {
                err = EFAULT;
            }
            break;
        case AS_TAG_LISTEN_BACKLOG:
            tmp = (ULONG)tag->ti_Data;
            err = bsd_listen(bsd, as->as_bsd, tmp);
            break;
        case AS_TAG_NOTIFY_MSGPORT:
            as->as_Notify.asn_Message.mn_ReplyPort = (struct MsgPort *)tag->ti_Data;
            break;
        case AS_TAG_NOTIFY_FD_MASK:
            as->as_Notify.asn_NotifyEvents = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_NOTIFY_NAME:
            as->as_Notify.asn_Message.mn_Node.ln_Name = (APTR)tag->ti_Data;
            break;
        case AS_TAG_IFACE_ADDRESS:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                copy_sa(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCSIFADDR, &ifr);
            } else {
                D(bug("%s: AS_TAG_IFACE_ADDRESS not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_NETMASK:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                copy_sa(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCSIFNETMASK, &ifr);
            } else {
                D(bug("%s: AS_TAG_IFACE_NETMASK not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        case AS_TAG_IFACE_BROADCAST:
            if (as->as_Socket.type == SOCK_RAW) {
                struct ifreq ifr;
                CopyMem(as->as_IFace.name, ifr.ifr_name, IFNAMSIZ);
                copy_sa(&ifr.ifr_addr, (struct ASocket_Address *)tag->ti_Data);
                err = bsd_ioctl(bsd, as->as_bsd, SIOCSIFBRDADDR, &ifr);
            } else {
                D(bug("%s: AS_TAG_IFACE_BROADCAST not valid for %d type sockets\n", __func__, as->as_Socket.type));
                err = EINVAL;
            }
            break;
        default:
            return EINVAL;
        }

        if (err)
            return err;

        tag->ti_Tag |= AS_TAGF_COMPLETE;
    }

    return 0;

    AROS_LIBFUNC_EXIT
}

