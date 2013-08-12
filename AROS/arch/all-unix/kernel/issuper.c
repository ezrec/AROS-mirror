#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_unix.h"
#include "kernel_intern.h"

AROS_LH0I(int, KrnIsSuper,
	  struct KernelBase *, KernelBase, 13, Kernel)
{
    AROS_LIBFUNC_INIT

    if (!KernelBase->kb_PlatformData->iface)
        return FALSE;

#if AROS_SMP
    if (!KernelBase->kb_PlatformData->thread)
        return FALSE;
#endif

    return UKB(KernelBase)->SupervisorCount;

    AROS_LIBFUNC_EXIT
}
