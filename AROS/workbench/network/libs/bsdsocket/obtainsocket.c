/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH4(LONG, ObtainSocket,

/*  SYNOPSIS */
        AROS_LHA(LONG, id,       D0),
        AROS_LHA(LONG, domain,   D1),
        AROS_LHA(LONG, type,     D2),
        AROS_LHA(LONG, protocol, D3),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 24, BSDSocket)

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
    struct BSDSocketBase *BSDSocketBase = SocketBase->lib_BSDSocketBase;
    struct bsd_fd *fd;
    struct bsd_fd *fdnode;


    /* See if we have space for a new socket */
    ns = bsdsocket_fd_avail(SocketBase);
    if (ns < 0)
        return -1;

    fd = &SocketBase->bsd_fd[ns];

    ObtainSemaphore(&BSDSocketBase->bs_Lock);
    ForeachNode(&BSDSocketBase->bs_FDList, fdnode) {
        if (fdnode->id != id)
            continue;

        if (domain != 0) {
            LONG ad = 0, ap = 0, at = 0;
            APTR as = fdnode->asocket;

            ASocketGet(as, AS_TAG_SOCKET_DOMAIN, &ad,
                           AS_TAG_SOCKET_TYPE, &at,
                           AS_TAG_SOCKET_PROTOCOL, &ap, TAG_END);
            if (domain != ad && type != at && protocol != ap)
                continue;
        }

        REMOVE(fdnode);
        ReleaseSemaphore(&BSDSocketBase->bs_Lock);

        CopyMem(fdnode, fd, sizeof(*fd));
        FreeVec(fdnode);
        return ns;
    }
    ReleaseSemaphore(&BSDSocketBase->bs_Lock);

    BSD_SET_ERRNO(SocketBase, EAGAIN);
    return -1;

    AROS_LIBFUNC_EXIT

} /* ObtainSocket */
