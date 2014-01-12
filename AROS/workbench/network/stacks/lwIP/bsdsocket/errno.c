/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH0(LONG, Errno,

/*  SYNOPSIS */

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 27, BSDSocket)

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

    return bsd_errno(bsdsocketBase->bsd);

    AROS_LIBFUNC_EXIT

} /* */
