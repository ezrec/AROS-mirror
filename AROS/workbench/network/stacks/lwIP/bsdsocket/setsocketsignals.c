/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(void, SetSocketSignals,

/*  SYNOPSIS */
        AROS_LHA(ULONG, intrmask, D0),
        AROS_LHA(ULONG, iomask,   D1),
        AROS_LHA(ULONG, urgmask,  D2),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 22, BSDSocket)

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

    SocketBase->sigintr = intrmask;
    SocketBase->sigio   = iomask;
    SocketBase->sigurg  = urgmask;

    AROS_LIBFUNC_EXIT

} /* SetSocketSignals */
