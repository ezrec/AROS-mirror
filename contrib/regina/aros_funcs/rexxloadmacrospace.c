/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxLoadMacroSpace()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH3(APIRET, RexxLoadMacroSpace,

/*  SYNOPSIS */
	AROS_LHA(ULONG, FuncCount, D0),
	AROS_LHA(PSZ *, FuncNames, A0),
	AROS_LHA(PSZ, MacroLibFile, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 30, Regina)

/*  FUNCTION
        RexxLoadMacroSpace function from Rexx SAA library

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

    return RexxLoadMacroSpace(FuncCount, FuncNames, MacroLibFile);

    AROS_LIBFUNC_EXIT
}
