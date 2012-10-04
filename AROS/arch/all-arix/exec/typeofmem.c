/*
 * typeofmem.c
 *
 *  Created on: Oct 4, 2012
 *      Author: michal
 */



#include <aros/debug.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <exec/types.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#include "memory.h"
#include "exec_intern.h"

/*****************************************************************************

    NAME */

    AROS_LH1(ULONG, TypeOfMem,

/*  SYNOPSIS */
    AROS_LHA(APTR, address, A1),

/*  LOCATION */
    struct ExecBase *, SysBase, 89, Exec)

/*  FUNCTION
    Return type of memory at a given address or 0 if there is no memory
    there.

    INPUTS
    address - Address to test

    RESULT
    The memory flags you would give to AllocMem().

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    unsigned char vec;

    int ret = PD(SysBase).SysIFace->mincore((APTR)((IPTR)address & ~4095), 1, &vec);

    if (ret != -1)
        return MEMF_CHIP;
    else
        return 0;

    AROS_LIBFUNC_EXIT
} /* TypeOfMem */

