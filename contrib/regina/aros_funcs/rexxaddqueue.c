/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxAddQueue()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH3(APIRET, RexxAddQueue,

/*  SYNOPSIS */
	AROS_LHA(PSZ, QueueName, A0),
	AROS_LHA(PRXSTRING, EntryData, A1),
	AROS_LHA(ULONG, AddFlag, D0),

/*  LOCATION */
	struct Library *, ReginaBase, 23, Regina)

/*  FUNCTION
        RexxAddQueue function from Rexx SAA library

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

    return RexxAddQueue(QueueName, EntryData, AddFlag);

    AROS_LIBFUNC_EXIT
}
