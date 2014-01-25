/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "miami_intern.h"

/*****************************************************************************

    NAME */
        AROS_LH7(int, MiamiSysCtl,

/*  SYNOPSIS */
        AROS_LHA(LONG *, name, A0),
        AROS_LHA(ULONG, namelen, D0),
        AROS_LHA(void *, oldp, A1),
        AROS_LHA(LONG *, oldlenp, A2),
        AROS_LHA(void *, newp, A3),
        AROS_LHA(LONG, newlen, D1),
        AROS_LHA(int, len, D2),

/*  LOCATION */
        struct Library *, MiamiBase, 5, Miami)

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

