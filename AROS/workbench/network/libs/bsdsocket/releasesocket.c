/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(LONG, ReleaseSocket,

/*  SYNOPSIS */
        AROS_LHA(LONG, sd, D0),
        AROS_LHA(LONG, id, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 25, BSDSocket)

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

    struct bsd_fd *fd, *fdnode;
    struct BSDSocketBase *BSDSocketBase = SocketBase->lib_BSDSocketBase;

    fd = BSD_GET_FD(SocketBase, sd);

    fdnode = AllocVec(sizeof(*fdnode), MEMF_ANY);
    if (fdnode == NULL) {
        BSD_SET_ERRNO(SocketBase, ENOMEM);
        return -1;
    }

    CopyMem(fd, fdnode, sizeof(*fd));
    fd->asocket = NULL;

    ObtainSemaphore(&BSDSocketBase->bs_Lock);
    ADDTAIL(&BSDSocketBase->bs_FDList, fdnode);
    ReleaseSemaphore(&BSDSocketBase->bs_Lock);

    return 0;

    AROS_LIBFUNC_EXIT

} /* ReleaseSocket */
