/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
#include "glyph.h"
#include "ttfglyphengine.h"
#include "ttfdebug.h"

#include <proto/exec.h>
#include <exec/memory.h>
#include <diskfont/oterrors.h>

/* convert freetype TT_F26Dot6 to Amiga FIXED */
#define F266toFX(x) (((x)<<10))

/* convert freetype TT_F26Dot6 to Integer */
#define F266toIR(x) (((x+32)>>6))
#define F266toI(x) ((x>>6))

/* return glyph index for charcode for current font of ge */
/* Convert an Amiga character to a glyph index. Actually, 2 translations
   take place. First from 8 bit Amiga char to unicode, via the engine
   codepage array. Then translate unicode to glyph index.
 */
int char_to_glyph(TTF_GlyphEngine *ge,int charcode)
{
    int gi=0;
	
    if((ge->cmap_index) & 0x10000)
    {	/* raw encoding */
	if(charcode>32 && charcode<128)
	{
	    gi = charcode - 32 + ((ge->cmap_index) & 0xFFFF);
	    if(gi > ge->properties.num_Glyphs)
		gi=0;
	}
    }
    else
    {	
	if(charcode<256)
	    gi=TT_Char_Index(ge->char_map, ge->codepage[charcode]);
	else
	{   /* pick up the rest as unicode */
	    gi=TT_Char_Index( ge->char_map, charcode );
	    if(charcode==0x25A1) gi=0; /* "not a char" in bullet */
	}
		
/* attempt to kludge in win symbol sets, blah, spit, yech! */
	if(gi==0 && charcode>=0x20 && charcode<=255
	   && ge->properties.os2->usFirstCharIndex>=0xF000)
	{
	    gi=TT_Char_Index( ge->char_map, 
			      (charcode
			       - 0x20
			       + ge->properties.os2->usFirstCharIndex
			      )
			    );
	}
    }

    return(gi);
}

/* call char_to_glyph for request_char in GlyphEngine and set
   glyph_code appropriately
 */
int UnicodeToGlyphIndex(TTF_GlyphEngine *ge)
{
    ge->glyph_code= char_to_glyph(ge,ge->request_char);

    return(ge->glyph_code);
}


/* render a glyph into GMap of engine
   caller in expected to have allocated GMap space (but not bitmap)
 */
