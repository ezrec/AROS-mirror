/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH2(LONG, ASocketNewA,

/*  SYNOPSIS */
        AROS_LHA(APTR *, new_as, A0),
        AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 5, ASocket)

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

    struct ASocket *as;
    struct TagItem *tag, *tptr = tags;
    const struct ASocket_Address *addr = NULL, *endp = NULL;
    BOOL listen = FALSE;
    ULONG listen_backlog = 0
    ULONG domain = AF_UNSPEC, type = 0, protocol = PF_UNSPEC;
    struct MsgPort *notify_msgport = NULL;
    ULONG notify_fd_mask = 0;
    APTR notify_name = NULL;
    ULONG iface_index = 0;

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

    /* This 'loop' stack only supports AF_LINK and AF_INET protocols
     */
    if (domain == AF_LINK && type == SOCK_RAW && protocol == 0) {
        if (iface_index != 0 && iface_name != NULL)
            return EINVAL;

        if (iface_name) {
            /* Search for ASocket interface by name */
            struct ASocket *tmp;
            err = EINVAL;
            ObtainSemaphore(&ASocketBase->ab_Lock);
            ForeachNode(&ASocketBase->ab_InterfaceList, tmp) {
                if (strcmp(tmp->as_Node.ln_Name, iface_name) == 0) {
                    LONG err;
                    err = ASocketDuplicate(tmp, new_as);
                    break;
                }
            }
            ReleaseSemaphore(&ASocketBase->ab_Lock);
            return err;
        } else if (iface_index < 128) {
            /* We abuse the as_Node.ln_Pri field for interface id */
            ObtainSemaphore(&ASocketBase->ab_Lock);
            ForeachNode(&ASocketBase->ab_InterfaceList, tmp) {
                if (tmp->as_Node.ln_Type == (BYTE)iface_index) {
                    LONG err;
                    err = ASocketDuplicate(tmp, new_as);
                    break;
                }
            }
            ReleaseSemaphore(&ASocketBase->ab_Lock);
            return err;
        }

        return EINVAL;
    } else if (domain == AF_INET) {
        /* Only SOCK_STREAM and SOCK_DRAM supported in this emulation */
        if (type != SOCK_STREAM && type != SOCK_DGRAM)
            return EINVAL;

        if (protocol == IPPROTO_IP)
            protocol = (type == SOCK_STREAM) ? IPPROTO_TCP : IPPROTO_UDP;

        if (addr && !addrcheck(addr))
            return EINVAL;

        if (endp && !addrcheck(endp))
            return EINVAL;
    } else {
        return EINVAL;
    }

    if ((as = AllocVec(sizeof(*as), MEMF_ANY | MEMF_CLEAR))) {
        as->as_Node.ln_Type = NT_UNKNOWN;
        as->as_Node.ln_Name = "ASocket (loop)";

        as->as_Usage = 1;

        as->as_Socket.domain = domain;
        as->as_Socket.type   = type;
        as->as_Socket.protocol = protocol;

        as->as_Listen.enabled = listen;
        as->as_Listen.backlog = listen_backlog;

        if (addr)
            CopyMem(addr->asa_Address, as->as_Address, 8);

        if (endp)
            CopyMem(endp->asa_Address, as->as_Endpoint, 8);

        as->as_Notify.asn_Message.mn_Node.ln_Type = NT_UNKNOWN;
        as->as_Notify.asn_Message.mn_Node.ln_Name = notify_name;
        as->as_Notify.asn_ASocket = as;
        as->as_Notify.asn_NotifyEvents = notify_mask;
        as->as_Notify.asn_Events = 0;

        ObtainSemaphore(&ASocketBase->ab_Lock);
        ADDTAIL(&ASocketBase->ab_SocketList, as);
        ReleaseSeamphore(&ASocketBase->ab_Lock);

        *new_as = as;

        return 0;
    }

    return ENOMEM;

    AROS_LIBFUNC_EXIT
}

