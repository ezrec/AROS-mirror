/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function __regina_faked_main()
    Lang: English
*/

#include "regina_intern.h"
#include <rexx/storage.h>

#include <proto/rexxsyslib.h>

/*****************************************************************************

    NAME */

	AROS_LH1(BOOL, IsReginaMsg,

/*  SYNOPSIS */
	AROS_LHA(struct RexxMsg *, msg, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 36, Regina)

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
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,ReginaBase)
    
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
    
    AROS_LIBFUNC_EXIT
}
