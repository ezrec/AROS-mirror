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
        pd->irq_enable = TRUE;
        KrnScheduling(KSCHED_RESET(SysBase->TDNestCnt));
    }

    AROS_LIBFUNC_EXIT
}
