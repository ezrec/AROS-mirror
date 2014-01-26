/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH2(LONG, ASocketAccept,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(APTR *, new_s, A1),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 16, ASocket)

/*  FUNCTION
  
        If there is a FD_ACCEPT pending, this function creates a
        new ASocket handle for the incoming connection.

    INPUTS

        as - ASocket handle that is listening
        new_as - Location to place the new ASocket handle

    RESULT

        0      - Success, new ASocket handle in 'new_as'
        ENOMEM - Out of memory allocation new handle
        EAGAIN - There is no pending FD_ACCEPT
        EINVAL - This socket has no AS_TAG_LISTEN_BACKLOG set

    NOTES

        An ASocket must have its AS_TAG_LISTEN_BACKLOG set to
        a non-zero value to be able to accept incoming connections.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    int err;
    struct ASocket *as = s, *new_as;

    D(bug("%s: as=%p, new_as=%p\n", __func__, s, new_s));

    if (as->as_Node.ln_Type != NT_AS_SOCKET)
        return EINVAL;
    
    new_as = AllocVec(sizeof(*new_as), MEMF_CLEAR | MEMF_ANY);
    if (new_as != NULL) {
        err = bsd_accept(ASocketBase->ab_bsd, as->as_bsd, &new_as->as_bsd);
        if (err == 0) {
            new_as->as_Node.ln_Type = NT_AS_SOCKET;
            new_as->as_Socket.domain = as->as_Socket.domain;
            new_as->as_Socket.type = as->as_Socket.type;
            new_as->as_Socket.protocol = as->as_Socket.protocol;
            *new_s = new_as;

            ObtainSemaphore(&ASocketBase->ab_Lock);
            ADDTAIL(&ASocketBase->ab_SocketList, new_as);
            ReleaseSemaphore(&ASocketBase->ab_Lock);
        }
    } else {
        err = ENOMEM;
    }

    return err;

    AROS_LIBFUNC_EXIT
}

