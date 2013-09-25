/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Unlock a spinlock
    Lang: english
*/

#include <proto/exec.h>

/*****************************************************************************

    NAME */

        AROS_LH1I(void, UnlockSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, spin, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 188, Exec)

/*  FUNCTION
        Release a spinlock obtained with LockSpin. Each call to LockSpin
        must be accompanied by one call to UnlockSpin.

    INPUTS
        spin - Pointer to spin 

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        LockSpin()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

     __sync_synchronize();
     *(ULONG *)spin=0;

    AROS_LIBFUNC_EXIT
} /* UnlockSpin */

