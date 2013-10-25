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
        systems, Forbid() or ObtainSemaphore usually give better
        performance.

        Like Forbid/Permit every single LockSpin must be followed by
        one UnlockSpin. Only the task, which locked the spin
        is allowed to free it again.

    EXAMPLE

    BUGS
        If a task below priority MIN_PRI (-120) holds a lock, a deadlock
        may happen.

    SEE ALSO
        AllocSpin(), UnlockSpin(), ResetSpin()

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
    BYTE org_priority;
    BYTE akt_priority;
    char *taskname; /* TODO: remove it again */
    ULONG count=0;

    /* allowed to be called early */
    /* a spinlock once locked with Forbid needs following Forbid locks, too */
    if((thistask == NULL) || (spin->owner == (struct Task *) -1) )
    {
        /* if we are already locked, we just increase the count, even if thistask is NULL
         * if we are not locked ATM, we try Forbid(). 
         */
        D(bug("[LOCKSPIN] thistask is NULL, fallback to Forbid()\n"));
        /* don't call Forbid(), as this calls LockSpin itself */
        KrnScheduling(KSCHED_FORBID);
        spin->owner=(struct Task *) -1;
        spin->nest++;
        return;
    }

    org_priority=thistask->tc_Node.ln_Pri;
    akt_priority=org_priority;
    taskname=thistask->tc_Node.ln_Name;

    /* no need to lock this access, either it is us, then there can't be a race or
     * it is NULL/another task, then the owner is different
     */
    if(spin->owner==thistask) 
    {
        D(bug("[LOCKSPIN] %lx reentry #%d of task %p (%s)\n", spin, spin->nest, thistask, taskname));
        spin->nest++;
        return;
    }
    else
    {
        D(bug("[LOCKSPIN] %lx task %p (%s) locking me ..\n", spin, thistask, taskname));
        //D(bug("[LOCKSPIN] %lx task %p (%s) locking me from 0x%p / 0x%p / 0x%p / 0x%p / 0x%p / 0x%p / 0x%p\n", spin, thistask, taskname, __builtin_return_address (1), __builtin_return_address (2), __builtin_return_address (3), __builtin_return_address (4), __builtin_return_address (5), __builtin_return_address (6), __builtin_return_address (7)));
    }

    while( __sync_lock_test_and_set(&spin->lock, 1) ) 
    {
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

                struct Task *sectask=spin->owner;
                /* DEBUG only, owner might be dead already! */
                if(sectask && (sectask != (struct Task *) -1))
                {
                  /* might still return dead values, but won't crash at least */
                  D(bug("[LOCKSPIN] task %p (%s) still holds the spinlock %lx with priority %d\n",
                        sectask, sectask->tc_Node.ln_Name, spin, sectask->tc_Node.ln_Pri));
                }
                else {
                    bug("[LOCKSPIN] ERROR: spin->owner is NULL, but spinlock %lx is still locked\n", spin);
                    bug("[LOCKSPIN] ERROR: BRUTE FORCE UNLOCK OF spinlock %lx\n", spin);
                    Alert( AN_SpinCorrupt );
                    /* maybe we are lucky.. */
                    spin->nest=0;
                    __sync_synchronize();
                    spin->lock=0;
                }


                D(bug("[LOCKSPIN] lower this task %lx (%s) to priority %d\n", 
                       thistask, taskname, akt_priority));
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
    D(bug("[LOCKSPIN] %lx task %p (%s) locked me!\n", spin, thistask, taskname));

    spin->nest=1;
    spin->owner=thistask;

    /* reset original priority */
    if(akt_priority!=org_priority) 
    {
        SetTaskPri(thistask, org_priority);
    }

    //D(bug("[LOCKSPIN] %lx locked by task %p (%s)\n", spin, thistask, taskname));

    AROS_LIBFUNC_EXIT
} /* LockSpin */

