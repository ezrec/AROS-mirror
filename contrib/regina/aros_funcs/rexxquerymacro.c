/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxQueryMacro()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxQueryMacro,

/*  SYNOPSIS */
	AROS_LHA(PSZ, FuncName, A0),
	AROS_LHA(PUSHORT, Position, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 27, Regina)

/*  FUNCTION
        RexxQueryMacro function from Rexx SAA library

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

    return RexxQueryMacro(FuncName, Position);

    AROS_LIBFUNC_EXIT
}
