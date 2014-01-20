/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/socket.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(int, sendmsg,

/*  SYNOPSIS */
        AROS_LHA(int,                   s,     D0),
        AROS_LHA(const struct msghdr *, msg,   A0),
        AROS_LHA(int,                   flags, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 45, BSDSocket)

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
    struct ASocket_Msg asmsg;
    size_t size;

    as = BSD_GET_ASOCKET(SocketBase, s);

    asmsg.asm_Message.mn_ReplyPort = SocketBase->bsd_MsgPort;
    asmsg.asm_MsgHdr = (APTR)msg;
    err = ASocketSendMsg(as, &asmsg, flags);

    if (err == 0)
        err = bsdsocket_wait_msg(SocketBase, &asmsg, &size);

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? size : -1;

    AROS_LIBFUNC_EXIT

} /* sendmsg */
