/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxDeregisterFunction()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxDeregisterFunction,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, name, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 17, Regina)

/*  FUNCTION
        RexxDeregisterFunction function from Rexx SAA library

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

    return RexxDeregisterFunction(name);

    AROS_LIBFUNC_EXIT

}
