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

        AROS_LH0(unsigned int, KrnGetCPUCount,

/*  SYNOPSIS */

/*  LOCATION */
	struct KernelBase *, KernelBase, 36, Kernel)

/*  FUNCTION
	Return total number of processors in the system

    INPUTS
	None

    RESULT
	Number of running CPUs in this system

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

#if AROS_SMP
    return KernelBase->kb_CPUCount;
#else
    return 1;
#endif

    AROS_LIBFUNC_EXIT
}
