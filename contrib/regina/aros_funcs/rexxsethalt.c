/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxSetHalt()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxSetHalt,

/*  SYNOPSIS */
	AROS_LHA(LONG, pid, D0),
	AROS_LHA(LONG, tid, D1),

/*  LOCATION */
	struct Library *, ReginaBase, 19, Regina)

/*  FUNCTION
        RexxSetHalt function from Rexx SAA library

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

    return RexxSetHalt(pid, tid);

    AROS_LIBFUNC_EXIT
}
