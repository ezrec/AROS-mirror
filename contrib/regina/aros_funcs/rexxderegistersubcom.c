/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxDeregisterSubcom()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxDeregisterSubcom,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(PCSZ, ModuleName, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 9, Regina)

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

    return RexxDeregisterSubcom(EnvName, ModuleName);

    AROS_LIBFUNC_EXIT

}
