/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Initialize the interface to the "hardware".
    Lang: english
*/

#include <exec/interrupts.h>
#include <exec/execbase.h>
#include <aros/asmcall.h>
#include <aros/atomic.h>
#include <aros/symbolsets.h>
#include <hardware/intbits.h>
#include <proto/exec.h>
#include <proto/hostlib.h>

#define timeval sys_timeval

#include "hostinterface.h"
#include "kernel_base.h"
#include "kernel_debug.h"
#include "kernel_globals.h"
#include "kernel_intr.h"
#include "kernel_intern.h"
#include "kernel_interrupts.h"
#include "kernel_scheduler.h"
#include "kernel_unix.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#undef timeval

#undef D
#define D(x)
#define DSC(x)

#ifdef SIGCORE_NEED_SA_SIGINFO
#define SETHANDLER(sa, h) 			\
    sa.sa_sigaction = h ## _gate;	\
    sa.sa_flags |= SA_SIGINFO
#else
#define SETHANDLER(sa, h) 			\
    sa.sa_handler = h ## _gate;
#endif

static void core_TrapHandler(int sig, regs_t *regs)
{
    struct KernelBase *KernelBase = getKernelBase();
    const struct SignalTranslation *s;
    short amigaTrap;
    struct AROSCPUContext ctx;

    SUPERVISOR_ENTER;

    /* Just for completeness */
    krnRunIRQHandlers(KernelBase, sig);

    bug("[KRN] Trap signal %d, SysBase %p, KernelBase %p\n", sig, SysBase, KernelBase);
    PRINT_SC(regs);

    /* Translate UNIX trap number to CPU and exec trap numbers */
    for (s = sigs; s->sig != -1; s++)
    {
	if (sig == s->sig)
	    break;
    }

    /*
     * Trap handler expects struct ExceptionContext, so we have to convert regs_t to it.
     * But first initialize all context area to zero, this is important since it may include
     * pointers to FPU state buffers.
     * TODO: FPU state also can be interesting for debuggers, we need to prepare space for it
     * too. Needs to be enclosed in some macros.
     */
    memset(&ctx, 0, sizeof(ctx));
    SAVEREGS(&ctx, regs);

    amigaTrap = s->AmigaTrap;
    if (s->CPUTrap != -1)
    {
	if (krnRunExceptionHandlers(KernelBase, s->CPUTrap, &ctx))
	    /* Do not call exec trap handler */
	    amigaTrap = -1;
    }

    /*
     * Call exec trap handler if needed.
     * Note that it may return, this means that the it has
     * fixed the problem somehow and we may safely continue.
     */
    if (amigaTrap != -1)
    	core_Trap(amigaTrap, &ctx);

    /* Trap handler(s) have possibly modified the context, so
       we convert it back before returning */
    RESTOREREGS(&ctx, regs);

    SUPERVISOR_LEAVE;
}

static void core_IRQ(int sig, regs_t *sc)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    ULONG cpu = KrnGetCPUNumber();

    SUPERVISOR_ENTER;

    D(bug("%s: CPU%d <- Signal %d\n", __func__, cpu, sig));

    if (pd->forbid_cpu != -1) {
        if (sig == SIGVTALRM || sig == SIGALRM) {
            SUPERVISOR_LEAVE;
            return;
        }
    }

    if (cpu == 0) {
        ULONG i, maxcpu = KrnGetCPUCount();

        if (sig == SIGVTALRM ||
            sig == SIGALRM ||
            sig == SIGIO) {
            /* Pass the timer and IO IRQs to all other CPU threads
             * via the SIGURG signal
             */
            for (i = 1; i < maxcpu; i++) {
                pd->iface->write(pd->thread[i].signal[1], &sig, sizeof(sig));
                pd->iface->pthread_kill(pd->thread[i].tid, SIGURG);
            }
        }
    }

    if (cpu != 0 && (sig == SIGALRM || sig == SIGVTALRM)) {
        /* We handle SIGALRM/SIGVTALRM time quantas here
         * for CPU1...N 
         */
        if (THISCPU->Elapsed && (--THISCPU->Elapsed == 0)) {
            THISCPU->SysFlags    |= SFF_QuantumOver;
            THISCPU->AttnResched |= ARF_AttnSwitch;
        }
        if (THISCPU->Elapsed == 0)
            THISCPU->Elapsed = THISCPU->Quantum;
    } else {
        /* Just additional protection - what if there's more than 32 signals? */
        if (sig < IRQ_COUNT)
            krnRunIRQHandlers(KernelBase, sig);
    }

    if (UKB(KernelBase)->SupervisorCount == 1)
        core_ExitInterrupt(sc);

    SUPERVISOR_LEAVE;
}

