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
            KrnScheduling(TRUE);
            pd->irq_enable = TRUE;
            pd->iface->pthread_kill(pd->thread[0].tid, SIGURG);
            AROS_HOST_BARRIER
        }
    }

    AROS_LIBFUNC_EXIT
}
