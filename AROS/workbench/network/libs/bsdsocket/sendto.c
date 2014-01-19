/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <proto/exec.h>

#include <sys/errno.h>
#include <sys/socket.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH6(int, sendto,

/*  SYNOPSIS */
        AROS_LHA(int,                     s,     D0),
        AROS_LHA(const void *,            msg,   A0),
        AROS_LHA(int,                     len,   D1),
        AROS_LHA(int,                     flags, D2),
        AROS_LHA(const struct sockaddr *, to,    A1),
        AROS_LHA(int,                     tolen, D3),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 10, BSDSocket)

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
    size_t size;

    as = BSD_GET_ASOCKET(SocketBase, s);

    if (len > 0) {
        struct ASocket_Msg asmsg;
        struct msghdr msghdr;
        struct iovec iovec[1];

        if (msg == NULL) {
            BSD_SET_ERRNO(SocketBase, EFAULT);
            return -1;
        }

        if (to == NULL && tolen > 0) {
            BSD_SET_ERRNO(SocketBase, EFAULT);
            return -1;
        }

        iovec[0].iov_base = (APTR)msg;
        iovec[0].iov_len  = len;

        msghdr.msg_name = (APTR)to;
        msghdr.msg_namelen = tolen;
        msghdr.msg_iov = &iovec[0];
        msghdr.msg_iovlen = 1;
        msghdr.msg_control = NULL;
        msghdr.msg_controllen = 0;
        msghdr.msg_flags = 0;

        asmsg.asm_Message.mn_ReplyPort = SocketBase->bsd_MsgPort;
        asmsg.asm_MsgHdr = &msghdr;

        err = ASocketSendMsg(as, &asmsg, flags);
        if (err == 0)
            size = bsdsocket_wait_msg(SocketBase, &asmsg);
    } else {
        size = 0;
        err = 0;
    }

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? size : -1;

    AROS_LIBFUNC_EXIT

} /* sendto */
