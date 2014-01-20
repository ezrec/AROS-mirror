/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
        AROS_LH1(VOID, ASocketDispose,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),

/*  LOCATION */
        struct Library *, ASocketBase, 6, ASocket)

/*  FUNCTION
 
        Dispose of a socket handle.

    INPUTS

        as - Socket handle to dispose.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct ASocket *a = (struct ASocket *)as;

    if (a == NULL)
        return;

    if (--a->as_Usage == 0) {
        ObtainSemaphore(&ASocketBase->ab_Lock);
        REMOVE(a);
        ReleaseSemaphore(&ASocketBase->ab_Lock);
        FreeVec(a);
    }

    return;

    AROS_LIBFUNC_EXIT
}

