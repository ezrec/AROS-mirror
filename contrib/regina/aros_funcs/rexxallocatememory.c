/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxAllocateMemory()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(PVOID, RexxAllocateMemory,

/*  SYNOPSIS */
	AROS_LHA(ULONG, Size, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 32, Regina)

/*  FUNCTION
        RexxAllocateMemory function from Rexx SAA library

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

    return RexxAllocateMemory(Size);

    AROS_LIBFUNC_EXIT
}
