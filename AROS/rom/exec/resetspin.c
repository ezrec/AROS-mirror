/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Lock a spinlock
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>

#include "exec_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(ULONG, ResetSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, aspin, A0),
        AROS_LHA(ULONG, parameter, D0),

/*  LOCATION */
        struct ExecBase *, SysBase, 189, Exec)

/*  FUNCTION
        Reset a spinlock. If the lock is already held
        by a task, unlock it by force, indepenently of
        how many nesting levels the lock has.

    INPUTS
        spin - Pointer to spin (TODO: allocated with AllocSpin)
        parameter:
          0: only release all locks held by us
          1: remove all spins, even foreign ones

    RESULT
        Returns the amount of nested LockSpin calls, which
        have not been unlocked by UnlockSpin.

    NOTES
        You should never have to call it on a foreign locked spinlock.
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
    char  *secname="(null)";
    struct Task *mytask;
    char  *mytaskname="(null)";
    ULONG reset=1;

    if(parameter>1) 
    {
        bug("[RESETSPIN] called with unknown paramater %d!\n", parameter);
        Alert( AN_SpinCorrupt );
        return 0;
    }

    if(spin->nest == 0) 
        return 0;

    mytask=FindTask(NULL);
    /* race condition security */
    sectask=spin->owner;

    if((parameter==0) && (mytask!=spin->owner))
    {
#if DEBUG
        if(mytask) 
            mytaskname=mytask->tc_Node.ln_Name;

        if(sectask && (sectask!=(struct Task *) -1))
            secname=sectask->tc_Node.ln_Name;

        D(bug("[RESETSPIN] INFO: lock %lx soft reset by %p (%s), but different owner %p (%s)\n", 
               spin, mytask, mytaskname, sectask, secname));
#endif
        return 0;
    }

    /* now we have to reset the lock and return the original nest count */

#if DEBUG
    if(sectask && (sectask != (struct Task *) -1))
    {
        D(bug("[RESETSPIN] INFO: spinlock %lx reset from me (task %p (%s)), although it was still held by task %p (%s)!\n", 
                spin, mytask, mytaskname, sectask, sectask->tc_Node.ln_Name));
    }
    else 
    {
        D(bug("[RESETSPIN] INFO: spinlock %lx reset request from me (task %p (%s)), although it was held by a NULL/* task!?\n", spin, mytask, mytaskname));
    }
#endif

     /* remember original nest count */
    nest=spin->nest;
  
    spin->owner=NULL;
    spin->nest =0;

    __sync_synchronize();
    spin->lock =0;

    return nest;

    AROS_LIBFUNC_EXIT
} /* InitSpin */

