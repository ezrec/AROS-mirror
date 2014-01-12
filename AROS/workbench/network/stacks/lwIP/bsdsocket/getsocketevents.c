/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(LONG, GetSocketEvents,

/*  SYNOPSIS */
        AROS_LHA(ULONG *, eventsp, A0),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 50, BSDSocket)

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

    aros_print_not_implemented ("GetSocketEvents");
/* FIXME: Write BSDSocket/GetSocketEvents */

    return 0;

    AROS_LIBFUNC_EXIT

} /* GetSocketEvents */
