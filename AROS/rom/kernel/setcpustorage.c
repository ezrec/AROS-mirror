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

        AROS_LH0(BOOL, KrnSetCPUStorage,

/*  SYNOPSIS */
                AROS_LHA(APTR, value, A0)

/*  LOCATION */
	struct KernelBase *, KernelBase, 42, Kernel)

/*  FUNCTION
	Set the local storage of this CPU

    INPUTS
	Value to set for the CPU's local storage

    RESULT
    TRUE on success, FALSE on failure

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* The actual implementation is entirely architecture-specific */
    return FALSE;

    AROS_LIBFUNC_EXIT
}
