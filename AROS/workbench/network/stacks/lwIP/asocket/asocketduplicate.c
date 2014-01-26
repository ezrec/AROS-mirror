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

    D(bug("%s: as=%p, new_as=%p\n", __func__, as, new_s));

    if (as->as_Node.ln_Type != NT_AS_SOCKET &&
        as->as_Node.ln_Type != NT_AS_OBTAINED)
        return EINVAL;
    
    if (new_s == NULL)
        return EFAULT;

    new_as = AllocVec(sizeof(*new_as), MEMF_ANY | MEMF_CLEAR);
    if (new_as) {
        CopyMem(as, new_as, sizeof(*as));
        err = bsd_socket_dup(ASocketBase->ab_bsd, as->as_bsd, &new_as->as_bsd);
        if (err == 0) {
            new_as->as_Node.ln_Type = NT_AS_SOCKET;

            ObtainSemaphore(&ASocketBase->ab_Lock);
            ADDTAIL(&ASocketBase->ab_SocketList, new_as);
            ReleaseSemaphore(&ASocketBase->ab_Lock);

            *new_s = new_as;
        } else {
            FreeVec(new_as);
        }
    } else {
        err = ENOMEM;
    }

    return err;

    AROS_LIBFUNC_EXIT
}

