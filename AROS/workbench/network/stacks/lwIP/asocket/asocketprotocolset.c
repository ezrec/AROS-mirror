/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH5(LONG, ASocketProtocolSet,

/*  SYNOPSIS */
        AROS_LHA(APTR, s, A0),
        AROS_LHA(LONG, level, D0),
        AROS_LHA(LONG, option, D1),
        AROS_LHA(CONST_APTR, val_addr, A2),
        AROS_LHA(LONG, val_len, D2),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 13, ASocket)

/*  FUNCTION

        Perform protocol specific operations on the socket.

        Conforms to 'setsockopt(2)' specifications, with the
        exception that it is passed an ASocket handle instead of
        a file descriptor.

    INPUTS

        as       - ASocket handle
        level    - protocol level to use (ie IPPROTO_IP, SOL_SOCKET, etc)
        option   - option id
        val_addr - address of the value to send
        va_len   - length to send

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

    struct ASocket *as = s;

    D(bug("%s: as=%p, level=%d, option=%d, val_addr=%p, val_len=%d\n", __func__, as, level, option, val_addr, val_len));

    if (as->as_Node.ln_Type != NT_AS_SOCKET)
        return EINVAL;
    
    return bsd_setsockopt(ASocketBase->ab_bsd, as->as_bsd, level, option, val_addr, val_len);

    AROS_LIBFUNC_EXIT
}

