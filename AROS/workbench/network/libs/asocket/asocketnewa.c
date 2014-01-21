/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
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

    AROS_FUNCTION_NOT_IMPLEMENTED(asocket);

    return EINVAL;

    AROS_LIBFUNC_EXIT
}

