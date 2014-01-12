/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(int, Dup2Socket,

/*  SYNOPSIS */
        AROS_LHA(int, fd1, D0),
        AROS_LHA(int, fd2, D1),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 44, BSDSocket)

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

    return bsd_socket_dup2(bsdsocketBase->bsd, fd1, fd2);

    AROS_LIBFUNC_EXIT

} /* Dup2Socket */
