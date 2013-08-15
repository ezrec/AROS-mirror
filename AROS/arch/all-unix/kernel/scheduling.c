/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#define DEBUG 0

#include <aros/atomic.h>
#include <aros/debug.h>
#include <aros/kernel.h>
#include <aros/libcall.h>
#include <proto/kernel.h>

#include "kernel_debug.h"
#include "kernel_base.h"
#include "kernel_intern.h"

AROS_LH1(BYTE, KrnScheduling,
    AROS_LHA(LONG, trigger, D0),
    struct KernelBase *, KernelBase, 43, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;
    unsigned int thiscpu = KrnGetCPUNumber();
    BYTE ret;

    if (pd->iface == NULL || pd->thread == NULL)
        return -1;

    pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
    D(bug("[KRN] KrnScheduling(%d) --> SysBase->TDNestCnt = %d, pd->forbid_depth = %d\n", trigger, SysBase->TDNestCnt, pd->forbid_depth));

    /* Wait to get a claim on the forbid_cpu indicator */
    while (pd->forbid_cpu >= 0 && pd->forbid_cpu != thiscpu) {
        pd->thread[thiscpu].state = STATE_STOPPED;
        pd->iface->pthread_cond_broadcast(&pd->thread[thiscpu].state_cond);
        while (pd->thread[thiscpu].state != STATE_RUNNING) {
            pd->iface->pthread_cond_wait(&pd->thread[thiscpu].state_cond, &pd->forbid_mutex);
        }
    }

    if (trigger == KSCHED_INSPECT) {
        /* Don't modify trigger */
    } else if (trigger == KSCHED_FORBID) {
        pd->forbid_depth++;
    } else if (trigger == KSCHED_PERMIT) {
        pd->forbid_depth--;
    } else if (-128 <= trigger && trigger <= 127) {
        pd->forbid_depth = trigger;
    } else {
        bug("[KRN] KrnScheduling: Invalid trigger '%d'\n", trigger);
        asm volatile ("int3");
    }

    if (pd->forbid_depth >= 0) {
        /* Forbid semantics:
         *  - Stop task switching on this CPU
         *  - Stop all other CPUs
         */
        if (pd->forbid_cpu == thiscpu) {
            /* Scheduler alread stopped */
        } else if (pd->forbid_cpu == -1) {
            int i;
            D(bug("CPU%d: Stop all other CPUs\n", thiscpu));
            pd->forbid_cpu = thiscpu;
            for (i = 0; i < pd->threads; i++) {
                struct KrnUnixThread *thread = &pd->thread[i];
                if (i == thiscpu || thread->tid == 0)
                    continue;
                D(bug("  CPU%d: Stop\n", i));
                /* Wait for ack from suspended CPU */
                while (thread->state == STATE_RUNNING) {
                    pd->iface->pthread_cond_wait(&thread->state_cond, &pd->forbid_mutex);
                }
            }
        } else {
            bug("CPU%d: CPU%d has the Forbid() - how did I get here?!\n", thiscpu, pd->forbid_cpu);
            asm volatile ("int3");
        }
        D(bug("CPU%d: Forbidden\n", thiscpu));
    } else {
        /* Permit semantics:
         *  - Start all other CPUs
         *  - Enable task switching
         */
        if (pd->forbid_cpu == -1) {
            /* Scheduler already started */
        } else if (pd->forbid_cpu == thiscpu) {
            int i;
            D(bug("CPU%d: Start all other CPUs\n", thiscpu));
            for (i = 0; i < pd->threads; i++) {
                struct KrnUnixThread *thread = &pd->thread[i];
                if (i == thiscpu || thread->tid == 0)
                    continue;
                if (thread->state == STATE_IDLE)
                    continue;
                D(bug("  CPU%d: Cont\n", i));
                thread->state = STATE_RUNNING;
                pd->iface->pthread_cond_broadcast(&thread->state_cond);
            }
            pd->forbid_cpu = -1;
        } else {
            D(bug("CPU%d: Attempted to Permit while CPU%d has Forbid\n", thiscpu, pd->forbid_cpu));
            asm volatile ("int3");
        }
        D(bug("CPU%d: Permitted\n", thiscpu));
    }
    ret = pd->forbid_depth;
    pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);

    return ret;

    AROS_LIBFUNC_EXIT
}
