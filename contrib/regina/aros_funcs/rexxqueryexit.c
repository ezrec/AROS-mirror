/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxQueryExit()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH4(APIRET, RexxQueryExit,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, ExitName, A0),
	AROS_LHA(PCSZ, ModuleName, A1),
	AROS_LHA(PUSHORT, Flag, D0),
	AROS_LHA(PUCHAR, UserArea, A2),

/*  LOCATION */
	struct Library *, ReginaBase, 14, Regina)

/*  FUNCTION
        RexxQueryExit function from Rexx SAA library

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

    return RexxQueryExit(ExitName, ModuleName, Flag, UserArea);

    AROS_LIBFUNC_EXIT
}
