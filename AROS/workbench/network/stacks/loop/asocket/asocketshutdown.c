/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
        AROS_LH2(LONG, ASocketShutdown,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(int, how, D1),

/*  LOCATION */
        struct Library *, ASocketBase, 17, ASocket)

/*  FUNCTION
 
       Shut down recv/send capability of an ASocket handle

    INPUTS

       as       - ASocket handle
       how      - One of the following:

            SHUT_RD     - Stop all reception
            SHUT_WR     - Stop all transmission
            SHUT_RDWR   - Stop all communication

    RESULT

        0 on success, or 'errno' on failure.

    NOTES

        If there are any pending ASocketSendMsg() or ASocketRecvMsg()
        transactions, they will be immediately sent to their mn_ReplyPort
        with asm_Errno set to EINTR.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}

