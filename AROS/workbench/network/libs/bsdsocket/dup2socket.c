/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(int, Dup2Socket,

/*  SYNOPSIS */
        AROS_LHA(int, fd1, D0),
        AROS_LHA(int, fd2, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 44, BSDSocket)

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
    APTR nas;
    LONG err;

    if (fd1 == fd2) {
        BSD_SET_ERRNO(SocketBase, EINVAL);
        return -1;
    }

    fd = BSD_GET_FD(SocketBase, fd1);

    if (fd2 < 0) {
        fd2 = bsdsocket_fd_avail(SocketBase);
        if (fd2 < 0)
            return -1;
    }

    if (fd2 >= SocketBase->bsd_fds) {
        BSD_SET_ERRNO(SocketBase, EBADF);
        return -1;
    }

    err = ASocketDuplicate(fd->asocket, &nas);
    if (err) {
        BSD_SET_ERRNO(SocketBase, err);
        return -1;
    }

    if (SocketBase->bsd_fd[fd2].asocket != NULL) {
       CloseSocket(fd2);
    }

    err = bsdsocket_fd_init(SocketBase, fd2, nas, fd->flags);
    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT
} /* Dup2Socket */
