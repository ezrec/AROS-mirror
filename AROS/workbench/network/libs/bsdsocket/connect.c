/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <sys/socket.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(int, connect,

/*  SYNOPSIS */
        AROS_LHA(int,               s,       D0),
        AROS_LHA(struct sockaddr *, name,    A0),
        AROS_LHA(int,               namelen, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 9, BSDSocket)

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

    APTR as;
    LONG err;
    struct ASocket_Address asa;

    as = BSD_GET_ASOCKET(SocketBase, s);

    asa.asa_Address = name;
    asa.asa_Length = namelen;

    err = ASocketSet(as, AS_TAG_SOCKET_ENDPOINT, &asa, TAG_END);

    BSD_SET_ERRNO(SocketBase, err);
    return (err == 0) ? 0 : -1;

    AROS_LIBFUNC_EXIT

} /* connect */
