/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxQuerySubcom()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH4(APIRET, RexxQuerySubcom,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(PCSZ, ModuleName, A1),
	AROS_LHA(PUSHORT, Flag, A2),
	AROS_LHA(PUCHAR, UserArea, A3),

/*  LOCATION */
	struct Library *, ReginaBase, 8, Regina)

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

    return RexxQuerySubcom(EnvName, ModuleName, Flag, UserArea);

    AROS_LIBFUNC_EXIT

}
