/*
 * Based on the code from the ttf.library by Richard Griffith and the
 * type1.library by Amish S. Dave
 */
#include "kerning.h"
#include "glyph.h"
#include "face.h"
#include "ttfdebug.h"

#include <proto/utility.h>
#include <diskfont/oterrors.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/glyph.h>
#include <utility/tagitem.h>

#include <string.h>

static void switch_family(TTF_GlyphEngine *engine)
{
    int pick_bi;

    /* switch files for family support if needed */
    pick_bi=engine->bold_sig + engine->italic_sig;

    switch(pick_bi)
    {
    case 0: /* normal */
	/* make sure we are set to base name */
	if(strcmp(engine->base_filename,engine->ttf_filename))
	{
	    OpenFace(engine,engine->base_filename);
	    D(bug("switch_family: revert to base\n"));
	}
	break;
    case 1: /* just italic */
	if(strlen(engine->italic_filename))
	{
	    if(strcmp(engine->italic_filename,engine->ttf_filename))
	    {
		OpenFace(engine,engine->italic_filename);
		D(bug("switch_family: set italic\n"));
	    }
	    engine->do_shear=0;
	}
	break;
    case 2: /* just bold */
	if(strlen(engine->bold_filename))
	{
	    if(strcmp(engine->bold_filename,engine->ttf_filename))
	    {
		OpenFace(engine,engine->bold_filename);
		D(bug("switch_family: set bold\n"));
	    }
	    engine->do_embold=0;
	}
	break;
    case 3: /* bold and italic */
	if(strlen(engine->bold_italic_filename))
	{
	    if(strcmp(engine->bold_italic_filename,engine->ttf_filename))
	    {
		OpenFace(engine,engine->bold_italic_filename);
		D(bug("switch_family: set bold italic\n"));
	    }
	    engine->do_shear=0;
	    engine->do_embold=0;
	}
	else
	{ /* no file, choose other as basis? */
	    /* we have a bold, use it and keep transform */
	    if(strlen(engine->bold_filename))
	    {
		if(strcmp(engine->bold_filename,engine->ttf_filename))
		{
		    OpenFace(engine,engine->bold_filename);
		    D(bug("switch_family: set bold, algo italic\n"));
		}
		engine->do_embold=0;
	    }
	}
	break;
    }

    /* just incase we slip through */
    if(engine->face_established==FALSE) 
	OpenFace(engine,engine->base_filename);

    return;
}


ULONG ObtainInfoA(struct GlyphEngine *_engine, struct TagItem *tags)
{
    TTF_GlyphEngine *engine = (TTF_GlyphEngine *)_engine;
    struct GlyphMap **gm_p;
    struct MinList **gw_p;
    Tag   otagtag;
    ULONG otagdata;

    struct TagItem *tstate;
    struct TagItem *tag;

    ULONG rc = 0;

    /* establish the correct face */
    switch_family(engine);

    /* can't do anything without a face */
    if(engine->face_established==FALSE) return(OTERR_Failure);

    tstate = tags;
    while (tag = NextTagItem(&tstate))
    {
	otagtag  = tag->ti_Tag;
	otagdata = tag->ti_Data;
	
	switch (otagtag)
	{
	case OT_GlyphMap:
//			D(bug("Obtain: OT_GlyphMap  Data=%lx\n", otagdata));
	    gm_p = (struct GlyphMap **)otagdata;

	    if((*gm_p = GetGlyph(engine))==NULL)
		rc = (ULONG) engine->last_error;
	    break;
			
	case OT_WidthList:
	    D(bug("Obtain: OT_WidthList  Data=%lx\n", otagdata));
			
	    gw_p = (struct MinList  **)otagdata;

	    if((*gw_p = GetWidthList(engine))==NULL)
		rc = (ULONG) engine->last_error;
	    break;

	case OT_TextKernPair:
	case OT_DesignKernPair:
	    D(bug("Obtain: KernPair Data=%lx\n", otagdata));
	    *((ULONG *)otagdata)=get_kerning_dir(engine);
	    break;
		
	default:
	    D(bug("Unanswered Obtain: Tag=%lx  Data=%lx\n",(LONG)otagtag, otagdata));
	    rc=OTERR_UnknownTag;
	    break;
	}
    }

    return rc;
}