#if AROS_SMP
static void core_Thread_SysCall(int sig, regs_t *sc)
{
    struct KernelBase *KernelBase = getKernelBase();

    if (THISCPU == NULL)
        return;

    core_SysCall(SIGUSR1, sc);
}

static void core_Thread_IRQ(int sig, regs_t *sc)
{
    /* This is *only* issued from the parent context
     */
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    unsigned int cpu = KrnGetCPUNumber();

    if (THISCPU == NULL)
        return;

    if (cpu == 0) {
        sigset_t sigs;
        SIGFILLSET(&sigs);
        SIGDELSET(&sigs, SIGURG);
        /* Actually reflects a change in IRQ status... */
        while (pd->irq_enable == FALSE) {
            /* Wait for a SIGURG when pd->irq_enable == TRUE */
            pd->iface->sigsuspend(&sigs);
        }
        return;
    }

    while (pd->iface->read(pd->thread[cpu].signal[0], &sig, sizeof(sig)) == sizeof(sig)) {
        D(bug("%s: CPU%d <- %d\n", __func__, cpu, sig));
        core_IRQ(sig, sc);
    }
}
#endif

/*
 * This is from sigcore.h - it brings in the definition of the
 * systems initial signal handler, which simply calls
 * sighandler(int signum, regs_t sigcontext)
*/
GLOBAL_SIGNAL_INIT(core_TrapHandler)
GLOBAL_SIGNAL_INIT(core_SysCall)
GLOBAL_SIGNAL_INIT(core_IRQ)
GLOBAL_SIGNAL_INIT(core_Thread_SysCall)
GLOBAL_SIGNAL_INIT(core_Thread_IRQ)

/* libc functions that we use */
static const char *kernel_functions[] =
{
    "raise",
#if AROS_SMP
    "pthread_sigmask",
#else
    "sigprocmask",
#endif
    "sigsuspend",
    "sigaction",
    "mprotect",
    "read",
    "fcntl",
    "mmap",
    "munmap",
#ifdef HOST_OS_linux
    "__errno_location",
#else
#ifdef HOST_OS_android
    "__errno",
#else
    "__error",
#endif
#endif
    "sigwait",
#ifndef HOST_OS_android
    "sigemptyset",
    "sigfillset",
    "sigaddset",
    "sigdelset",
#endif
#if AROS_SMP
    "pthread_create",
    "pthread_self",
    "pthread_kill",
    "pthread_cond_init",
    "pthread_cond_broadcast",
    "pthread_cond_signal",
    "pthread_cond_wait",
    "pthread_mutex_init",
    "pthread_mutex_lock",
    "pthread_mutex_unlock",
    "pthread_key_create",
    "pthread_setspecific",
    "pthread_getspecific",
    "write",
    "pipe2",
#endif
    NULL
};

/*
 * We have hostlib.resource and can use it in order to pick up all needed host OS functions.
 */
int core_Setup(void *libc, void *libpthread)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    APTR HostLibBase;
    ULONG r;

    /* We have hostlib.resource. Obtain the complete set of needed functions. */
    HostLibBase = OpenResource("hostlib.resource");
    if (!HostLibBase)
    {
    	krnPanic(KernelBase, "Failed to open hostlib.resource");
    	return FALSE;
    }

    pd->iface = (struct KernelInterface *)HostLib_GetInterface(libc, kernel_functions, &r);
    if (!pd->iface)
    {
    	krnPanic(KernelBase, "Failed to allocate host-side libc interface");
    	return FALSE;
    }

    if (r)
    {
        int i;
        krnPanic(KernelBase, "Failed to reslove %u functions from host libc, scanning libpthread", r);
        for (i = 0; kernel_functions[i] != NULL; i++) {
            if (((void **)(pd->iface))[i] == NULL) {
                char *err;
                void *sym;
                sym = HostLib_GetPointer(libpthread, kernel_functions[i], &err);
                if (sym == NULL) {
                    krnPanic(KernelBase, "libpthread: Can't resolve symbol %s: %s\n", kernel_functions[i], err);
                    return FALSE;
                }
                ((void **)(pd->iface))[i] = sym;
            }
        }
    }

    /* Cache errno pointer, for context switching */
    pd->errnoPtr = pd->iface->__error();
    AROS_HOST_BARRIER

    return TRUE;
}

/*
 * Our post-SINGLETASK initialization code.
 * At this point we are starting up interrupt subsystem.
 */
int core_Start(void)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct sigaction sa = {};
    const struct SignalTranslation *s;
    sigset_t tmp_mask;

#if DEBUG
    /* Pass unhandled exceptions to the debugger, if present */
    SIGEMPTYSET(&pd->sig_int_mask);
