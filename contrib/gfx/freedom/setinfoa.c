/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom function SetInfoA()
    Lang: English
*/

#include "freedom_intern.h"
#include <string.h>

#include <proto/utility.h>
#include <proto/freedom.h>
#include <proto/dos.h>

#define DEBUG 0
#   include <aros/debug.h>

/*****************************************************************************

    NAME */

	AROS_LH2(ULONG, SetInfoA,

/*  SYNOPSIS */
    	AROS_LHA(struct GlyphEngine *, glyphEngine, A0),
	AROS_LHA(struct TagItem *, tagList, A1),
	
/*  LOCATION */
	struct Library *, FreedomBase, 7, Freedom)

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

    struct TagItem *tstate,*ti;
    LONG    	    data;
    UWORD    	    platform,encoding;
    WORD    	    i;
    char    	    s[300];

    ULONG rc = OTERR_Success;

    tstate = tagList;

    D(bug("\n*** SetInfoA ***\n"));
    
    while ((ti = NextTagItem((const struct TagItem **)&tstate)))
    {
        data = ti->ti_Data;

        switch(ti->ti_Tag)
        {
            case OT_DeviceDPI:
                D(bug("  OT_DeviceDPI xdpi=%d ydpi=%d\n", data >> 16, data & 0xFFFF));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_DeviceDPI_X = data >> 16;
                MYGE(glyphEngine)->gle_GlyphRequest.gr_DeviceDPI_Y = data & 0xFFFF;
                break;

            case OT_GlyphCode:
                D(bug("  OT_GlyphCode %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode = data;
                break;
                
            case OT_GlyphCode2:
                D(bug("  OT_GlyphCode2 %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphCode2 = data;
                break;
                
            case OT_GlyphWidth:
                D(bug("  OT_GlyphWidth %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_GlyphWidth = data;
                break;
                
            case OT_OTagPath:
                D(bug("  OT_OTagPath \"%s\"\n", data));
                strcpy(MYGE(glyphEngine)->gle_OTagName,(char *)data);
                break;
                
            case OT_OTagList:
                D(bug("  --- OT_OTagList --\n"));
                ClearGlyphRequest(MYGE(glyphEngine));
                SetInfoA(glyphEngine, (struct TagItem *)data);
                
                /* kill old font */
                
                if (MYGE(glyphEngine)->gle_Glyph.z)
                {
                    TT_Done_Glyph(MYGE(glyphEngine)->gle_Glyph);
                    MYGE(glyphEngine)->gle_Glyph.z = 0;
                }
                
                if (MYGE(glyphEngine)->gle_Instance.z)
                {
                    TT_Done_Instance(MYGE(glyphEngine)->gle_Instance);
                    MYGE(glyphEngine)->gle_Instance.z = 0;
                }
                
                if (MYGE(glyphEngine)->gle_Face.z)
                {
                    TT_Close_Face(MYGE(glyphEngine)->gle_Face);
                    MYGE(glyphEngine)->gle_Face.z = 0;
                }
                
                MYGE(glyphEngine)->gle_FontOK = FALSE;

                /* try creating new font */
                
                strcpy(s,"fonts:_truetype");
                AddPart(s, MYGE(glyphEngine)->gle_FontName, 299);
                
                if (TT_Open_Face(MYGE(glyphEngine)->gle_Engine,s,&MYGE(glyphEngine)->gle_Face) != 0)
                {
                    rc = OTERR_NoFace;
                    D(bug("OpenFace failed!"));
                    break;
                }
                
                TT_Get_Face_Properties(MYGE(glyphEngine)->gle_Face,&MYGE(glyphEngine)->gle_Face_Properties);
                
                if (TT_New_Instance(MYGE(glyphEngine)->gle_Face,&MYGE(glyphEngine)->gle_Instance) != 0)
                {
                    TT_Close_Face(MYGE(glyphEngine)->gle_Face);
                    MYGE(glyphEngine)->gle_Face.z = 0;

                    rc = OTERR_NoMemory;
                    D(bug("NewInstance failed!"));
                    break;
                }
                
                if (TT_New_Glyph(MYGE(glyphEngine)->gle_Face,&MYGE(glyphEngine)->gle_Glyph) != 0)
                {
                    TT_Close_Face(MYGE(glyphEngine)->gle_Face);
                    MYGE(glyphEngine)->gle_Face.z = 0;

    	    	    /*  TT_Done_Instance(MYGE(glyphEngine)->gle_Instance);
                    MYGE(glyphEngine)->gle_Instance.z=0;*/ /* killed width TT_Close_Face !? */
                    
                    rc = OTERR_NoMemory;
                    D(bug("NewGlyph failed!"));
                    break;
                }
                
                data=MYGE(glyphEngine)->gle_Face_Properties.num_CharMaps;
                
                for (i = 0; i < data; i++ )
                {
                    TT_Get_CharMap_ID(MYGE(glyphEngine)->gle_Face,i,&platform,&encoding);
                    if ( (platform == 3 && encoding == 1 )  ||
                        (platform == 0 && encoding == 0 ) )
                    {
                        TT_Get_CharMap(MYGE(glyphEngine)->gle_Face,i,&MYGE(glyphEngine)->gle_CharMap);
                        break;
                    }
                }

                if (i == data)
                {
                    /* unicode charmap not found */
                    TT_Close_Face(MYGE(glyphEngine)->gle_Face);
                    MYGE(glyphEngine)->gle_Face.z = 0;

    	    	    /* TT_Done_Instance(MYGE(glyphEngine)->gle_Instance);
                    MYGE(glyphEngine)->gle_Instance.z=0;*/ /* killed with TT_Close_Face !? */
                    
                    TT_Done_Glyph(MYGE(glyphEngine)->gle_Glyph);
                    MYGE(glyphEngine)->gle_Glyph.z = 0;
                    
                    rc = OTERR_BadFace;

                    D(bug("Unicode charmap not found!"));
                    break;
                                        
                }
                
    	    	D(bug("Font loaded successfully :)"));
                MYGE(glyphEngine)->gle_FontOK = TRUE;
                
                break;

            case OT_PointHeight:
                D(bug("  OT_PointHeight %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_PointHeight = data;
                break;
                
            case OT_PointSize:
                D(bug("  OT_PointSize %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_PointHeight = data*16;
                break;
                
            case OT_SetFactor:
                D(bug("  OT_SetFactor %d\n", data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_SetFactor = data;
                break;
            
            case (OT_Spec1+1) | OT_Indirect:
                D(bug("  OT_Spec1+1 | OT_Indirect: %s\n", data));
                strcpy(MYGE(glyphEngine)->gle_FontName,(char *)data);
                break;
                
            case OT_StrikeThrough:
                D(bug("  OT_StrikeThrough %d\n", data));
                rc = OTERR_UnknownTag;
                break;
                
            case OT_UnderLined:
                D(bug("  OT_UnderLined %d\n", data));
                rc = OTERR_UnknownTag;
                break;
            
            case OT_RotateSin:
                D(bug("  OT_RotateSin %d (0x%x)\n", data, data));                
                MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateSin = data;
                break;
                
            case OT_RotateCos:
                D(bug("  OT_RotateCos %d (0x%x)\n", data, data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_RotateCos = data;
                break;

            case OT_ShearSin:
                D(bug("  OT_ShearSin %d (0x%x)\n", data, data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearSin = data;
                break;
                
            case OT_ShearCos:
                D(bug("  OT_ShearCos %d (0x%x)\n", data, data));
                MYGE(glyphEngine)->gle_GlyphRequest.gr_ShearCos = data;
                break;

            case OT_EmboldenX:
                D(bug("  OT_EmboldenX %d (0x%x)\n", data, data));
               break;
                
            case OT_EmboldenY:
                D(bug("  OT_EmboldenY %d (0x%x)\n", data, data));
                break;

            default:
                D(bug("  OT_??? tag = 0x%x tagdata = %d (0x%x)\n", ti->ti_Tag, data, data));
                break;
		
        } /* switch(ti->ti_Tag) */
	
        if (rc) break;

    } /* while ((ti=NextTagItem(&tstate))) */

    D(bug("\n#### SetInfoA done retval = %d ###\n", rc));

    return rc;
    
    AROS_LIBFUNC_EXIT

} /* SetInfoA */
