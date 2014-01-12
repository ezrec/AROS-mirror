/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(int, listen,

/*  SYNOPSIS */
        AROS_LHA(int, s,       D0),
        AROS_LHA(int, backlog, D1),

/*  LOCATION */
        struct bsdsocketBase *, bsdsocketBase, 7, BSDSocket)

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

    return bsd_listen(bsdsocketBase->bsd, s, backlog);

    AROS_LIBFUNC_EXIT

} /* listen */
