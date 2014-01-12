/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH0(int, getdtablesize,

/*  SYNOPSIS */

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 23, BSDSocket)

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

    return bsd_getdtablesize(bsdsocketBase->bsd);

    AROS_LIBFUNC_EXIT

} /* getdtablesize */