void RenderGlyph(TTF_GlyphEngine *ge)
{
    TT_Error  error;
    int xoffset, yoffset, do_transform;
    TT_Pos trans_x=0;
    TT_Pos trans_y=0;
    TT_Raster_Map bitmap;

    /* these two values indicate no glyph rendered */
    ge->GMap->glm_Width = 0;
    ge->GMap->glm_BitMap = NULL;

    error = TT_Load_Glyph(ge->instance,
			  ge->glyph,
			  ge->glyph_code,
			  TTLOAD_DEFAULT);
    /*				TTLOAD_SCALE_GLYPH); */ /* test disable hinting */
    if(error)
    {
	D(bug("Error loading glyph %ld  code = %ld.\n",
	      (LONG)ge->request_char, (LONG) error ));
	return;
    }

    /* we will need the metrics, no matter what transform we apply */
    TT_Get_Glyph_Metrics( ge->glyph, &ge->metrics );
	
    TT_Get_Glyph_Outline( ge->glyph, &ge->outline );
	
    do_transform=(ge->do_rotate || ge->do_shear);
	
    if(ge->do_shear)
	/* apply shear transform to the outline */
	TT_Transform_Outline( &ge->outline, &ge->shear_matrix );
	
    if(ge->do_rotate)
    {
	/* apply rotate transform to the outline */
	TT_Transform_Outline( &ge->outline, &ge->rotate_matrix );
	
	/* position for next glyph, uses original metrics */
	trans_x=ge->metrics.advance;
	trans_y=0;
	TT_Transform_Vector(&trans_x,
			    &trans_y,
			    &ge->rotate_matrix);
    }

    if(do_transform)
	/* transform is applied, correct or get metrics */
	TT_Get_Outline_BBox( &ge->outline, &ge->metrics.bbox );

    bitmap.flow  = TT_Flow_Down;	/* bitmap should be top first */

    /* how big is glyph */
    bitmap.width =(ge->metrics.bbox.xMax - ge->metrics.bbox.xMin) / 64;
    bitmap.rows  =(ge->metrics.bbox.yMax - ge->metrics.bbox.yMin) / 64;

    /* adjust bitmap to 4 byte (32 bit) alignment for width */
    bitmap.width = (bitmap.width + 31) & -32;
    bitmap.cols  = bitmap.width / 8;	/* byte count */
    bitmap.size  = bitmap.rows * bitmap.cols;

    /* some (i.e. space) have no bitmap. Bullet.library returns
     a OTERR_UnknownGlyph, so we will ultimately, too */
    if(bitmap.size==0)
    {
	D(bug("no bitmap for glyph %ld\n",ge->request_char));
	return;
    }
	
/*	bitmap.bitmap= AllocPooled(ge->GlyphPool,(ULONG)bitmap.size); */
    bitmap.bitmap= AllocVec((ULONG)bitmap.size,MEMF_PUBLIC | MEMF_CLEAR);

    if(bitmap.bitmap==NULL)
    {
	D(bug("bitmap allocation for %ld bytes failed\n",bitmap.size));
	return;
    }

    xoffset = 0 - ge->metrics.bbox.xMin;
    yoffset = 0 - ge->metrics.bbox.yMin;

    TT_Translate_Outline( &ge->outline, xoffset, yoffset );

    error=TT_Get_Outline_Bitmap( ge->engine, &ge->outline, &bitmap );

    if(error)
    {
	D(bug("Error rendering glyph %ld  code = %ld.\n",
	      (LONG)ge->request_char, (LONG) error ));
	FreeVec(bitmap.bitmap);
	return;
    }
	
    ge->GMap->glm_BMModulo    = bitmap.cols;
    ge->GMap->glm_BMRows      = bitmap.rows;
    ge->GMap->glm_BlackLeft   = 0; 
    ge->GMap->glm_BlackTop    = 0;
    ge->GMap->glm_BlackWidth  = 
	F266toIR(ge->metrics.bbox.xMax - ge->metrics.bbox.xMin);

    ge->GMap->glm_BlackHeight = bitmap.rows;

    if(!(ge->do_rotate))
    {
	ge->GMap->glm_XOrigin     = ~F266toFX(ge->metrics.bearingX)+1;
	ge->GMap->glm_YOrigin     = F266toFX(ge->metrics.bearingY);
	
	ge->GMap->glm_X0          = 0-F266toI(ge->metrics.bearingX);
	ge->GMap->glm_Y0          = F266toI(ge->metrics.bbox.yMax);

	ge->GMap->glm_X1 = 
	    F266toI((ge->metrics.advance - ge->metrics.bearingX));

	ge->GMap->glm_Y1          = ge->GMap->glm_Y0; 
    }
    else
    {
	ge->GMap->glm_XOrigin     = ~F266toFX(ge->metrics.bbox.xMin)+1;
	ge->GMap->glm_YOrigin     = F266toFX(ge->metrics.bbox.yMax);
	ge->GMap->glm_X0          = 0-F266toI(ge->metrics.bbox.xMin);
	ge->GMap->glm_Y0          = F266toI(ge->metrics.bbox.yMax);

	ge->GMap->glm_X1 = F266toI(trans_x-ge->metrics.bbox.xMin);

	ge->GMap->glm_Y1 = F266toI(ge->metrics.bbox.yMax-trans_y);
    }
    /* horizontal pixels per EM square as 16.16 */

    ge->GMap->glm_Width = 
	(ge->metrics.advance << 10) / 
	(( ge->imetrics.x_ppem 
	   * ge->corrected_upem)
	 / ge->properties.header->Units_Per_EM);


    ge->GMap->glm_BitMap      = bitmap.bitmap;

#if 0
    D(bug("render glyph %ld to %ld\n",(LONG)ge->glyph_code,(LONG)ge->request_char));
    
    D(bug("   bbox.xMin=%lx  ",(LONG)ge->metrics.bbox.xMin));
    D(bug("bbox.yMin=%lx  ",(LONG)ge->metrics.bbox.yMin));
    D(bug("bbox.xMax=%lx  ",(LONG)ge->metrics.bbox.xMax));
    D(bug("bbox.yMax=%lx\n",(LONG)ge->metrics.bbox.yMax));

    D(bug("   horiBearingX=%lx  ",(LONG)ge->metrics.horiBearingX));
    D(bug("horiBearingY=%lx\n",(LONG)ge->metrics.horiBearingY));

    D(bug("   vertBearingX=%lx  ",(LONG)ge->metrics.vertBearingX));
    D(bug("vertBearingY=%lx\n",(LONG)ge->metrics.vertBearingY));

    D(bug("   horiAdvance=%lx  ",(LONG)ge->metrics.horiAdvance));
    D(bug("vertAdvance=%lx\n",(LONG)ge->metrics.vertAdvance));
#endif
#if 0
    D(bug(" glm_BMModulo    = %lx\n",(LONG)ge->GMap->glm_BMModulo));
    D(bug(" glm_BMRows      = %lx\n",(LONG)ge->GMap->glm_BMRows));
    D(bug(" glm_BlackLeft   = %lx\n",(LONG)ge->GMap->glm_BlackLeft));
    D(bug(" glm_BlackTop    = %lx\n",(LONG)ge->GMap->glm_BlackTop));
    D(bug(" glm_BlackWidth  = %lx\n",(LONG)ge->GMap->glm_BlackWidth));
    D(bug(" glm_BlackHeight = %lx\n",(LONG)ge->GMap->glm_BlackHeight));

    D(bug(" glm_XOrigin     = %lx\n",(LONG)ge->GMap->glm_XOrigin));
    D(bug(" glm_YOrigin     = %lx\n",(LONG)ge->GMap->glm_YOrigin));
    D(bug(" glm_X0          = %lx\n",(LONG)ge->GMap->glm_X0));
    D(bug(" glm_Y0          = %lx\n",(LONG)ge->GMap->glm_Y0));

    D(bug(" glm_X1          = %lx\n",(LONG)ge->GMap->glm_X1));
    D(bug(" glm_Y1          = %lx\n",(LONG)ge->GMap->glm_Y1));
    D(bug(" glm_Width       = %lx\n",(LONG)ge->GMap->glm_Width));
#endif
}

