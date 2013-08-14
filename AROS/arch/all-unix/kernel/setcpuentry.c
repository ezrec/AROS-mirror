/*
    Copyright © 1995-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_intern.h"

#include <proto/kernel.h>

AROS_LH3(BOOL, KrnSetCPUEntry,
    AROS_LHA(ULONG, cpu, D0),
    AROS_LHA(struct Hook *, hook, A0),
    AROS_LHA(APTR, message, A1),
    struct KernelBase *, KernelBase, 40, Kernel)
{
    AROS_LIBFUNC_INIT

    struct PlatformData *pd = KernelBase->kb_PlatformData;

    if (cpu >= pd->threads)
        return FALSE;

    if (pd->thread[cpu].hook)
        return FALSE;

    /* Order is important here - we need to
     * make sure the message is set before the hook
     */
    pd->thread[cpu].message = message;
    pd->thread[cpu].hook = hook;

    /* Wait for startup acknowlege */
    pd->iface->pthread_mutex_lock(&pd->forbid_mutex);
    while (pd->thread[cpu].hook && pd->thread[cpu].state == STATE_IDLE) {
        pd->iface->pthread_cond_wait(&pd->thread[cpu].state_cond, &pd->forbid_mutex);
    }
    pd->iface->pthread_mutex_unlock(&pd->forbid_mutex);

    return TRUE;

    AROS_LIBFUNC_EXIT
}
