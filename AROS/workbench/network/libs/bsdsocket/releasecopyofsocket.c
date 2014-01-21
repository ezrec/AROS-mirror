/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH2(LONG, ReleaseCopyOfSocket,

/*  SYNOPSIS */
        AROS_LHA(LONG, sd, D0),
        AROS_LHA(LONG, id, D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 26, BSDSocket)

/*  FUNCTION

        Make a new reference to a given socket (pointed by its descriptor)
        and release it to the socket list held by bsdsocket.library.

    INPUTS

        fd - descriptor of the socket to release.
        id - the key value to identify use of this socket. It can be
             unique or not, depending on its  value.  If id value is
             between 0  and  65535,  inclusively,  it is  considered
             nonunique  and it can  be  used as a port  number,  for
             example.   If  id is greater  than  65535 and less than
             2^31) it  must be unique in currently  held sockets  in
             the bsdsocket.library socket  list,  otherwise an error
             will  be returned  and  socket  is  not  released.   If
             id  == UNIQUE_ID (defined in <sys/socket.h>) an  unique
             id will be generated.

    RESULT

        id - -1 in case of error and the key value of the socket put
             in the list. Most useful when an unique id is generated
             by this routine.

        EINVAL - Requested unique id is already used.

        ENOMEM - Needed memory couldn't be allocated.

    NOTES

        The socket descriptor is not deallocated.

    EXAMPLE

    BUGS

    SEE ALSO

        ObtainSocket(), ReleaseSocket()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    int ns;

    ns = Dup2Socket(sd, -1);
    if (ns < 0)
        return ns;

    return ReleaseSocket(ns, id);

    AROS_LIBFUNC_EXIT

} /* ReleaseCopyOfSocket */
