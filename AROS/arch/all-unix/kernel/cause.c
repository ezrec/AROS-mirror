#include <aros/libcall.h>

#include <signal.h>

#include "kernel_base.h"
#include "kernel_intern.h"

AROS_LH0(void, KrnCause,
	  struct KernelBase *, KernelBase, 3, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;

#if AROS_SMP
    pd->iface->pthread_kill(pd->thread[0].tid, SIGUSR2);
    AROS_HOST_BARRIER
#else
    pd->iface->raise(SIGUSR2);
    AROS_HOST_BARRIER
#endif

    AROS_LIBFUNC_EXIT
}
