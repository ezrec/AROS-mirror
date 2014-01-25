/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "miami_intern.h"

/*****************************************************************************

    NAME */
        AROS_LH7(LONG, getnameinfo,

/*  SYNOPSIS */
        AROS_LHA(const struct sockaddr *, sa, A0),
        AROS_LHA(LONG, addrlen, D0),
        AROS_LHA(char *, hostname, A1),
        AROS_LHA(LONG, hostlen, D1),
        AROS_LHA(char *, servicename, A2),
        AROS_LHA(LONG, servicelen, D2),
        AROS_LHA(LONG, flags, D3),

/*  LOCATION */
        struct Library *, MiamiBase, 45, Miami)

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

    return -1;

    AROS_LIBFUNC_EXIT
}

