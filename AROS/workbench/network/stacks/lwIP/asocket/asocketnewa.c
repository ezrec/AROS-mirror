/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

#include <fcntl.h>

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH2(LONG, ASocketNewA,

/*  SYNOPSIS */
        AROS_LHA(APTR *, new_as, A0),
        AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 5, ASocket)

/*  FUNCTION
 
        Create a new ASocket based on the descrition in 'tags'.

    INPUTS

        new_as - Pointer that will be filled in with the new
                 ASocket handle

        tags   - Tags list descripting the new socket.

        Mandatory tags:

        AS_TAG_SOCKET_DOMAIN        ULONG
            Address domain - see the <sys/socket.h> AF_* constants.

        AS_TAG_SOCKET_TYPE          ULONG
            Socket type - see the SOCK_* type constants in <sys/socket.h>
                        
        AS_TAG_SOCKET_PROTOCOL      ULONG
            Socket protocol - see the PF_* constants in <sys/socket.h>

        Optional tags - these can be set later via ASocketSet()

        AS_TAG_SOCKET_ADDRESS       struct ASocket_Address *
            Address to bind the socket to.

        AS_TAG_SOCKET_ENDPOINT      struct ASocket_Address *
            Address to connect the socket to.

        AS_TAG_SOCKET_LISTEN        BOOL
            The socket is listening for an inbound connection

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

        AS_TAG_IFACE_INDEX          ULONG
            Interface id to attach to (1...N)

        AS_TAG_IFACE_NAME           CONST_STRPTR
            Interface name to attach to (ie "lo", "eth0", etc)

        Note that AS_TAG_IFACE_INDEX and AS_TAG_IFACE_NAME are mutually
        exclusive - only one may be specified for finding a AF_LINK socket.

    RESULT

        On success, new_as will be filled in with the new ASocket handle.

        0 on success, or a 'errno' value on failue.

        Valid tags will be modified to ti_Tag |= AS_TAGF_COMPLETE

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketDispose(), ASocketDuplicate(), ASocketListObtain(),
        ASocketListRelease()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct bsd *bsd = ASocketBase->ab_bsd;
    struct bsd_sock *s;
    struct TagItem *tag, *tptr = tags;
    const struct ASocket_Address *addr = NULL, *endp = NULL;
    BOOL listen = FALSE;
    ULONG listen_backlog = 0;
    ULONG domain = AF_UNSPEC, type = 0, protocol = PF_UNSPEC;
    struct MsgPort *notify_msgport = NULL;
    ULONG notify_fd_mask = 0;
    APTR notify_name = NULL;
    ULONG iface_index = 0;
    CONST_STRPTR iface_name = NULL;
    int err;

    D(bug("%s: new_as=%p, tags=%p\n", __func__, new_as, tags));

    while ((tag = LibNextTagItem(&tptr))) {
        switch (tag->ti_Tag) {
        case AS_TAG_SOCKET_DOMAIN:
            domain = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_SOCKET_TYPE:
            type = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_SOCKET_PROTOCOL:
            protocol = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_SOCKET_ADDRESS:
            addr = (struct ASocket_Address *)tag->ti_Data;
            break;
        case AS_TAG_SOCKET_ENDPOINT:
            endp = (struct ASocket_Address *)tag->ti_Data;
            break;
        case AS_TAG_SOCKET_LISTEN:
            listen = (BOOL)tag->ti_Data;
            break;
        case AS_TAG_LISTEN_BACKLOG:
            listen_backlog = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_NOTIFY_MSGPORT:
            notify_msgport = (struct MsgPort *)tag->ti_Data;
            break;
        case AS_TAG_NOTIFY_FD_MASK:
            notify_fd_mask = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_NOTIFY_NAME:
            notify_name = (APTR)tag->ti_Data;
            break;
        case AS_TAG_IFACE_INDEX:
            iface_index = (ULONG)tag->ti_Data;
            break;
        case AS_TAG_IFACE_NAME:
            iface_name = (CONST_STRPTR)tag->ti_Data;
            break;
        default:
            return EINVAL;
        }

        tag->ti_Tag |= AS_TAGF_COMPLETE;
    }

    if (domain == AF_LINK && type == SOCK_RAW && protocol == 0) {
        if (iface_index != 0 && iface_name != NULL)
            return EINVAL;

        /* Unused for now */
        return EINVAL;
    }

    err = bsd_socket(bsd, &s, domain, type, protocol);
    if (err == 0) {
        struct ASocket *as = (struct ASocket *)s;
        /* Put in async mode */
        err = bsd_fcntl(bsd, s, F_SETFL, bsd_fcntl(bsd, s, F_GETFL, 0) | O_NONBLOCK);
        if (err < 0) {
            bsd_close(bsd, s);
            return err;
        }
        as->as_Node.ln_Type = NT_UNKNOWN;
        as->as_Node.ln_Name = "ASocket (loop)";

        as->as_Socket.domain = domain;
        as->as_Socket.type   = type;
        as->as_Socket.protocol = protocol;

        if (listen) {
            as->as_Listen.enabled = listen;
            as->as_Listen.backlog = listen_backlog;

            err = bsd_listen(bsd, s, listen_backlog);
            if (err != 0) {
                bsd_close(bsd, s);
                return err;
            }
        }

        if (addr) {
            err = bsd_bind(bsd, s, addr->asa_Address, addr->asa_Length);
            if (err != 0) {
                bsd_close(bsd, s);
                return err;
            }
        }

        if (endp) {
            err = bsd_connect(bsd, s, endp->asa_Address, endp->asa_Length);
            if (err != 0) {
                bsd_close(bsd, s);
                return err;
            }
        }

        as->as_Notify.asn_Message.mn_Node.ln_Type = NT_UNKNOWN;
        as->as_Notify.asn_Message.mn_Node.ln_Name = notify_name;
        as->as_Notify.asn_Message.mn_ReplyPort = notify_msgport;
        as->as_Notify.asn_ASocket = as;
        as->as_Notify.asn_NotifyEvents = notify_fd_mask;
        as->as_Notify.asn_Events = 0;

        ObtainSemaphore(&ASocketBase->ab_Lock);
        ADDTAIL(&ASocketBase->ab_SocketList, as);
        ReleaseSemaphore(&ASocketBase->ab_Lock);

        *new_as = as;

        return 0;
    }

    return ENOMEM;

    AROS_LIBFUNC_EXIT
}

