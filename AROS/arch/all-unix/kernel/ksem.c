/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#include <aros/debug.h>
#include <proto/kernel.h>

#include "kernel_base.h"
#include "kernel_intern.h"

#include "ksem.h"

int _ksem_init(struct KernelBase *KernelBase, struct ksem *ksem)
{
    struct PlatformData *pd = KernelBase->kb_PlatformData;

    pd->iface->pthread_mutex_init(&ksem->mutex, NULL);
    pd->iface->pthread_cond_init(&ksem->cond, NULL);
    
    ksem->owner = -1;
    ksem->depth = 0;

    return 0;
}

int _ksem_obtain(struct KernelBase *KernelBase, struct ksem *ksem)
{
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    ULONG thiscpu = KrnGetCPUNumber();
    
    pd->iface->pthread_mutex_lock(&ksem->mutex);
    /* Wait until we can own it */
    bug("CPU%d: Obtain\n", thiscpu);
    while (ksem->owner >= 0 && ksem->owner != thiscpu) {
        pd->iface->pthread_cond_wait(&ksem->cond, &ksem->mutex);
    }
    ksem->owner = thiscpu;
    bug("CPU%d: Obtain (%d)\n", thiscpu, ksem->depth);
    ksem->depth++;
    pd->iface->pthread_mutex_unlock(&ksem->mutex);

    return 0;
}

int _ksem_release(struct KernelBase *KernelBase, struct ksem *ksem)
{
    struct PlatformData *pd = KernelBase->kb_PlatformData;
    ULONG thiscpu = KrnGetCPUNumber();
    
    pd->iface->pthread_mutex_lock(&ksem->mutex);
    /* Wait - we should be the owner! */
    ASSERT(ksem->owner == thiscpu);
    ksem->depth--;
    bug("CPU%d: Release (%d)\n", thiscpu, ksem->depth);
    if (ksem->depth == 0) {
        ksem->owner = -1;
        bug("CPU%d: Released\n", thiscpu);
        pd->iface->pthread_cond_broadcast(&ksem->cond);
    }
    pd->iface->pthread_mutex_unlock(&ksem->mutex);

    return 0;
}
