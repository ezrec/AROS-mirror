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
    void *storage = NULL;

    if (pd->iface && pd->thread) {
        storage = pd->iface->pthread_getspecific(pd->key_storage);
    }

    return storage;

    AROS_LIBFUNC_EXIT
}
