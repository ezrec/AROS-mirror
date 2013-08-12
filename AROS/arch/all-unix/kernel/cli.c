
#include <aros/debug.h>
#include <aros/libcall.h>

#include <signal.h>

#include "kernel_base.h"
#include "kernel_intern.h"

#include <proto/kernel.h>

/* See rom/kernel/cli.c for documentation */

AROS_LH0(void, KrnCli,
    struct KernelBase *, KernelBase, 9, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;

    /* This check is needed for early atomics on ARM <v6, which are Disable()/Enable()-based */
    if (pd->iface)
    {
#if AROS_SMP
        unsigned int thiscpu = KrnGetCPUNumber();
        D(bug("CPU%d: CLI\n", thiscpu));
        pd->iface->pthread_mutex_lock(&pd->cli_mutex);
        pd->thread[thiscpu].in_cli = TRUE;

        if (pd->cli_thread != thiscpu) {
            while (pd->cli_thread != -1) {
                D(bug("CPU%d: CLI: Wait for CPU%d\n", thiscpu, pd->cli_thread));
                pd->iface->pthread_cond_wait(&pd->cli_cond, &pd->cli_mutex);
            }
            D(bug("CPU%d: CLI: Ok\n", thiscpu));
            pd->cli_thread = thiscpu;
        }
        pd->iface->pthread_mutex_unlock(&pd->cli_mutex);
#else
        pd->iface->sigprocmask(SIG_BLOCK, &pd->sig_int_mask, NULL);
#endif
        AROS_HOST_BARRIER
    }

    AROS_LIBFUNC_EXIT
}
