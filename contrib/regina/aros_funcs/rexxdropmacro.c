/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxDropMacro()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxDropMacro,

/*  SYNOPSIS */
	AROS_LHA(PSZ, FuncName, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 26, Regina)

/*  FUNCTION
        RexxDropMacro function from Rexx SAA library

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

    return RexxDropMacro(FuncName);

    AROS_LIBFUNC_EXIT
}