#else
    /* We only want signal that we can handle at the moment */
    SIGFILLSET(&pd->sig_int_mask);
#endif
    SIGEMPTYSET(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    /* 
     * These ones we consider as processor traps.
     * They are not be blocked by KrnCli()
     */
    SETHANDLER(sa, core_TrapHandler);
    for (s = sigs; s->sig != -1; s++)
    {
	pd->iface->sigaction(s->sig, &sa, NULL);
	AROS_HOST_BARRIER
	SIGDELSET(&pd->sig_int_mask, s->sig);
    }

    /* SIGUSRs are software interrupts, we also never block them */
    SIGDELSET(&pd->sig_int_mask, SIGUSR1);
    SIGDELSET(&pd->sig_int_mask, SIGUSR2);
    /* We want to be able to interrupt AROS using Ctrl-C in its console,
       so exclude SIGINT too. */
    SIGDELSET(&pd->sig_int_mask, SIGINT);

    /*
     * Any interrupt including software one must disable
     * all interrupts. Otherwise one interrupt may happen
     * between interrupt handler entry and supervisor count
     * increment. This can cause bad things in cpu_Dispatch()
     */
    sa.sa_mask = pd->sig_int_mask;

    /* Install interrupt handlers */
    SETHANDLER(sa, core_IRQ);
#if DEBUG
    /* Use VTALRM instead of ALRM during debugging, so
     * that stepping though code won't have to deal
     * with constant SIGALRM processing.
     *
     * NOTE: This will cause the AROS clock to march slower
     *       than the host clock in debug builds!
     */
    pd->iface->sigaction(SIGVTALRM, &sa, NULL);
    AROS_HOST_BARRIER
#else
    pd->iface->sigaction(SIGALRM, &sa, NULL);
    AROS_HOST_BARRIER
#endif
    pd->iface->sigaction(SIGIO  , &sa, NULL);
    AROS_HOST_BARRIER

    /* Software IRQs do not need to block themselves. Anyway we know when we send them. */
    sa.sa_flags |= SA_NODEFER;

    pd->iface->sigaction(SIGUSR2, &sa, NULL);
    AROS_HOST_BARRIER

    SETHANDLER(sa, core_SysCall);
    pd->iface->sigaction(SIGUSR1, &sa, NULL);
    AROS_HOST_BARRIER

    /* SIGSYS is explicitly sent to the CPU theads to trigger
     * their core_SysCall
     */
    SETHANDLER(sa, core_Thread_SysCall);
    pd->iface->sigaction(SIGSYS, &sa, NULL);
    AROS_HOST_BARRIER

    /* SIGURG is explicitly sent to the CPU theads to trigger
     * their core_INT
     */
    SETHANDLER(sa, core_Thread_IRQ);
    pd->iface->sigaction(SIGURG, &sa, NULL);
    AROS_HOST_BARRIER

    /* We need to start up with disabled interrupts */
    pd->iface->sigprocmask(SIG_BLOCK, &pd->sig_int_mask, NULL);
    AROS_HOST_BARRIER

    /*
     * Explicitly make sure that SIGUSR1 and SIGUSR2 are enabled.
     * This effectively kicks DalvikVM's ass on Android and takes SIGUSR1 away
     * from its "signal catcher". On other platforms this at least should not harm.
     * I also added SIGUSR2, just in case.
     */
    SIGEMPTYSET(&tmp_mask);
    SIGADDSET(&tmp_mask, SIGUSR1);
    SIGADDSET(&tmp_mask, SIGUSR2);
    pd->iface->sigprocmask(SIG_UNBLOCK, &tmp_mask, NULL);
    AROS_HOST_BARRIER

    pd->irq_enable = TRUE;

    return TRUE;
}

/*
 * All syscalls are mapped to single SIGUSR1. We look at task's tc_State
 * to determine the needed action.
 * This is not inlined because actual SIGUSR1 number is host-specific, it can
 * differ between different UNIX variants, and even between different ports of the same
 * OS (e. g. Linux). We need host OS includes in order to get the proper definition, and
 * we include them only in arch-specific code.
 */
void unix_SysCall(unsigned char n, struct KernelBase *KernelBase)
{
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    ULONG cpu = KrnGetCPUNumber();
    DSC(bug("[KRN] SysCall %d\n", n));

#if AROS_SMP
    if (cpu == 0)
        pd->iface->raise(SIGUSR1);
    else {
        pd->iface->pthread_kill(pd->thread[cpu].tid, SIGSYS);
    }
#else
    pd->iface->raise(SIGUSR1);
#endif

    AROS_HOST_BARRIER
}
