/*
    Copyright © <year>, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "miami_intern.h"

/*****************************************************************************

    NAME */
        AROS_LH4(LONG, getaddrinfo,

/*  SYNOPSIS */
        AROS_LHA(const char *, hostname, A0),
        AROS_LHA(const char *, servicename, A1),
        AROS_LHA(const struct addrinfo *, hintsp, A2),
        AROS_LHA(struct addrinfo **, result, A3),

/*  LOCATION */
        struct Library *, MiamiBase, 44, Miami)

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

