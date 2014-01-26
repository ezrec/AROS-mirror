/*
    Copyright © 2014, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************

    NAME */

        #include <protos/asocket.h>

        AROS_LH2(BOOL, ASocketCompareA,

/*  SYNOPSIS */
        AROS_LHA(APTR, as, A0),
        AROS_LHA(struct TagItem *, pattern, A1),

/*  LOCATION */
        struct Library *, ASocketBase, 18, ASocket)

/*  FUNCTION
 
        Determine if an ASocket matches the descrition in 'tags'.

    INPUTS

        as      - Pointer to an existing ASocket handle

        pattern - Tags list to compare against

        All readable AS_TAG_* tags can be used to compare against.
        Unknown tags will be ignored.

    RESULT

        TRUE if the socket matches the tag list.

        FALSE if the socket does not match.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

        ASocketListObtain(), ASocketListRelease()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    AROS_FUNCTION_NOT_IMPLEMENTED(asocket);

    return FALSE;

    AROS_LIBFUNC_EXIT
}

