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

    if (pd->iface)
    {
        KrnScheduling(KSCHED_RESET(SysBase->TDNestCnt));
        pd->irq_enable = TRUE;
    }

    AROS_LIBFUNC_EXIT
}
