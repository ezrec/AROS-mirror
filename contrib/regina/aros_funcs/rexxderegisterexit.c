/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxDeregisterExit()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxDeregisterExit,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(PCSZ, ModuleName, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 13, Regina)

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

    return RexxDeregisterExit(EnvName, ModuleName);

    AROS_LIBFUNC_EXIT
}
