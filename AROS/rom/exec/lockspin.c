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

        AROS_LH1I(void, LockSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, aspin, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 187, Exec)

/*  FUNCTION
        Lock a spinlock exclusively. If the lock is already held
        by another task, busy wait until the lock is free again.
        If more than one task waits for an unlock, it is not
        predictable, which task will get the lock next.

    INPUTS
        spin - Pointer to spin (TODO: allocated with AllocSpin)

    RESULT

    NOTES
        A spinlock does nothing more than to busy wait until
        the spin variable is 0. If it is 0, it sets it to 1 and
        returns. Spinlocks are protected against concurrency, so
        they work both in SMP and non SMP systems. On single CPU
        systems, Forbid() or ObtainSemaphore usually gives better
        performance.

        Like Forbid/Permit every single LockSpin must be followed by
        one UnlockSpin. Only the task, which locked the spin
        is allowed to free it again.

    EXAMPLE

    BUGS
        If a task below priority MIN_PRI (-120) holds a lock, a deadlock
        may happen.

    SEE ALSO
        AllocSpin(), UnlockSpin()

    INTERNALS
        Problem on AROS is, that a busy waiting task with a higher
        priority never gives any CPU time to lower priority tasks.
        So a lower priority task, which locked the spin before, has
        no chance to unlock it again. So a deadlock happens.

        So we *need* to give away some CPU time to other tasks, even
        if this reduces the reaction time of the spinlock or as an
        alternative, we need to lower the owning task's priority. 

        On Unix-like systems, a busy waiting tasks is scheduled with
        a lower priority automatically by the scheduler. As the AROS
        scheduler does not do this, we lower our priority ourselves.
        As soon as we got the lock, we resume with the original 
        priority.

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct SpinLock *spin=(struct SpinLock *) aspin;
    struct Task *thistask=FindTask(NULL);
    /* get current task priority (without changing it) */
    BYTE org_priority=thistask->tc_Node.ln_Pri;
    BYTE akt_priority=org_priority;
    ULONG count=0;

    while( __sync_lock_test_and_set(&spin->lock, 1) ) 
    {
        if(spin->owner==thistask) 
        {
            D(bug("[LOCKSPIN] reentry #%d of task %p (%s)\n", spin->nest, thistask, thistask->tc_Node.ln_Name));
            spin->nest++;
            return;
        }

        count++;
        /* If we are busy far too long, lower our priority, if still possible */
        /* TODO: Is 0xFFFF a good value? more tests need to be done here */
        if(count>0xFFFF) 
        {
            count=0;

            if(akt_priority>MIN_PRI+5) 
            {
                /* we can go lower */
                akt_priority=akt_priority-5;
                if(akt_priority <= MIN_PRI) 
                {
                    /* reached lower end */
                    akt_priority=MIN_PRI;
                }
                /* lower priority
                 * we could access owner->tc_Node.ln_Pri, but there is a slight chance, that
                 * the owner tasks ends, before we get the priority. And we can't use Forbid() here
                 */

                D(bug("[LOCKSPIN] task %p still holds the spinlock %lx (%d) with priority %d (%s)\n",
                        spin->owner, spin, spin->lock, spin->owner->tc_Node.ln_Pri, 
                        spin->owner->tc_Node.ln_Name));

                D(bug("[LOCKSPIN] lower this task %lx to priority %d (%s)\n", 
                      thistask, akt_priority, thistask->tc_Node.ln_Name));
                SetTaskPri(thistask, akt_priority);
            }
        }
#if 0
        else 
        {
            /* relax CPU ?? */
        }
#endif
    }

    spin->nest=1;
    spin->owner=thistask;

    /* reset original priority */
    if(akt_priority!=org_priority) 
    {
        SetTaskPri(thistask, org_priority);
    }

    AROS_LIBFUNC_EXIT
} /* LockSpin */

