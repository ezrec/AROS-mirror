/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxAddMacro()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH3(APIRET, RexxAddMacro,

/*  SYNOPSIS */
	AROS_LHA(PSZ, FuncName, A0),
	AROS_LHA(PSZ, SourceFile, A1),
	AROS_LHA(ULONG, Position, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 25, Regina)

/*  FUNCTION
        RexxAddMacro function from Rexx SAA library

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

    return RexxAddMacro(FuncName, SourceFile, Position);

    AROS_LIBFUNC_EXIT
}
