/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxReorderMacro()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxReorderMacro,

/*  SYNOPSIS */
	AROS_LHA(PSZ, FuncName, A0),
	AROS_LHA(ULONG, Position, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 28, Regina)

/*  FUNCTION
        RexxStart function from Rexx SAA library

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

    return RexxReorderMacro(FuncName, Position);

    AROS_LIBFUNC_EXIT
}
