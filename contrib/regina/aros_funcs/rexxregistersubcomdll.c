/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function DummyFunction()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH5(APIRET, RexxRegisterSubcomDll,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(PCSZ, ModuleName, A1),
	AROS_LHA(PCSZ, ProcedureName, A2),
	AROS_LHA(PUCHAR, UserArea, A3),
	AROS_LHA(ULONG, DropAuth, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 7, Regina)

/*  FUNCTION
        RexxRegisterSubcomDll function from Rexx SAA library

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

    return RexxRegisterSubcomDll(EnvName, ModuleName, ProcedureName, UserArea, DropAuth);

    AROS_LIBFUNC_EXIT

}
