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

AROS_LH0(APTR, KrnGetCPUStorage,
    struct KernelBase *, KernelBase, 41, Kernel)
{
    AROS_LIBFUNC_INIT

    int cpu = KrnGetCPUNumber();

    void *storage = NULL;

    void **tls_cpu_storage = TLS_GET(CPUStorage);

    if (tls_cpu_storage)
    {
        storage = tls_cpu_storage[cpu];
    }

    return storage;

    AROS_LIBFUNC_EXIT
}
