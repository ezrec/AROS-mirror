/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */
#include <aros/debug.h>
#include <aros/kernel.h>
#include <exec/execbase.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_globals.h"
#include "kernel_unix.h"

#if AROS_SMP
static void *smp_entry(void *threadp)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KrnUnixThread *thread = threadp;
    ULONG cpu;
    IPTR res;
    sigset_t newset;

    /* Determine our CPU
     */
    cpu = thread - &pd->thread[0];
    bug("CPU%d: Kernel IPI\n", cpu);

    /* SIGSYS and SIGURG for threads */
    SIGFILLSET(&newset);
    SIGDELSET(&newset, SIGSYS);     /* SysCall-stype */
    SIGDELSET(&newset, SIGURG);     /* IRQ-style */
    SIGDELSET(&newset, SIGSTOP);    /* Disable() */
    SIGDELSET(&newset, SIGCONT);    /* Enable() */
    pd->iface->sigprocmask(SIG_SETMASK, &newset, NULL);
   
    do {
        while (thread->hook == NULL);
        res = CALLHOOKPKT(thread->hook, &cpu, thread->message);
        thread->hook = NULL;
    } while (res == 0);

    return (void *)res;
}

/* Unix __thread implementations (at least on Linux) require a
 * malloc() and abort().
 *
 * Provide them.
 */
void *malloc(size_t size)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KernelInterface *iface = pd->iface;

    return iface->malloc(size);
}

__attribute__((__noreturn__)) void abort(void)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KernelInterface *iface = pd->iface;

    iface->abort();
    for (;;);
}
#endif

int smp_Start(void)
{
#if AROS_SMP
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KernelInterface *iface = pd->iface;
    sigset_t oldset, newset;
    ULONG cpu;
    ULONG cpus;
    
    cpus = LibGetTagData(KRN_MaxCPUs, 0, BootMsg);
    if (cpus == 0)
        cpus = 1;

    /* Initialize the boot CPU's per-thread data */
    pd->threads = cpus;
    pd->thread = AllocMem(sizeof(struct KrnUnixThread)*cpus, MEMF_ANY | MEMF_CLEAR);

    /* Block all signals..
     * NOTE: For SMP, sigprocmask is actually pointing to pthread_sigmask,
     *       as required for pthread support
     */
    iface->sigprocmask(0, NULL, &oldset);

    SIGFILLSET(&newset);

    iface->sigprocmask(SIG_SETMASK, &newset, NULL);

    pd->iface->pthread_mutex_init(&pd->cli_mutex, NULL);
    pd->iface->pthread_cond_init(&pd->cli_cond, NULL);

    /* Start all new threads - they will wait for their KrnUnixTLS->CPUId to
     * be non-zero
     */
    pd->thread[0].tid = iface->pthread_self();
    for (cpu = 1; cpu < pd->threads; cpu++) {
        int err;
#ifdef HOST_OS_linux
#define O_NONBLOCK 04000
#endif
        pd->iface->pipe2(pd->thread[cpu].signal, O_NONBLOCK );
        err = iface->pthread_create(&pd->thread[cpu].tid, NULL, smp_entry, &pd->thread[cpu]);
        if (err) {
            D(bug("CPU%d: Could not start\n", cpu));
        }
    }
    
    /* Restore the signal set for the boot thread */
    iface->sigprocmask(SIG_SETMASK, &oldset, NULL);
#endif
    return 0;
}


