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

AROS_LH1(void, KrnScheduling,
    AROS_LHA(BYTE, trigger, D0),
    struct KernelBase *, KernelBase, 43, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;
    unsigned int thiscpu = KrnGetCPUNumber();

    if (pd->iface == NULL || pd->thread == NULL)
        return;

    pd->iface->pthread_mutex_lock(&pd->forbid_mutex);

    if (trigger > 0)
        AROS_ATOMIC_INC(SysBase->TDNestCnt);
    else if (trigger < 0)
        AROS_ATOMIC_DEC(SysBase->TDNestCnt);

    D(bug("[KRN] KrnScheduling(%d) --> SysBase->TDNestCnt = %d\n", trigger, SysBase->TDNestCnt));

    if (SysBase->TDNestCnt >= 0 && pd->forbid_cpu != thiscpu) {
        /* Forbid semantics:
         *  - Stop task switching on this CPU
         *  - Stop all other CPUs
         */

        while (pd->forbid_cpu >= 0 && pd->forbid_cpu != thiscpu) {
            struct KrnUnixThread *thread = &pd->thread[thiscpu];
            /* Wait for release of forbid */
            thread->state = STATE_STOPPED;
            pd->iface->pthread_cond_broadcast(&thread->state_cond);
            D(bug("CPU%d: Waiting for CPU%d to give up Forbid()\n", thiscpu, pd->forbid_cpu));
            while (thread->state == STATE_STOPPED) {
                pd->iface->pthread_cond_wait(&thread->state_cond, &pd->forbid_mutex);
            }
        }

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
            D(bug("CPU%d: Insane - we already waited for this!\n", thiscpu));
            asm volatile ("int3");
        }
        D(bug("CPU%d: Forbidden\n", thiscpu));
    } else if (SysBase->TDNestCnt < 0 && pd->forbid_cpu != -1) {
        /* Permit semantics:
         *  - Start all other CPUs
         *  - Enable task switching
         */

        while (pd->forbid_cpu >= 0 && pd->forbid_cpu != thiscpu) {
            struct KrnUnixThread *thread = &pd->thread[thiscpu];
            /* Wait for release of forbid */
            thread->state = STATE_STOPPED;
            pd->iface->pthread_cond_broadcast(&thread->state_cond);
            D(bug("CPU%d: Waiting for CPU%d to give up Forbid()\n", thiscpu, pd->forbid_cpu));
            while (thread->state == STATE_STOPPED) {
                pd->iface->pthread_cond_wait(&thread->state_cond, &pd->forbid_mutex);
            }
        }

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

    pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);

    AROS_LIBFUNC_EXIT
}
