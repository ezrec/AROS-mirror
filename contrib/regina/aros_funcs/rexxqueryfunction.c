/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxQueryFunction()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxQueryFunction,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, name, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 18, Regina)

/*  FUNCTION
        RexxQueryFunction function from Rexx SAA library

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

    return RexxQueryFunction(name);

    AROS_LIBFUNC_EXIT
}
