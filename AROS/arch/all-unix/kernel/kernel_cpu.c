/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1

#include <aros/debug.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <hardware/intbits.h>
#include <proto/exec.h>

#include <unistd.h>

#include "kernel_base.h"
#include "kernel_debug.h"
#include "kernel_globals.h"
#include "kernel_intern.h"
#include "kernel_intr.h"
#include "kernel_scheduler.h"

/*
 * Task exception handler.
 * Exceptions work in a way similar to MinGW32 port. We can't manipulate stack inside
 * UNIX signal handler, because context's SP is set to a point where it was at the moment
 * when signal was caught. Signal handler uses some processes' stack space by itself, and
 * if we try to use some space below SP, we will clobber signal handler's stack.
 * In order to overcome this we disable interrupts and jump to exception handler. Since
 * interrupts are disabled, et_RegFrame of our task still contains original context (saved
 * at the moment of task switch). In our exception handler we already left the signal handler,
 * so we can allocate some storage on stack and place our context there. After this we call
 * exec's Exception().
 * When we return, we place our saved context back into et_RegFrame and cause a SysCall (SIGUSR1)
 * with a special TS_EXCEPT state. SysCall handler will know then that it needs just to dispatch
 * the same task with the saved context (see cpu_DispatchContext() routine).
 */
static void cpu_Exception(void)
{
    struct KernelBase *KernelBase = getKernelBase();
    /* Save return context and IDNestCnt on stack */
    struct Task *task = THISCPU->ThisTask;
    char nestCnt = task->tc_IDNestCnt;
    char save[KernelBase->kb_ContextSize];
    APTR savesp;

    /* Save original context */
    CopyMem(task->tc_UnionETask.tc_ETask->et_RegFrame, save, KernelBase->kb_ContextSize);
    savesp = task->tc_SPReg;

    Exception();

    /* Restore saved task state and resume it. Note that interrupts are
       disabled again here */
    task->tc_IDNestCnt = nestCnt;
    SysBase->IDNestCnt = nestCnt;

    /* Restore saved context */
    CopyMem(save, task->tc_UnionETask.tc_ETask->et_RegFrame, KernelBase->kb_ContextSize);
    task->tc_SPReg = savesp;

    /* This tells task switcher that we are returning from the exception */
    THISCPU->ThisTask->tc_State = TS_EXCEPT;

    /* System call */
    KernelBase->kb_PlatformData->iface->raise(SIGUSR1);
    AROS_HOST_BARRIER
}

void cpu_Switch(regs_t *regs)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct Task *task = THISCPU->ThisTask;
    struct AROSCPUContext *ctx = task->tc_UnionETask.tc_ETask->et_RegFrame;

    D(bug("[KRN] cpu_Switch(), task %p (%s)\n", task, task->tc_Node.ln_Name));
    D(PRINT_SC(regs));

    SAVEREGS(ctx, regs);
    ctx->errno_backup = *KernelBase->kb_PlatformData->errnoPtr;
    task->tc_SPReg = (APTR)SP(regs);
    core_Switch();
}

/* We have three cases when we enter cpu_Dispatch:
 *   - No CPUs are have disabled task switching
 *   - A CPU (not us) has disabled task switching
 *      - In this case, we suspend, and wait for that
 *        CPU to put us back in running state
 *   - This CPU has disabled task switching
 *      - Since we are in Dispatch state, we have to
 *        start task switching again.
 */
