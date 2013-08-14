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

static void *smp_entry(void *threadp)
{
    struct KernelBase *KernelBase = getKernelBase();
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    struct KrnUnixThread *thread = threadp;
    unsigned int cpu;
    IPTR res;
    sigset_t newset;

    /* Determine our CPU
     */
    cpu = thread - &pd->thread[0];
    bug("CPU%d: Kernel IPI\n", cpu);
    pd->iface->pthread_setspecific(pd->key_cpu, &cpu);

    /* SIGSYS and SIGURG for threads */
    SIGFILLSET(&newset);
    SIGDELSET(&newset, SIGSYS);     /* SysCall-stype */
    SIGDELSET(&newset, SIGURG);     /* IRQ-style */
    SIGDELSET(&newset, SIGTSTP);    /* Disable() */
    SIGDELSET(&newset, SIGCONT);    /* Enable() */
    pd->iface->sigprocmask(SIG_SETMASK, &newset, NULL);

    do {
        /* Acknowledge that we are ready to go */
        pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
        thread->state = STATE_IDLE;
        pd->iface->pthread_cond_broadcast(&thread->state_cond);
        pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);

        bug("CPU%d: Idle\n", cpu);
        while (thread->hook == NULL);

        /* Notify that we are running */
        pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
        thread->state = STATE_RUNNING;
        pd->iface->pthread_cond_broadcast(&thread->state_cond);
        pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);

        bug("CPU%d: Running hook %p\n", cpu, thread->hook);
        res = CALLHOOKPKT(thread->hook, &cpu, thread->message);

        thread->hook = NULL;
    } while (res == 0);

    return (void *)res;
}

int smp_Start(void)
{
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

    pd->forbid_cpu = -1;
    pd->iface->pthread_mutex_init(&pd->forbid_mutex, NULL);

    pd->iface->pthread_key_create(&pd->key_cpu, NULL);
    pd->iface->pthread_key_create(&pd->key_storage, NULL);

    SIGEMPTYSET(&newset);
    SIGADDSET(&newset, SIGCONT);

    /* Start all new threads - they will wait for their KrnUnixTLS->CPUId to
     * be non-zero
     */
    pd->thread[0].tid = iface->pthread_self();
    for (cpu = 1; cpu < pd->threads; cpu++) {
        int err;
        struct KrnUnixThread *thread = &pd->thread[cpu];
#ifdef HOST_OS_linux
#define O_NONBLOCK 04000
#endif
        thread->state = STATE_STOPPED;
        pd->iface->pthread_cond_init(&thread->state_cond, NULL);
        pd->iface->pipe2(thread->signal, O_NONBLOCK );
        err = iface->pthread_create(&thread->tid, NULL, smp_entry, &pd->thread[cpu]);
        if (err) {
            D(bug("CPU%d: Could not start\n", cpu));
        }
        /* Wait for startup acknowlege */
        pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
        while (thread->state != STATE_IDLE) {
            pd->iface->pthread_cond_wait(&thread->state_cond, &pd->forbid_mutex);
        }
        pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);
    }
    
    /* Restore the signal set for the boot thread */
    iface->sigprocmask(SIG_SETMASK, &oldset, NULL);
    return 0;
}


