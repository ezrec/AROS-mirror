/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxQueryQueue()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(APIRET, RexxQueryQueue,

/*  SYNOPSIS */
	AROS_LHA(PSZ, QueueName, A0),
	AROS_LHA(ULONG*, Count, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 22, Regina)

/*  FUNCTION
        RexxQueryQueue function from Rexx SAA library

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

    return RexxQueryQueue(QueueName, Count);

    AROS_LIBFUNC_EXIT
}
