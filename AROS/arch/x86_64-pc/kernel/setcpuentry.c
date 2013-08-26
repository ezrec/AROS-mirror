/*
    Copyright © 1995-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/kernel.h>
#include <aros/libcall.h>

#include "kernel_base.h"
#include "kernel_intern.h"
#include "kernel_debug.h"
#include "tls.h"
#include "apic.h"

#include <proto/kernel.h>
#include <proto/exec.h>

#define D(x) x

AROS_LH3(BOOL, KrnSetCPUEntry,
    AROS_LHA(ULONG, cpu, D0),
    AROS_LHA(struct Hook *, hook, A0),
    AROS_LHA(APTR, message, A1),
    struct KernelBase *, KernelBase, 40, Kernel)
{
    AROS_LIBFUNC_INIT

    BOOL retval = FALSE;

    D(bug("[Kernel] KrnSetCPUEntry(CPU%d, %p, %p)\n", cpu, hook, message));

    volatile mbox_t *mbox = &TLS_GET(MessageBoxes)[cpu];

    spinlock_acquire(&mbox->inbox_lock);
    mbox->message = message;
    mbox->hook = hook;
    spinlock_release(&mbox->inbox_lock);

    APTR stack = SuperState();

    core_APIC_DoIPI(KernelBase->kb_PlatformData->kb_APIC->lapicBase, cpu, (1 << 14) | 0xfd);

    UserState(stack);

    return retval;

    AROS_LIBFUNC_EXIT
}
