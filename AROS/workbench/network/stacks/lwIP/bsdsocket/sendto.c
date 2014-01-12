/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <proto/exec.h>

#include <sys/errno.h>
#include <sys/socket.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH6(int, sendto,

/*  SYNOPSIS */
        AROS_LHA(int,                     s,     D0),
        AROS_LHA(const void *,            msg,   A0),
        AROS_LHA(int,                     len,   D1),
        AROS_LHA(int,                     flags, D2),
        AROS_LHA(const struct sockaddr *, to,    A1),
        AROS_LHA(int,                     tolen, D3),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 10, BSDSocket)

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

    return bsd_sendto(bsdsocketBase->bsd, s, msg, len, flags, to, tolen);

    AROS_LIBFUNC_EXIT

} /* sendto */
