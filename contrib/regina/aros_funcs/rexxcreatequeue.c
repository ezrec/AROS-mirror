/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxCreateQueue()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH4(APIRET, RexxCreateQueue,

/*  SYNOPSIS */
	AROS_LHA(PSZ, Buffer, A0),
	AROS_LHA(ULONG, BuffLen, D0),
	AROS_LHA(PSZ, RequestedName, A1),
	AROS_LHA(ULONG*, DupFlag, A2),

/*  LOCATION */
	struct Library *, ReginaBase, 20, Regina)

/*  FUNCTION
        RexxCreateQueue function from Rexx SAA library

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

    return RexxCreateQueue(Buffer, BuffLen, RequestedName, DupFlag);

    AROS_LIBFUNC_EXIT
}
