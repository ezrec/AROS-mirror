/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Lock a spinlock
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>

#define MIN_PRI -120

/*****************************************************************************

    NAME */

        AROS_LH1I(void, LockSpin,

/*  SYNOPSIS */
        AROS_LHA(APTR, spin, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 187, Exec)

/*  FUNCTION
        Lock a spinlock exclusively. If the lock is already held
        by another task, busy wait until the lock is free again.
        If more than one task waits for an unlock, it is not
        predictable, which task will get the lock next.

    INPUTS
        spin - Pointer to spin (at the moment, a pointer to LONG)

    RESULT

    NOTES
        A spinlock does nothing more than to busy wait until
        the spin variable is 0. If it is 0, it sets it to 1 and
        returns. Spinlocks are protected against concurrency, so
        they work both in SMP and non SMP systems. On single CPU
        systems, Forbid() or ObtainSemaphore usually gives better
        performance.

    EXAMPLE

    BUGS
        If a task below priority MIN_PRI (-120) holds a lock, a deadlock
        may happen.

    SEE ALSO
        UnlockSpin()

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

    struct Task *thistask=FindTask(NULL);
    /* get current task priority (without changing it) */
    BYTE org_priority=thistask->tc_Node.ln_Pri;
    BYTE akt_priority=org_priority;
    ULONG count=0;

    while( /*(*(int *)spin==1) ||*/ __sync_lock_test_and_set((int *)spin, 1) ) 
    {
        count++;

        /* If we are busy far too long, lower our priority, if still possible */
        /* TODO: Is 0xFFFF a good value? more tests need to be done here */
        if(count>0xFFFF) 
        {
            count=0;

            if(akt_priority!=MIN_PRI) 
            {
                /* we can go lower */
                akt_priority=akt_priority-5;
                if(akt_priority<MIN_PRI) 
                {
                    /* reached lower end */
                    akt_priority=MIN_PRI;
                }
                /* lower priority */

                D(bug("LOCKSPIN: lower %lx to akt_priority %d\n", thistask, akt_priority));
                SetTaskPri(thistask, akt_priority);
                //thistask->tc_Node.ln_Pri=akt_priority;
            }
        }
#if 0
        else 
        {
            /* relax CPU ?? */
        }
#endif
    }

    /* reset original priority */
    if(akt_priority!=org_priority) 
    {
        SetTaskPri(thistask, org_priority);
        //thistask->tc_Node.ln_Pri=org_priority;
    }

    AROS_LIBFUNC_EXIT
} /* LockSpin */

