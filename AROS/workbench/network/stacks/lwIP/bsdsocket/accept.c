/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(int, accept,

/*  SYNOPSIS */
        AROS_LHA(int,               s,       D0),
        AROS_LHA(struct sockaddr *, addr,    A0),
        AROS_LHA(int *,             addrlen, A1),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 8, BSDSocket)

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

    return bsd_accept(bsdsocketBase->bsd, s, addr, addrlen);

    AROS_LIBFUNC_EXIT

} /* accept */
