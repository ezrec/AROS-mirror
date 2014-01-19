/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <proto/exec.h>
#include <sys/errno.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(int, socket,

/*  SYNOPSIS */
        AROS_LHA(int, domain,   D0),
        AROS_LHA(int, type,     D1),
        AROS_LHA(int, protocol, D2),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 5, BSDSocket)

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

    int ns;
    LONG err = EMFILE;

    /* Find a free slot */
    for (ns = 0; ns < SocketBase->bsd_fds; ns++) {
        if (SocketBase->bsd_fd[ns].asocket == NULL) {
            APTR as;
            err = ASocketNew(&as,
                             AS_TAG_SOCKET_DOMAIN, domain,
                             AS_TAG_SOCKET_TYPE, type,
                             AS_TAG_SOCKET_PROTOCOL, protocol,
                             TAG_END);
            if (err == 0)
                err = bsdsocket_fd_init(SocketBase, &SocketBase->bsd_fd[ns], as, O_RDWR);
            break;
        }
    }

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? ns : err;

    AROS_LIBFUNC_EXIT

} /* socket */
