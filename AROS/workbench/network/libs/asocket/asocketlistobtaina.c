/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH1(struct List *, ASocketListObtain,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, pattern, A0),

/*  LOCATION */
        struct Library *, ASocketBase, 8, ASocket)

/*  FUNCTION
 
        Obtain a struct List * that refers to a list of struct Node *s
        that can be passed like ASocket handles to ASocketGet().

        The ASockets in this list cannot be used for sending or
        receiving data.

    INPUTS

        pattern - Tags that specify the sockets of interest.
                  A NULL pattern will return an empty list.

        Valid tags for the pattern include:

        AS_TAG_SOCKET_DOMAIN        ULONG
            Address domain - see the <sys/socket.h> AF_* constants.

        AS_TAG_SOCKET_TYPE          ULONG
            Socket type - see the SOCK_* type constants in <sys/socket.h>
                        
        AS_TAG_SOCKET_PROTOCOL      ULONG
            Socket protocol - see the PF_* constants in <sys/socket.h>

        AS_TAG_SOCKET_ADDRESS       struct ASocket_Address *
            Address the socket is bound to.

        AS_TAG_SOCKET_ENDPOINT      struct ASocket_Address *
            Address the socket is connected to.

        AS_TAG_SOCKET_LISTEN        BOOL
            Sockets that are listening for connections

        AS_TAG_LISTEN_BACKLOG       ULONG
            A specific size of a listening socket's backlog
            Only valid if AS_TAG_SOCKET_LISTEN = TRUE in the pattern.

        AS_TAG_NOTIFY_MSGPORT       struct MsgPort *
            MsgPort that receives the struct ASocket_Notify messages.

        AS_TAG_NOTIFY_FD_MASK       ULONG
            Mask of notify events - see FD_* in <sys/socket.h>

        AS_TAG_NOTIFY_NAME          APTR
            Value of the asn_Message.mn_Node.ln_Name field of
            the ASocket_Notify messages sent to the AS_TAG_NOTIFY_MSGPORT.

        Tags specific to AF_LINK/SOCK_RAW/0 sockets:

        AS_TAG_IFACE_INDEX          ULONG
            Interface id to attach to (1...N)

        AS_TAG_IFACE_NAME           CONST_STRPTR
            Interface name to attach to (ie "lo", "eth0", etc)

        AS_TAG_IFACE_IFF_MASK       ULONG
            Mask of interface flags - see IFF_* in <net/if.h>

        AS_TAG_IFACE_METRIC         ULONG
            Metric of the interface

    RESULT

        NULL on failure (only ENOMEM)

        Otherwise, returns a 'struct List *' owned by ASocketBase
        that the caller can inspect, and call ASocketGet() on its
        list items.

        Call ASocketListRelease() to return the List to ASocketBase.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketListRelease()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    AROS_FUNCTION_NOT_IMPLEMENTED(asocket);

    return NULL;

    AROS_LIBFUNC_EXIT
}

