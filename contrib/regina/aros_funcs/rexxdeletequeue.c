/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxDeleteQueue()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(APIRET, RexxDeleteQueue,

/*  SYNOPSIS */
	AROS_LHA(PSZ, QueueName, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 21, Regina)

/*  FUNCTION
        RexxDeleteQueue function from Rexx SAA library

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

    return RexxDeleteQueue(QueueName);

    AROS_LIBFUNC_EXIT
}
