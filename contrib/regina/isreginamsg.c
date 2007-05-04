/*
    Copyright © 2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <rexx/storage.h>
#include <proto/rexxsyslib.h>

#include "rexx.h"

/*****************************************************************************

    NAME */

BOOL IsReginaMsg (struct RexxMsg *msg)

/*  FUNCTION
        Check to see if message was generated from the regina interpreter.
        This function will look at the rm_Private1 and rm_Private2 fields
        to see if they are valid.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    tsd_node_t *node;
    BOOL ok;
    struct Library *RexxSysBase;
    
    RexxSysBase = OpenLibrary("rexxsyslib.library", 0);
    if (RexxSysBase == NULL) return FALSE;
    ok = IsRexxMsg(msg);
    CloseLibrary(RexxSysBase);
    if (!ok) return FALSE;

    ok = FALSE;
    ForeachNode(__regina_tsdlist, node)
    {
	if (node->TSD == (tsd_t *)msg->rm_Private2)
	{
	    ok = TRUE;
	    break;
	}
    }

    return ok;
}
