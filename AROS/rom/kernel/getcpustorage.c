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

        AROS_LH0(APTR, KrnGetCPUStorage,

/*  SYNOPSIS */

/*  LOCATION */
	struct KernelBase *, KernelBase, 41, Kernel)

/*  FUNCTION
    Return this CPU's local storage

    INPUTS
	None

    RESULT
    Local storage for this CPU

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* The actual implementation is entirely architecture-specific */
    return NULL;

    AROS_LIBFUNC_EXIT
}
