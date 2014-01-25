/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

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

    while ((tag = LibNextTagItem(&tptr))) {
        int err = 0;
        struct ASocket_Address *addr;

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
            err = bsd_getsockname(bsd, s, addr->asa_Address, &addr->asa_Length);
            break;
        case AS_TAG_SOCKET_ENDPOINT:
            addr = (struct ASocket_Address *)tag->ti_Data;
            err = bsd_getpeername(bsd, s, addr->asa_Address, &addr->asa_Length);
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
#if 0
        case AS_TAG_IFACE_INDEX:
            *(ULONG *)tag->ti_Data = iface_index;
            break;
        case AS_TAG_IFACE_NAME:
            *(CONST_STRPTR *)tag->ti_Data = iface_name;
            break;
#endif
        default:
            return EINVAL;
        }

        if (err)
            return err;

        tag->ti_Tag |= AS_TAGF_COMPLETE;
    }
    return EINVAL;

    AROS_LIBFUNC_EXIT
}

