/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "asocket_intern.h"

/*****************************************************************************

    NAME */
        #include <proto/asocket.h>

        AROS_LH1(VOID, ASocketListRelease,

/*  SYNOPSIS */
        AROS_LHA(struct List *, aslist, A0),

/*  LOCATION */
        struct ASocketBase *, ASocketBase, 9, ASocket)

/*  FUNCTION

    INPUTS

        aslist - An ASocketList from ASocketListObtain()

    RESULT

        None

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketListObtain()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct ASocket *as, *tmp;

    D(bug("%s: aslist=%p\n", __func__, aslist));

    ForeachNodeSafe(aslist, as, tmp) {

        REMOVE(as);
        bsd_close(ASocketBase->ab_bsd, as->as_bsd);
        FreeVec(as);
    }

    FreeVec(aslist);

    AROS_LIBFUNC_EXIT
}

