/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom function ObtainInfoA()
    Lang: English
*/

#include "freedom_intern.h"
#include <proto/exec.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#define Config_NoPointFraction 0
#define Config_UseHintedAdvance 0

#define DEBUG 0
#   include <aros/debug.h>

static UBYTE emptybitmap[4];

/*****************************************************************************

    NAME */

	AROS_LH2(ULONG, ObtainInfoA,

/*  SYNOPSIS */
    	AROS_LHA(struct GlyphEngine *, glyphEngine, A0),
	AROS_LHA(struct TagItem *, tagList, A1),
	
/*  LOCATION */
	struct Library *, FreedomBase, 8, Freedom)

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

    struct TagItem  	    *tstate,*ti;
    struct GlyphMap 	    *gm;
    struct GlyphWidthEntry  *gwe;
    UBYTE   	    	    *widthentrybuffer;
    LONG    	    	    data,advance;
    WORD    	    	    xmin,xmax,ymin,ymax,width,height,width32,glyph;
    /* char    	    	    s[100];*/
    
    ULONG rc = OTERR_Success;

    tstate = tagList;

    D(bug("\n*** ObtainInfoA ***\n"));

    while ((ti = NextTagItem((const struct TagItem **)&tstate)))
    {
        data = ti->ti_Data;

        switch(ti->ti_Tag)
        {
            case OT_GlyphMap:
                D(bug("  OT_GlyphMap 0x%lx\n", data));
                if (!data)
                {
                    rc = OTERR_BadTag;
                    break;
                }
                
                if (!MYGE(glyphEngine)->gle_FontOK)
                {
                    rc = OTERR_NoFace;
                    break;
                }
                
                if (TT_Set_Instance_Resolutions(MYGE(glyphEngine)->gle_Instance,
                                                          MYGE(glyphEngine)->gle_GlyphRequest.gr_DeviceDPI_X,
                                                          MYGE(glyphEngine)->gle_GlyphRequest.gr_DeviceDPI_Y) != 0)
                {
                    D(bug("SetInstanceResolutions failed!"));
                    rc = OTERR_Failure;
                    break;
                }
                
                if (Config_NoPointFraction)
                {
                    advance = 0xFFFF0000;
                } else {
                    advance = 0xFFFFFFFF;
                }

                if (TT_Set_Instance_CharSizes(MYGE(glyphEngine)->gle_Instance,
                                              FIXED_TO_FIXED26(TT_MulDiv(MYGE(glyphEngine)->gle_GlyphRequest.gr_PointHeight,
                                                                                  MYGE(glyphEngine)->gle_GlyphRequest.gr_SetFactor,
                                                                                  0x10000) & advance),
                                              FIXED_TO_FIXED26(MYGE(glyphEngine)->gle_GlyphRequest.gr_PointHeight & advance)
                                             ) != 0)
                {
                    D(bug("SetInstanceCharSizes failed!"));
                    rc = OTERR_Failure;
                    break;
                }

                glyph = TT_Char_Index(MYGE(glyphEngine)->gle_CharMap,MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode);

                if (TT_Load_Glyph(MYGE(glyphEngine)->gle_Instance,
                                  MYGE(glyphEngine)->gle_Glyph,
                                  glyph,
                                  TTLOAD_DEFAULT) != 0)
                {
                    D(bug("LoadGlyph failed!"));
                    rc = OTERR_Failure;
                    break;
                }                        

                TT_Get_Glyph_Metrics(MYGE(glyphEngine)->gle_Glyph,&MYGE(glyphEngine)->gle_Glyph_Metrics);
                TT_Get_Instance_Metrics(MYGE(glyphEngine)->gle_Instance,&MYGE(glyphEngine)->gle_Instance_Metrics);

                /* Shearing */
                
                if ((MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearCos != 0) &&
                     ((MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearCos != 0x10000) ||
                      (MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearSin != 0))
                    )
                {
                    MYGE(glyphEngine)->gle_Matrix.xx = 0x10000;
                    MYGE(glyphEngine)->gle_Matrix.xy = TT_MulDiv(MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearSin,
                                                                        0x10000,
                                                                        MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearCos);
                    
                    MYGE(glyphEngine)->gle_Matrix.yx = 0;
                    MYGE(glyphEngine)->gle_Matrix.yy = 0x10000;
                    
                    TT_Get_Glyph_Outline(MYGE(glyphEngine)->gle_Glyph,
                                                &MYGE(glyphEngine)->gle_Outline);

                    TT_Transform_Outline(&MYGE(glyphEngine)->gle_Outline,
                                                &MYGE(glyphEngine)->gle_Matrix);

                    TT_Get_Outline_BBox(&MYGE(glyphEngine)->gle_Outline,
                                              &MYGE(glyphEngine)->gle_Glyph_Metrics.bbox);

                    MYGE(glyphEngine)->gle_Glyph_Metrics.bearingX = MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.xMin;
                    MYGE(glyphEngine)->gle_Glyph_Metrics.bearingY = MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.yMax;
                    
                }

                /* Rotation */

                if ((MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateCos != 0x10000) ||
                     (MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateSin != 0))
                {
                    MYGE(glyphEngine)->gle_Matrix.xx = MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateCos;
                    MYGE(glyphEngine)->gle_Matrix.xy = -MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateSin;
                    
                    MYGE(glyphEngine)->gle_Matrix.yx = MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateSin;
                    MYGE(glyphEngine)->gle_Matrix.yy = MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateCos;
                    
                    TT_Get_Glyph_Outline(MYGE(glyphEngine)->gle_Glyph,
                                                &MYGE(glyphEngine)->gle_Outline);

                    TT_Transform_Outline(&MYGE(glyphEngine)->gle_Outline,
                                                &MYGE(glyphEngine)->gle_Matrix);

                    TT_Get_Outline_BBox(&MYGE(glyphEngine)->gle_Outline,
                                              &MYGE(glyphEngine)->gle_Glyph_Metrics.bbox);

                    MYGE(glyphEngine)->gle_Glyph_Metrics.bearingX = MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.xMin;
                    MYGE(glyphEngine)->gle_Glyph_Metrics.bearingY = MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.yMax;
                    
                }
                
                if (Config_UseHintedAdvance)
                {
                    advance = TT_MulDiv(MYGE(glyphEngine)->gle_Glyph_Metrics.advance,
                                              0x10000,
                                              MYGE(glyphEngine)->gle_Instance_Metrics.x_scale);
                }
		else
		{
                
                    TT_Get_Face_Metrics(MYGE(glyphEngine)->gle_Face,
                                              glyph,
                                              glyph,
                                              0,
                                              (TT_UShort *)&width,
                                              0,
                                              0);
                    advance = width;
                }
                
                advance = TT_MulDiv(advance,
                                    0x10000,
                                    MYGE(glyphEngine)->gle_Face_Properties.header->Units_Per_EM);
                

                
                xmin=FLOOR(MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.xMin);
                ymin=FLOOR(MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.yMin);

                xmax=CEILING(MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.xMax);
                ymax=CEILING(MYGE(glyphEngine)->gle_Glyph_Metrics.bbox.yMax);
                
                width=(xmax-xmin)/64;
                width32=(width+31) & -32;
                height=(ymax-ymin)/64;

                if (!(gm=AllocVec(sizeof(struct GlyphMap),MEMF_PUBLIC|MEMF_CLEAR)))
                {
                    rc = OTERR_NoMemory;
                    break;
                }
                
                if (width>0 && height>0)
                {
                    if (!(gm->glm_BitMap=AllocVec(height * (width32 / 8),MEMF_PUBLIC|MEMF_CLEAR)))
                    {
                        FreeVec(gm);
                        rc = OTERR_NoMemory;
                        break;
                    }
                
                    MYGE(glyphEngine)->gle_Raster_Map.rows = height;
                    MYGE(glyphEngine)->gle_Raster_Map.cols = width32 / 8;
                    MYGE(glyphEngine)->gle_Raster_Map.width = width;
                    MYGE(glyphEngine)->gle_Raster_Map.flow = TT_Flow_Down;
                    MYGE(glyphEngine)->gle_Raster_Map.bitmap = gm->glm_BitMap;
                    
                    if (TT_Get_Glyph_Bitmap(MYGE(glyphEngine)->gle_Glyph,
                                                    &MYGE(glyphEngine)->gle_Raster_Map,
                                                    -xmin,
                                                    -ymin) != 0)
                    {
                        D(bug("GetGlyphBitMap failed!"));
                        FreeVec(gm->glm_BitMap);
                        FreeVec(gm);
                        rc = OTERR_Failure;
                        break;
                    }

                    gm->glm_BMModulo = width32 / 8;
                    gm->glm_BMRows = height;
                    gm->glm_BlackLeft = 0;
                    gm->glm_BlackTop = 0;
                    gm->glm_BlackWidth = width;
                    gm->glm_BlackHeight = height;

                } /* if (width>0 && height>0) */ else
                {
                    width = 1;
                    height = 1;

                    gm->glm_BitMap = emptybitmap;
                
                    gm->glm_BMModulo = 4;
                    gm->glm_BMRows = height;
                    
                    gm->glm_BlackLeft = 0;
                    gm->glm_BlackTop = 0;
                    gm->glm_BlackWidth = width;
                    gm->glm_BlackHeight = height;
                }
                
                gm->glm_XOrigin = FIXED26_TO_FIXED(-MYGE(glyphEngine)->gle_Glyph_Metrics.bearingX);
                gm->glm_YOrigin = FIXED26_TO_FIXED(MYGE(glyphEngine)->gle_Glyph_Metrics.bearingY);
                
                gm->glm_X0 = FIXED_TO_APPROX(gm->glm_XOrigin);
                gm->glm_Y0 = FIXED_TO_APPROX(gm->glm_YOrigin);
                gm->glm_X1 = FIXED_TO_APPROX(gm->glm_XOrigin + FIXED26_TO_FIXED(MYGE(glyphEngine)->gle_Glyph_Metrics.advance));
                gm->glm_Y1 = gm->glm_Y0 + height - 1;

                gm->glm_Width = advance;
                
                D(bug("   [black = %ld,%ld - %ld x %ld   xy = %ld, %ld - %ld,%ld  width = %lx (%ld)]",
                         gm->glm_BlackLeft, gm->glm_BlackTop,
                     gm->glm_BlackWidth, gm->glm_BlackHeight,
                     gm->glm_X0,gm->glm_Y0,
                     gm->glm_X1,gm->glm_Y1,
                     gm->glm_Width,
                     gm->glm_Width >> 16));
                     
                *(struct GlyphMap **)data = gm;
                break;
                
            case OT_TextKernPair:
                D(bug("  OT_TextKernPair 0x%lx\n", data));
                if (data)
                {
                    *(FIXED *)data = 0;
                } else {
                    rc = OTERR_BadTag;
                }
                break;
                
            case OT_DesignKernPair:
                D(bug("  OT_DesignKernPair 0x%lx\n", data));
                if (data)
                {
                    *(FIXED *)data = 0;
                } else {
                    rc = OTERR_BadTag;
                }
                break;
            
            case OT_WidthList:
                D(bug("  OT_WidthList 0x%lx\n", data));

                if (!(widthentrybuffer = AllocVec(sizeof(struct MinList) + (MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode2 - MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode + 1) * sizeof(struct GlyphWidthEntry),MEMF_PUBLIC)))
                {
                    rc = OTERR_NoMemory;
                    *(UBYTE *)data = 0;
                    
                }
		else
		{
                    NewList((struct List *)widthentrybuffer);
                    gwe = (struct GlyphWidthEntry *)(widthentrybuffer + sizeof(struct MinList));

                    for(width = MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode;width <= MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode2;width++)
                    {
                        glyph = TT_Char_Index(MYGE(glyphEngine)->gle_CharMap,width);
                        if (glyph)
                        {
                            TT_Get_Face_Metrics(MYGE(glyphEngine)->gle_Face,
                                                      glyph,
                                                      glyph,
                                                      0,
                                                      (TT_UShort *)&xmax,
                                                      0,
                                                      0);

                            advance = TT_MulDiv(xmax,
                                                    0x10000,
                                                    MYGE(glyphEngine)->gle_Face_Properties.header->Units_Per_EM);
                            
                            
                            gwe->gwe_Code = width;
                            gwe->gwe_Width = advance;
                            AddTail((struct List *)widthentrybuffer,(struct Node *)gwe);

                            gwe++;                            
                        }
                    }
                    *(UBYTE **)data = widthentrybuffer;
                    
                }

                break;

            default:
                D(bug("  OT_??? tag = 0x%lx data = %ld (0x%lx) \n", ti->ti_Tag, data, data));
    	    	break;
		
        } /* switch(ti->ti_Tag) */
	
        if (rc) break;

    } /* while ((ti=NextTagItem(&tstate))) */

    D(bug("\n#### ObtainInfoA done retval = %ld ###\n", rc));
    
    return rc;

    AROS_LIBFUNC_EXIT

} /* ObtainInfoA */
