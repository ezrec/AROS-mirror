/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */
        AROS_LH5(LONG, ASocketProtocolGet,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(LONG, level, D0),
        AROS_LHA(LONG, option, D1),
        AROS_LHA(APTR, val_addr, A2),
        AROS_LHA(LONG *, val_len, A3),

/*  LOCATION */
        struct Library *, ASocketBase, 12, ASocket)

/*  FUNCTION
 
        Perform protocol specific operations on the socket.

        Conforms to 'getsockopt(2)' specifications, with the
        exception that it is passed an ASocket handle instead of
        a file descriptor.

    INPUTS

        as       - ASocket handle
        level    - protocol level to use (ie IPPROTO_IP, SOL_SOCKET, etc)
        option   - option id
        val_addr - address of the value to receive
        va_len   - length expected

    RESULT

        0 on success, 'errno' on failue

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return EINVAL;

    AROS_LIBFUNC_EXIT
}

