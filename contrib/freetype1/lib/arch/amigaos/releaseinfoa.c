/*
 * Based on the code from the ttf.library by Richard Griffith and the
 * type1.library by Amish S. Dave
 */
#include "ttfglyphengine.h"
#include "kerning.h"
#include "ttfdebug.h"

#include <proto/utility.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/glyph.h>
#include <utility/tagitem.h>


ULONG ReleaseInfoA(struct GlyphEngine *_engine, struct TagItem *tags)
{
    TTF_GlyphEngine *engine = (TTF_GlyphEngine *)_engine;
    Tag   otagtag;
    ULONG otagdata;

    struct TagItem *tstate;
    struct TagItem *tag;
    struct GlyphMap *GMap;

    tstate = tags;
    while (tag = NextTagItem(&tstate))
    {
	otagtag  = tag->ti_Tag;
	otagdata = tag->ti_Data;

	switch (otagtag)
	{
	case OT_GlyphMap:
	    //D(bug("Release: OT_GlyphMap  Data=%lx\n", otagdata));
	    GMap=(struct GlyphMap *)otagdata;
	    if(GMap->glm_BitMap) FreeVec(GMap->glm_BitMap);
	    FreeVec(GMap);
	    break;

	case OT_WidthList:
	    //D(bug("Release: OT_WidthList  Data=%lx\n", otagdata));
	    FreeWidthList((struct MinList  *)otagdata);
	    break;

	default: /* ??? */
	    D(bug("Unknown Release: Tag=%lx  Data=%lx\n",(LONG)otagtag, otagdata));
	    break;
	}
    }

    return 0;
}
