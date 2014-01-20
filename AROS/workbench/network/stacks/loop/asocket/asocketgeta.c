/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
        AROS_LH2(LONG, ASocketGetA,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 10, ASocket)

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

    return EINVAL;

    AROS_LIBFUNC_EXIT
}

