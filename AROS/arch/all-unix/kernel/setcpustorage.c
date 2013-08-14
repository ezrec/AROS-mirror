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

AROS_LH1(BOOL, KrnSetCPUStorage,
    AROS_LHA(APTR, value, A0),
    struct KernelBase *, KernelBase, 42, Kernel)
{
    AROS_LIBFUNC_INIT

    bug("CPU%d: Set CPU Storage: %p\n", KrnGetCPUNumber(), value);
    struct PlatformData *pd = KernelBase->kb_PlatformData;

    if (pd->iface && pd->thread) {
        pd->iface->pthread_setspecific(pd->key_storage, value);
        return TRUE;
    } else {
        return FALSE;
    }

    AROS_LIBFUNC_EXIT
}
