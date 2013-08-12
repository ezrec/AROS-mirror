#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_intern.h"

AROS_LH0(unsigned int, KrnGetCPUCount,
	 struct KernelBase *, KernelBase, 36, Kernel)
{
    AROS_LIBFUNC_INIT
    struct PlatformData *pdata = KernelBase->kb_PlatformData;

    return pdata->threads;

    AROS_LIBFUNC_EXIT
}
