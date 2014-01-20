/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/ioctl.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(int, IoctlSocket,

/*  SYNOPSIS */
        AROS_LHA(int,           s,       D0),
        AROS_LHA(unsigned long, request, D1),
        AROS_LHA(char *,        argp,    A0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 19, BSDSocket)

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
    LONG err = 0;
    ULONG tmp;

    D(bug("%s: s=%d, request=%d, argp=%p\n", __func__, s, request, argp));

    fd = BSD_GET_FD(SocketBase, s);

    switch (request) {
    case FIOASYNC:
        tmp = fd->flags;
        switch (*(long *)argp) {
        case 0: fd->flags &= ~O_ASYNC; break;
        case 1: fd->flags |=  O_ASYNC; break;
        default: err = EINVAL; break;
        }
        if (tmp != fd->flags) {
            if (fd->flags & O_ASYNC) {
                /* Fail if no owner for sigio */
                if (fd->sigioTask == NULL) {
                    fd->flags = tmp;
                    err = EINVAL;
                    break;
                }
                /* Put in async mode - use the global async monitor task */
                err = ASocketSet(fd->asocket,
                                 AS_TAG_NOTIFY_MSGPORT, SocketBase->lib_BSDSocketBase->bs_MsgPort,
                                 AS_TAG_NOTIFY_FD_MASK, fd->fd_mask,
                                 AS_TAG_NOTIFY_NAME, fd,
                                 TAG_END);
            } else {
                /* Stop async mode */
                err = ASocketSet(fd->asocket, 
                                 AS_TAG_NOTIFY_MSGPORT, NULL,
                                 AS_TAG_NOTIFY_FD_MASK, 0,
                                 AS_TAG_NOTIFY_NAME, NULL,
                                 TAG_END);
            }
            if (err != 0)
                fd->flags = tmp;
        }
        break;
    case FIOCLEX:
    case FIONCLEX:
        /* Valid, but ignored */
        break;
    case FIOGETOWN:
        *(struct Task **)argp = fd->sigioTask;
        break;
    case SIOCGPGRP:
        *(struct Task **)argp = fd->sigurgTask;
        break;
    case FIONBIO:
        switch (*(long *)argp) {
        case 0: fd->flags &= ~O_NONBLOCK; break;
        case 1: fd->flags |=  O_NONBLOCK; break;
        default: err = EINVAL; break;
        }
        break;
    case FIONREAD:
        err = ASocketGet(fd->asocket, AS_TAG_STATUS_READABLE, &tmp, TAG_END);
        if (err == 0) {
            *(long *)argp = (long)tmp;
        }
        break;
    case FIOSETOWN:
        fd->sigioTask = (struct Task *)argp;
        break;
    case SIOCSPGRP:
        fd->sigurgTask = (struct Task *)argp;
        break;
    default:
        err = EINVAL;
        break;
    }

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT

} /* IoctlSocket */
