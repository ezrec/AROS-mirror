/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(int, shutdown,

/*  SYNOPSIS */
        AROS_LHA(int, s,   D0),
        AROS_LHA(int, how, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 14, BSDSocket)

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

    APTR as;
    LONG err;

    as = BSD_GET_ASOCKET(SocketBase, s);

    err = ASocketShutdown(as, how);
    
    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT

} /* shutdown */
