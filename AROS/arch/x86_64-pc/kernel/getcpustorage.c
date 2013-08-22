/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/debug.h>
#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "tls.h"

#include <proto/kernel.h>

AROS_LH0(APTR, KrnGetCPUStorage,
    struct KernelBase *, KernelBase, 41, Kernel)
{
    AROS_LIBFUNC_INIT

    void *storage = TLS_GET(CPUStorage);

    return storage;

    AROS_LIBFUNC_EXIT
}
