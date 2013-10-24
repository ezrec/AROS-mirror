/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Lock a spinlock
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>

#include "exec_intern.h"

#define MIN_PRI -120

/*****************************************************************************

    NAME */

        AROS_LH1I(ULONG, ResetSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, aspin, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 189, Exec)

/*  FUNCTION
        Reset a spinlock. If the lock is already held
        by a task, unlock it by force, indepenently of
        how many nesting levels the lock has.

    INPUTS
        spin - Pointer to spin (TODO: allocated with AllocSpin)

    RESULT
        Returns the amount of nested LockSpin calls, which
        have not been unlocked by UnlockSpin.

    NOTES
        You should never have to call it on a locked spinlock.
        Rethink twice before you do.

    EXAMPLE

    BUGS
        This has race condition bugs! Imagine, what happens, if someone
        calls LockSpin inbetween, or UnlockSpin..
        So only call it from Forbid/Permit to be safe!

    SEE ALSO
        AllocSpin(), UnlockSpin(), ResetSpin()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct SpinLock *spin=(struct SpinLock *) aspin;
    ULONG  nest=0;
    struct Task *sectask;

    nest=spin->nest;

    if(spin->nest > 0) 
    {
        sectask=spin->owner;
        if(sectask && (sectask != (struct Task *) -1)) 
        {
            D(bug("[RESETSPIN] WARNING: spinlock %lx reset, although it was still held by task %p (%s)!\n", 
                   spin, sectask, sectask->tc_Node.ln_Name));
        }
        else 
        {
            D(bug("[RESETSPIN] WARNING: spinlock %lx reset, although it was held by a NULL task!?\n", spin));
        }
        nest=spin->nest;
    }
    spin->owner=NULL;
    spin->nest =0;

    __sync_synchronize();
    spin->lock =0;

    return nest;

    AROS_LIBFUNC_EXIT
} /* InitSpin */

