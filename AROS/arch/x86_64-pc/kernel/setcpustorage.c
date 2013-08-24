/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_debug.h"
#include "kernel_base.h"
#include "kernel_intern.h"
#include "tls.h"

#include <proto/kernel.h>
#include <proto/exec.h>

AROS_LH1(BOOL, KrnSetCPUStorage,
    AROS_LHA(APTR, value, A0),
    struct KernelBase *, KernelBase, 42, Kernel)
{
    AROS_LIBFUNC_INIT

    int cpu = KrnGetCPUNumber();
    void **cpu_storage = TLS_GET(CPUStorage);

    if (cpu_storage != NULL)
    {
        cpu_storage[cpu] = value;

        return TRUE;
    }
    else
        return FALSE;

    AROS_LIBFUNC_EXIT
}
