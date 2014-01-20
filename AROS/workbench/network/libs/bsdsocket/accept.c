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
        struct bsdsocketBase *, SocketBase, 8, BSDSocket)

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

    APTR  nas = NULL;
    int err, ns;
    struct bsd_fd *fd;

    fd = BSD_GET_FD(SocketBase, s);

    /* Wait for ASocket to have an accept() */
    err = bsdsocket_wait_event(SocketBase, fd->asocket, FD_ACCEPT);
    if (err < 0) {
        BSD_SET_ERRNO(SocketBase, err);
        return -1;
    }

    /* Ok, we have an FD_ACCEPT waiting - allocate a free socket */
    ns = bsdsocket_fd_avail(SocketBase);
    if (ns < 0)
        return -1;

    err = ASocketAccept(fd->asocket, &nas);
    if (err == 0) {
        if (addrlen && addr) {
            struct ASocket_Address as_addr;
            as_addr.asa_Length = *addrlen;
            as_addr.asa_Address = addr;
            err = ASocketGet(nas, AS_TAG_SOCKET_ENDPOINT, &as_addr, TAG_END);
            /* We're ignoring any error for now... */
            (void)err;
            *addrlen = as_addr.asa_Length;
        }
        bsdsocket_fd_init(SocketBase, ns, nas, O_RDWR);
        return ns;
    }

    BSD_SET_ERRNO(SocketBase, err);
    return -1;

    AROS_LIBFUNC_EXIT

} /* accept */
