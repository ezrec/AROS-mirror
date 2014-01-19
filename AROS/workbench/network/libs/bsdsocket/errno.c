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
        struct bsdsocketBase *, SocketBase, 27, BSDSocket)

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

    return SocketBase->bsd_errno.value;

    AROS_LIBFUNC_EXIT

} /* */
