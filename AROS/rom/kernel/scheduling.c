/*
    Copyright © 1995-2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
*/

#include <aros/kernel.h>
#include <aros/libcall.h>

#include <kernel_base.h>

/*****************************************************************************

    NAME */
#include <proto/kernel.h>

        AROS_LH1(BYTE, KrnScheduling,
            AROS_LHA(LONG, trigger, D0),

/*  SYNOPSIS */

/*  LOCATION */
    struct KernelBase *, KernelBase, 43, Kernel)

/*  FUNCTION
    Alter kernel task switching

    INPUTS
    None

    RESULT

    NOTES
    This does nothing on uni-processor systems

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* The actual implementation can be entirely architecture-specific */
    switch (trigger) {
    case KSCHED_INPECT: break;
    case KSCHED_FORBID: AROS_ATOMIC_INC(SysBase->TDNestCnt); break;
    case KSCHED_PERMIT: AROS_ATOMIC_DEC(SysBase->TDNestCnt); break;
    default:            SysBase->TDNestCnt = (BYTE)trigger;

    return SysBase->TDNestCnt;

    AROS_LIBFUNC_EXIT
}
