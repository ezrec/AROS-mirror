/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/errno.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(int, CloseSocket,

/*  SYNOPSIS */
        AROS_LHA(int, s, D0),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 20, BSDSocket)

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

    D(bug("[bsdsocket] CloseSocket(%u)\n", s));

    return bsd_close(s);

    AROS_LIBFUNC_EXIT
} /* CloseSocket */
