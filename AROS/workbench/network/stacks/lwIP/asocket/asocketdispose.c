/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH1(VOID, ASocketDispose,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 6, ASocket)

/*  FUNCTION
 
        Dispose of a socket handle.

    INPUTS

        as - Socket handle to dispose.

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

    D(bug("%s: as=%p\n", __func__, as));

    bsd_close(ASocketBase->ab_bsd, as);

    return;

    AROS_LIBFUNC_EXIT
}

