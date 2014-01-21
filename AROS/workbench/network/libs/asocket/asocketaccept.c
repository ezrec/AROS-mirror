/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH2(LONG, ASocketAccept,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(APTR *, new_as, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 16, ASocket)

/*  FUNCTION
  
        If there is a FD_ACCEPT pending, this function creates a
        new ASocket handle for the incoming connection.

    INPUTS

        as - ASocket handle that is listening
        new_as - Location to place the new ASocket handle

    RESULT

        0      - Success, new ASocket handle in 'new_as'
        ENOMEM - Out of memory allocation new handle
        EAGAIN - There is no pending FD_ACCEPT
        EINVAL - This socket has no AS_TAG_LISTEN_BACKLOG set

    NOTES

        An ASocket must have its AS_TAG_LISTEN_BACKLOG set to
        a non-zero value to be able to accept incoming connections.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    AROS_FUNCTION_NOT_IMPLEMENTED(asocket);

    return EINVAL;

    AROS_LIBFUNC_EXIT
}

