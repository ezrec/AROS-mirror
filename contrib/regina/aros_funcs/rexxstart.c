/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function DummyFunction()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH9(APIRET, RexxStart,

/*  SYNOPSIS */
	AROS_LHA(LONG, ArgCount, D0),
	AROS_LHA(PRXSTRING, ArgList, A0),
	AROS_LHA(PCSZ, ProgramName, A1),
	AROS_LHA(PRXSTRING, Instore, A2),
	AROS_LHA(PCSZ, EnvName, A3),
	AROS_LHA(LONG, CallType, D1),
	AROS_LHA(PRXSYSEXIT, Exits, A4),
	AROS_LHA(PSHORT, ReturnCode, A5),
	AROS_LHA(PRXSTRING, Result, D2),

/*  LOCATION */
	struct Library *, ReginaBase, 5, Regina)

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

    return RexxStart(ArgCount, ArgList, ProgramName, Instore, EnvName, CallType, Exits, ReturnCode, Result);

    AROS_LIBFUNC_EXIT

} /* DummyFunction */
