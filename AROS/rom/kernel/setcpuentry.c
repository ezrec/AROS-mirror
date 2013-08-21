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

        AROS_LH3(BOOL, KrnSetCPUEntry,

/*  SYNOPSIS */
                AROS_LHA(ULONG, cpu, D0),
                AROS_LHA(struct Hook *, hook, A0),
                AROS_LHA(APTR, message, A1),

/*  LOCATION */
	struct KernelBase *, KernelBase, 40, Kernel)

/*  FUNCTION
	Set entry point for a CPU

    INPUTS
	hook - Method to call when the CPU has nothing to do
	    When the Hook is called, its 'object' parameter
	    is set to a ULONG * to a ULONG cpu_number
	message - Message parameter to pass to the Hook

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
