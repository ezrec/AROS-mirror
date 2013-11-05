/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Unlock a spinlock
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>
#include <proto/kernel.h>

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
        LockSpin(), ResetSpin()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct SpinLock *spin=(struct SpinLock *) aspin;
    struct Task *thistask=FindTask(NULL);
    char *taskname=NULL;

    if(thistask)
        taskname=thistask->tc_Node.ln_Name;

    if(spin->nest == 0)
    {
        bug("[PANIC] Spinlock %lx is not locked, but unlocked by task %p (%s)\n", spin, thistask, taskname);
        Alert( AN_SpinCorrupt );
    }

    /* early boot time might not have had thistask, so owner was set to -1 and Forbid() was used to lock */
    if(spin->owner == (struct Task *) -1) 
    {
        D(bug("[UNLOCKSPIN] %lx fallback to Permit()\n", spin));
        spin->nest--;
        if(spin->nest == 0) 
        {
            spin->owner=(struct Task *) NULL;
            __sync_synchronize();
            spin->lock=0;
        }
        /* don't call Permit(), as this calls UnlockSpin => recursion */
        KrnScheduling(KSCHED_PERMIT);
        return;
    }

    if(spin->owner!=thistask) 
    {
        kprintf("[PANIC] Spinlock %lx held by task %p, but unlocked by task %p (%s)\n", 
                 spin, spin->owner, thistask, taskname);
        Alert( AN_SpinCorrupt );
    }

    if(spin->nest>1) 
    {
        spin->nest--;
        return;
    }

    spin->owner=NULL;
    spin->nest =0;

    //D(bug("[UNLOCKSPIN] %lx unlocked by task %p (%s)\n", spin, thistask, taskname));

    __sync_synchronize();
    spin->lock =0;

    AROS_LIBFUNC_EXIT
} /* UnlockSpin */

