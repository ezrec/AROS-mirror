/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "miami_intern.h"

/*****************************************************************************

    NAME */
        AROS_LH1(char *, gai_strerror,

/*  SYNOPSIS */
        AROS_LHA(LONG, error, D0),

/*  LOCATION */
        struct Library *, MiamiBase, 42, Miami)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return "(unknown error)";

    AROS_LIBFUNC_EXIT
}

