/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
        AROS_LH3(LONG, ASocketRecvMsg,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(struct ASocket_Msg *, msg, A1),
        AROS_LHA(int, flags, D0),

/*  LOCATION */
        struct Library *, ASocketBase, 15, ASocket)

/*  FUNCTION
 
        Receive a message from a socket, asynchronously.

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

    if (msg == NULL)
        return EFAULT;

    msg->asm_Message.mn_Node.ln_Type = NT_AS_MSG;
    ReplyMsg(&msg->asm_Message);

    return 0;

    AROS_LIBFUNC_EXIT
}

