/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom function CloseEngine()
    Lang: English
*/

#include "freedom_intern.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */

	AROS_LH1(void, CloseEngine,

/*  SYNOPSIS */
    	AROS_LHA(struct GlyphEngine *, glyphEngine, A0),
	
/*  LOCATION */
	struct Library *, FreedomBase, 6, Freedom)

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

    if (glyphEngine)
    {
	if (MYGE(glyphEngine)->gle_Glyph.z)
	{
	    TT_Done_Glyph(MYGE(glyphEngine)->gle_Glyph);
	}

	if (MYGE(glyphEngine)->gle_Instance.z)
	{
	    TT_Done_Instance(MYGE(glyphEngine)->gle_Instance);
	}

	if (MYGE(glyphEngine)->gle_Face.z)
	{
	    TT_Close_Face(MYGE(glyphEngine)->gle_Face);
	}

	if (MYGE(glyphEngine)->gle_Engine.z)
	{
	    TT_Done_FreeType(MYGE(glyphEngine)->gle_Engine);
	}

	FreeMem(glyphEngine, sizeof(struct MyGlyphEngine));
    }
    
    AROS_LIBFUNC_EXIT

} /* CloseEngine */
