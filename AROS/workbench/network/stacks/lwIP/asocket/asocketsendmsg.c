/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH3(LONG, ASocketSendMsg,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(struct ASocket_Msg *, msg, A1),
        AROS_LHA(int, flags, D0),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 14, ASocket)

/*  FUNCTION

        Send a message to a socket, asynchronously.

        This function returns immediately.

        Once the IO is complete, the message will be sent
        to the msg->asm_Message.mn_ReplyPort with a node
        type of NT_AS_MSG_SEND

    INPUTS

        as      - ASocket handle
        msg     - struct ASocket_Msg that will be processed
        flags   - MSG_* flags - see <sys/socket.h>

    RESULT

        0 on success, 'errno' on failure

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    int err;

    if (msg == NULL)
        return EFAULT;

    err = bsd_sendmsg(ASocketBase->ab_bsd, s, msg->asm_MsgHdr, flags);
    if (err)
        return err;

    msg->asm_Message.mn_Node.ln_Type = NT_AS_MSG_SEND;
    ReplyMsg(&msg->asm_Message);

    return 0;

    AROS_LIBFUNC_EXIT
}

