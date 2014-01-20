/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(int, CloseSocket,

/*  SYNOPSIS */
        AROS_LHA(int, s, D0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 20, BSDSocket)

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
    struct bsd_fd *fd;
    LONG err;

    D(bug("[bsdsocket] CloseSocket(%u)\n", s));

    fd = BSD_GET_FD(SocketBase, s);

    err = BSD_FD_CALLBACK(SocketBase, s, FDCB_FREE);
    if (err == 0) {
        ASocketDispose(fd->asocket);
        fd->asocket = NULL;
    }

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : err;

    AROS_LIBFUNC_EXIT
} /* CloseSocket */