static void thread_SuspendOrBreakForbid(struct KernelBase *KernelBase)
{
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KrnUnixThread *thread;
    unsigned int thiscpu = KrnGetCPUNumber();

    thread = &pd->thread[thiscpu];

    pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
    if (pd->forbid_cpu == thiscpu) {
        int i;
        D(bug("CPU%d: Breaking forbid state\n", thiscpu));
        for (i = 0; i < pd->threads; i++) {
            if (i == thiscpu || pd->thread[i].state != STATE_STOPPED)
                continue;
            pd->thread[i].state = STATE_RUNNING;
            pd->iface->pthread_cond_broadcast(&pd->thread[i].state_cond);
        }
        pd->forbid_cpu = -1;
    }
    pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);
    pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
    while (pd->forbid_cpu >= 0 && pd->forbid_cpu != thiscpu) {
        D(bug("CPU%d: Suspending by request from %d\n", thiscpu, pd->forbid_cpu));

        /* Let the forbid holder know we're alseep */
        thread->state = STATE_STOPPED;
        pd->iface->pthread_cond_broadcast(&thread->state_cond);
        D(bug("CPU%d: Stopped...\n", thiscpu));
        pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);
        pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
        
       /* Wait to be awoken */
        while (thread->state != STATE_RUNNING) {
            pd->iface->pthread_cond_wait(&thread->state_cond, &pd->forbid_mutex);
        }
        D(bug("CPU%d: Continuing...\n", thiscpu));
    }
    pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);
}

void cpu_Dispatch(regs_t *regs)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct Task *task;
    sigset_t sigs;
#if AROS_SMP
    unsigned int cpu = KrnGetCPUNumber();
#else
    unsigned int cpu = 0;
#endif

    /* This macro relies on 'pd' being present */
    SIGEMPTYSET(&sigs);

     /* If scheduling is disabled, suspend */
    thread_SuspendOrBreakForbid(KernelBase);
    while (!(task = core_Dispatch()))
    {
        if (cpu == 0) {
            /* Sleep almost forever ;) */
            KernelBase->kb_PlatformData->iface->sigsuspend(&sigs);
            AROS_HOST_BARRIER
        }

        if (cpu == 0) {
            /* If IRQs are disabled, re-enable */
            if (!pd->irq_enable) {
                pd->irq_enable = TRUE;
            }

            if ((THISCPU->SysFlags & SFF_SoftInt))
                core_Cause(INTB_SOFTINT, 1L << INTB_SOFTINT);
        }
    
        thread_SuspendOrBreakForbid(KernelBase);
    }

    D(bug("[KRN] cpu_Dispatch(), task %p (%s)\n", task, task->tc_Node.ln_Name));
    cpu_DispatchContext(task, regs, pd);
}

void cpu_DispatchContext(struct Task *task, regs_t *regs, struct PlatformData *pd)
{
    struct AROSCPUContext *ctx;

    /* If task is in TS_REMOVED state, RemTask has already uninitialized 
     * the ETask structure. So check for a valid ETask. 
     * WARNING: not sure, if this is a good idea! 
     */
    if((task->tc_Flags & TF_ETASK) && (task->tc_State != TS_REMOVED))
    {
        ctx = task->tc_UnionETask.tc_ETask->et_RegFrame;

        RESTOREREGS(ctx, regs);
        *pd->errnoPtr = ctx->errno_backup;

        D(PRINT_SC(regs));
    }
    else 
    {
        bug("[KRN] WARNING: CPU%d: task %p has no valid ETask!\n", KrnGetCPUNumber(), task);
        /* what next?? */
    }

    if (task->tc_Flags & TF_EXCEPT)
    {
        /* Disable interrupts, otherwise we may lose saved context */
        SysBase->IDNestCnt = 0;

        /* Manipulate the current cpu context so Exec_Exception gets
           excecuted after we leave the kernel resp. the signal handler. */
        PC(regs) = (IPTR)cpu_Exception;
    }

    /*
     * Adjust user mode interrupts state.
     * Brackets MUST present, these are complex macros.
     */
    if (SysBase->IDNestCnt < 0)
    {
        D(bug("CPU%d: %p: Enabled\n", KrnGetCPUNumber(), THISCPU->ThisTask));
#if AROS_SMP
        KrnSti();
#else
        SC_ENABLE(regs);
#endif
    }
    else
    {
        D(bug("CPU%d: %p: Disabled\n", KrnGetCPUNumber(), THISCPU->ThisTask));
#if AROS_SMP
        KrnCli();
#else
        SC_DISABLE(regs);
#endif
    }
}
