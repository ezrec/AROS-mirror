/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxRegisterFunctionDll()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH3(APIRET, RexxRegisterFunctionDll,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, ExternalName, A0),
	AROS_LHA(PCSZ, LibraryName, A1),
	AROS_LHA(PCSZ, InternalName, A2),

/*  LOCATION */
	struct Library *, ReginaBase, 16, Regina)

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

    return RexxRegisterFunctionDll(ExternalName, LibraryName, InternalName);

    AROS_LIBFUNC_EXIT

}
