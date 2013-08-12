/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/debug.h>
#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_unix.h"

#include <proto/kernel.h>

AROS_LH0(APTR, KrnGetCPUStorage,
    struct KernelBase *, KernelBase, 41, Kernel)
{
    AROS_LIBFUNC_INIT
    struct PlatformData *pd = KernelBase->kb_PlatformData;

    if (pd->iface && pd->thread) {
        return pd->thread[KrnGetCPUNumber()].storage;
    } else {
        bug("CPUx: Can't CPU Storage, hostlib interface not set up yet\n");
        return NULL;
    }

    AROS_LIBFUNC_EXIT
}
