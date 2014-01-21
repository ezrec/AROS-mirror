/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH1(VOID, ASocketDispose,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),

/*  LOCATION */
        struct Library *, ASocketBase, 6, ASocket)

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

    AROS_FUNCTION_NOT_IMPLEMENTED(asocket);

    return;

    AROS_LIBFUNC_EXIT
}

