/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH5(int, setsockopt,

/*  SYNOPSIS */
        AROS_LHA(int,    s,       D0),
        AROS_LHA(int,    level,   D1),
        AROS_LHA(int,    optname, D2),
        AROS_LHA(void *, optval,  A0),
        AROS_LHA(int,    optlen,  D3),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 15, BSDSocket)

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

    err = ASocketProtocolSet(as, level, optname, optval, optlen);
    
    BSD_SET_ERRNO(SocketBase, err);
    return err;


    AROS_LIBFUNC_EXIT

} /* setsockopt */
