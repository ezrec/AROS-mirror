/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxFreeMemory()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxFreeMemory,

/*  SYNOPSIS */
	AROS_LHA(PVOID, MemoryBlock, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 33, Regina)

/*  FUNCTION
        RexxFreeMemory function from Rexx SAA library

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,ReginaBase)

    return RexxFreeMemory(MemoryBlock);

    AROS_LIBFUNC_EXIT
}
