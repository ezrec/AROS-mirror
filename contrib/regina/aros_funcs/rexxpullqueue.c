/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxPullQueue()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH4(APIRET, RexxPullQueue,

/*  SYNOPSIS */
	AROS_LHA(PSZ, QueueName, A0),
	AROS_LHA(PRXSTRING, DataBuf, A1),
	AROS_LHA(PDATETIME, TimeStamp, A2),
	AROS_LHA(ULONG, WaitFlag, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 24, Regina)

/*  FUNCTION
        RexxPullQueue function from Rexx SAA library

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

    return RexxPullQueue(QueueName, DataBuf, TimeStamp, WaitFlag);

    AROS_LIBFUNC_EXIT
}
