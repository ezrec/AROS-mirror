/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH2(LONG, ASocketDuplicate,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(APTR *, new_as, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 7, ASocket)

/*  FUNCTION
 
        Duplicate an ASocket handle. Note that the duplicate
        is identical in all ways to the original, except that
        ASocketDispose() of the original or duplicate leaves
        the other undisturbed.

    INPUTS

        as - ASocket handle
        new_as - Location to recieve the duplicate ASocket handle

    RESULT

        0 on success, and 'errno' value on failue.

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

    return EINVAL;

    AROS_LIBFUNC_EXIT
}

