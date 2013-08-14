#include <aros/kernel.h>
#include <aros/libcall.h>
#include <exec/execbase.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_unix.h"

AROS_LH0(unsigned int, KrnGetCPUNumber,
	 struct KernelBase *, KernelBase, 37, Kernel)
{
    AROS_LIBFUNC_INIT
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    unsigned int cpu = 0;

    if (pd->iface && pd->thread) {
        const unsigned int *cpu_p;
        cpu_p = pd->iface->pthread_getspecific(pd->key_cpu);
        if (cpu_p)
            cpu = *cpu_p;
    }
   
    return cpu;

    AROS_LIBFUNC_EXIT
}
