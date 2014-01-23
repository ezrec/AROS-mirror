/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH2(LONG, ASocketDuplicate,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(APTR *, new_s, A1),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 7, ASocket)

/*  FUNCTION
 
        Duplicate an ASocket handle. Note that the duplicate
        is identical in all ways to the original, except that
        ASocketDispose() of the original or duplicate leaves
        the other undisturbed.

    INPUTS

        as - ASocket handle
        new_as - Location to recieve the duplicate ASocket handle

    RESULT

        0 on success, and 'errno' value on failue.

    NOTES

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

    if (new_as == NULL)
        return EFAULT;

    err = bsd_socket_dup(ASocketBase->ab_bsd, (struct bsd_sock *)as, (struct bsd_sock **)&new_as);
    if (err == 0 && as != new_as) {
        new_as->as_Socket.domain = as->as_Socket.domain;
        new_as->as_Socket.type = as->as_Socket.type;
        new_as->as_Socket.protocol = as->as_Socket.protocol;
        *new_s = new_as;
    }

    return err;

    AROS_LIBFUNC_EXIT
}

