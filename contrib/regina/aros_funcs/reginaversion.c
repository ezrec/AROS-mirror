/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function ReginaVersion()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, ReginaVersion,

/*  SYNOPSIS */
	AROS_LHA(PRXSTRING, VersionString, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 34, Regina)

/*  FUNCTION
        ReginaVersion function from Rexx SAA library

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

    return ReginaVersion(VersionString);

    AROS_LIBFUNC_EXIT
}
