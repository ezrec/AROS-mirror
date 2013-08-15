
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
        pd->irq_enable = FALSE;
        KrnScheduling(KSCHED_RESET(0));
        pd->iface->pthread_kill(pd->thread[0].tid, SIGURG);
        AROS_HOST_BARRIER
    }

    AROS_LIBFUNC_EXIT
}
