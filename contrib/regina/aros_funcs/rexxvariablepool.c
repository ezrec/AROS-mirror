/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxVariablePool()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxVariablePool,

/*  SYNOPSIS */
	AROS_LHA(PSHVBLOCK, RequestBlockList, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 10, Regina)

/*  FUNCTION
        RexxVariablePool function from Rexx SAA library

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

    return RexxVariablePool(RequestBlockList);

    AROS_LIBFUNC_EXIT
}
