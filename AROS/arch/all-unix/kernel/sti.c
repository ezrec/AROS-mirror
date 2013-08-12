#include <aros/debug.h>
#include <aros/libcall.h>

#include <signal.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_unix.h"

AROS_LH0(void, KrnSti,
	  struct KernelBase *, KernelBase, 10, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;

    if (1 || !UKB(KernelBase)->SupervisorCount)
    {
        if (pd->iface)
        {
#if AROS_SMP
            unsigned int thiscpu = KrnGetCPUNumber();
            D(bug("CPU%d: STI\n", thiscpu));
            pd->iface->pthread_mutex_lock(&pd->cli_mutex);
            if (pd->cli_thread == -1) {
                /* Nothing to do */
            } else {
                if (pd->cli_thread != thiscpu) {
                    /* Wait for the other CPU to release its Disable */
                    while (pd->cli_thread != -1) {
                        D(bug("CPU%d: STI: Waiting for CPU%d\n", thiscpu, pd->cli_thread));
                        pd->iface->pthread_cond_wait(&pd->cli_cond, &pd->cli_mutex);
                    }
                } else {
                    /* Start all other threads */
                    pd->cli_thread = -1;
                    pd->thread[thiscpu].in_cli = FALSE;
                    pd->iface->pthread_cond_signal(&pd->cli_cond);
                }
            }
            pd->iface->pthread_mutex_unlock(&pd->cli_mutex);
#else
            pd->iface->sigprocmask(SIG_UNBLOCK, &pd->sig_int_mask, NULL);
#endif
            AROS_HOST_BARRIER
        }
    }

    AROS_LIBFUNC_EXIT
}
