/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH0(int, getdtablesize,

/*  SYNOPSIS */

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 23, BSDSocket)

/*  FUNCTION
        
        Return value of maximum  number of open socket  descriptors.
        Larger  socket  descriptor  table  can   be  allocated  with
        the SocketBaseTagList() call.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        SocketBaseTagList()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return SocketBase->bsd_fds;

    AROS_LIBFUNC_EXIT

} /* getdtablesize */
