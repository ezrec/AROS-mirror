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
    unsigned int i = 0;

    if (pd->iface && pd->thread) {
        pthread_t self;

        self = pd->iface->pthread_self();
        for (i = 0; i < pd->threads; i++) {
            if (pd->thread[i].tid == self)
                break;
        }
        if (i == pd->threads)
            i = 0;
    }
   
    return i;

    AROS_LIBFUNC_EXIT
}
