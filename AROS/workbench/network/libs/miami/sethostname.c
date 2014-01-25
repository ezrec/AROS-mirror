/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "miami_intern.h"

/*****************************************************************************

    NAME */
        AROS_LH2(int, sethostname,

/*  SYNOPSIS */
        AROS_LHA(const char *, name, A0),
        AROS_LHA(size_t, namelen, D0),

/*  LOCATION */
        struct Library *, MiamiBase, 23, Miami)

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

    return -1;

    AROS_LIBFUNC_EXIT
}

