/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Unlock a spinlock
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>

#include "exec_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1I(void, UnlockSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, aspin, A0),

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

    struct SpinLock *spin=(struct SpinLock *) aspin;
    struct Task *thistask=FindTask(NULL);

    if(spin->owner!=thistask) 
    {
        kprintf("[PANIC] Spinlock %lx held by task %p, but unlocked by task %p\n", 
                 spin, spin->owner, thistask);
        Alert( AN_SemCorrupt );
    }

    if(spin->owner==NULL)
    {
        kprintf("[PANIC] Spinlock %lx is not locked, but unlocked by task %p\n", spin, thistask);
        Alert( AN_SemCorrupt );
    }
    
    if(spin->nest>1) 
    {
        spin->nest--;
        return;
    }

    spin->owner=NULL;

    __sync_synchronize();
    spin->lock=0;

    AROS_LIBFUNC_EXIT
} /* UnlockSpin */

