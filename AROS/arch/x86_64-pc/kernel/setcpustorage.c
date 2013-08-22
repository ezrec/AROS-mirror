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

AROS_LH1(BOOL, KrnSetCPUStorage,
    AROS_LHA(APTR, value, A0),
    struct KernelBase *, KernelBase, 42, Kernel)
{
    AROS_LIBFUNC_INIT

    bug("CPU%d: Set CPU Storage: %p\n", KrnGetCPUNumber(), value);

    TLS_SET(CPUStorage, value);

    AROS_LIBFUNC_EXIT
}