/* set current glyph to no character glyph (#0)
   assume Gmap already allocated
 */
void XeroGlyph(TTF_GlyphEngine *ge)
{
#if 0
    D(bug("Xeroing %ld\n",(LONG)i));
#endif	
    ge->glyph_code=0;
    /* allocate GMap space */
    ge->GMap=AllocVec((ULONG)sizeof(struct GlyphMap),
		      MEMF_PUBLIC | MEMF_CLEAR);
    if(ge->GMap)
	RenderGlyph(ge);
}

/* given an Amiga character, return a filled in GlyphMap */
struct GlyphMap *GetGlyph(TTF_GlyphEngine *ge)
{
    if (ge->instance_changed)
    {
	/* reset everything first */
	if(SetInstance(ge)!=OTERR_Success)
	    return NULL;
    }

    if (ge->cmap_index==NO_CMAP_SET && ChooseEncoding(ge)!=0)
	return NULL;

#if 0
    if(ge->request_char>255)
    {
	/* out of range */
	XeroGlyph(ge);
	return ge->GMap;
    }
#endif

    UnicodeToGlyphIndex(ge);
    if(ge->glyph_code)
    {	/* has code, try to render */
	/* first, get a GlyphMap structure to fill in */
	ge->GMap=AllocVec((ULONG)sizeof(struct GlyphMap),
			  MEMF_PUBLIC | MEMF_CLEAR);
	if(ge->GMap)
	    RenderGlyph(ge);
    }
    else
    {
	set_last_error(ge,OTERR_UnknownGlyph);
	return(NULL);
    }

    /* odd ball glyph (includes SPACE, arrrgghhh) mimic bullet */
    if (ge->GMap->glm_Width == 0)
    {
	set_last_error(ge,OTERR_UnknownGlyph);
	FreeVec(ge->GMap);
	return(NULL);
    }

    return ge->GMap;
}
