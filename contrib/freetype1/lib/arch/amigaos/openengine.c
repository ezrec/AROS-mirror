/*
 * Based on the code from the ttf.library by Richard Griffith and the
 * type1.library by Amish S. Dave
 */
#include "ttfglyphengine.h"
#include "ttfdebug.h"

#include <exec/types.h>
#include <diskfont/glyph.h>
#include <aros/symbolsets.h>

static struct Library *mylib;

struct GlyphEngine *OpenEngine(void)
{
    static UBYTE *EngineName = "freetype1";
    TTF_GlyphEngine *ge;

    D(bug("Opening freetype1 engine\n"));
    
    ge = AllocGE();
    ge->gle_Library = mylib;
    ge->gle_Name = EngineName;

    return ((struct GlyphEngine *)ge);
}

static AROS_SET_LIBFUNC(getbase, struct Library *, TTFBase)
{
    AROS_SET_LIBFUNC_INIT
    
    mylib = TTFBase;
    
    return TRUE;
    
    AROS_SET_LIBFUNC_EXIT
}

ADD2INITLIB(getbase, 0);
