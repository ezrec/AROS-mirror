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

        AROS_LH1(void, KrnScheduling,
            AROS_LHA(BOOL, enabled, D0),

/*  SYNOPSIS */

/*  LOCATION */
    struct KernelBase *, KernelBase, 43, Kernel)

/*  FUNCTION
    Enable or disable kernel task switching

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

    /* The actual implementation is entirely architecture-specific */
    return;

    AROS_LIBFUNC_EXIT
}
