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
        struct bsdsocketBase *, SocketBase, 50, BSDSocket)

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

    int s;

    if (eventsp == NULL) {
        BSD_SET_ERRNO(SocketBase, EFAULT);
        return -1;
    }

    for (s = 0; s < SocketBase->bsd_fds; s++) {
        APTR as;
        LONG err;
        ULONG active;

        if (SocketBase->bsd_fd[s].asocket == NULL)
            continue;
        
        as = SocketBase->bsd_fd[s].asocket;

        err = ASocketGet(as, AS_TAG_NOTIFY_FD_ACTIVE, &active, TAG_END);
        if (err != 0) {
            BSD_SET_ERRNO(SocketBase, err);
            return -1;
        }

        if (active) {
            *eventsp = active;
            return s;
        }
    }

    /* No active FDs found */
    BSD_SET_ERRNO(SocketBase, 0);
    return  -1;

    AROS_LIBFUNC_EXIT

} /* GetSocketEvents */
