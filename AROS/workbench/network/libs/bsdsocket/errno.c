/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH0(LONG, Errno,

/*  SYNOPSIS */

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 27, BSDSocket)

/*  FUNCTION
 
        When  some  function  in  socket  library  return  an  error
        condition value, they also set a specific error value.  This
        error value can be extracted by this function.

    INPUTS

        None

    RESULT

        Error value  indicating  the error on  last failure  of some
        socket function call.

    NOTES

        Return  value  of  Errno()  is not changed  after successful
        function so it cannot be used to determine success of any
        function call  of this library.  Also, another function call
        to this  library may change  the return value of  Errno() so
        use it right after error occurred.

    EXAMPLE

    BUGS

    SEE ALSO

        SetErrnoPtr()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return SocketBase->bsd_errno.value;

    AROS_LIBFUNC_EXIT

} /* */
