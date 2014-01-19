/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/errno.h>
#include <netdb.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(struct protoent *, getprotobyname,

/*  SYNOPSIS */
        AROS_LHA(char *, name, A0),

/*  LOCATION */
        struct bsdsocketBase *, taskBase, 41, BSDSocket)

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

    aros_print_not_implemented ("getprotobyname");
/* FIXME: Write BSDSocket/getprotobyname */
    return 0;

    AROS_LIBFUNC_EXIT

} /* getprotobyname */
