/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom function OpenEngine()
    Lang: English
*/

#include "freedom_intern.h"
#include <proto/exec.h>
#include <proto/freedom.h>

/*****************************************************************************

    NAME */

	AROS_LH0(struct GlyphEngine *, OpenEngine,

/*  SYNOPSIS */

/*  LOCATION */
	struct Library *, FreedomBase, 5, Freedom)

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
    AROS_LIBBASE_EXT_DECL(struct Library *,FreedomBase)

    struct MyGlyphEngine *rc;

    if ((rc = AllocMem(sizeof(struct MyGlyphEngine), MEMF_PUBLIC | MEMF_CLEAR)))
    {
	if (TT_Init_FreeType(&rc->gle_Engine) != 0)
	{
	    CloseEngine((struct GlyphEngine *)rc);
	    rc=0;
	}
	else
	{
	    ClearGlyphRequest(rc);
	    
	    rc->gle_Library = (struct Library *)FreedomBase;
	    rc->gle_Name    = "freedom";

	} /* if (TT_Init_FreeType(&rc->gle_Engine) != 0) else ... */

    } /* if ((rc = AllocMem(sizeof(struct MyGlyphEngine), MEMF_PUBLIC | MEMF_CLEAR))) */

    return (struct GlyphEngine *)rc;
    
    AROS_LIBFUNC_EXIT

} /* OpenEngine */
