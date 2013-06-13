#include <aros/debug.h>
#include <clib/macros.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <proto/graphics.h>
#include <proto/oop.h>

#include "gfxfuncsupport.h"

/****************************************************************************************/

void hidd2buf_fast(struct BitMap *hidd_bm, LONG x_src , LONG y_src, APTR dest_info,
    	    	   LONG x_dest, LONG y_dest, ULONG xsize, ULONG ysize, VOID (*putbuf_hook)(),
		   struct IntCGFXBase *CyberGfxBase)
{

    ULONG tocopy_w, tocopy_h;
    
    LONG pixels_left_to_process = xsize * ysize;
    ULONG current_x, current_y, next_x, next_y;

    OOP_Object *bm_obj;

    next_x = 0;
    next_y = 0;
    
    bm_obj = HIDD_BM_OBJ(hidd_bm);
	
    LOCK_PIXBUF    

    while (pixels_left_to_process)
    {
	
	current_x = next_x;
	current_y = next_y;
	
	if (NUMPIX < xsize)
	{
	   /* buffer cant hold a single horizontal line, and must 
	      divide each line into copies */
	    tocopy_w = xsize - current_x;
	    if (tocopy_w > NUMPIX)
	    {
	        /* Not quite finished with current horizontal pixel line */
	    	tocopy_w = NUMPIX;
		next_x += NUMPIX;
	    }
	    else
	    {	/* Start at a new line */
	    
	    	next_x = 0;
		next_y ++;
	    }
	    tocopy_h = 1;
	    
    	}
    	else
    	{
	    tocopy_h = MIN(NUMPIX / xsize, ysize - current_y);
	    tocopy_w = xsize;

	    next_x = 0;
	    next_y += tocopy_h;
	    
    	}
	
	
	/* Get some more pixels from the HIDD */
	HIDD_BM_GetImage(bm_obj
		, (UBYTE *)CyberGfxBase->pixel_buf
		, tocopy_w
		, x_src + current_x
		, y_src + current_y
		, tocopy_w, tocopy_h
		, vHidd_StdPixFmt_Native32);


	/*  Write pixels to the destination */
	putbuf_hook(dest_info
		, current_x + x_src
		, current_y + y_src
		, current_x + x_dest
		, current_y + y_dest
		, tocopy_w, tocopy_h
		, (HIDDT_Pixel *)CyberGfxBase->pixel_buf
		, bm_obj
		, HIDD_BM_PIXTAB(hidd_bm)
	);
	
	pixels_left_to_process -= (tocopy_w * tocopy_h);

    }

    ULOCK_PIXBUF

    return;
    
}

/****************************************************************************************/

BYTE hidd2cyber_pixfmt[] = {
    -1,
    -1,
    -1,
    PIXFMT_RGB24,
    PIXFMT_BGR24,
    PIXFMT_RGB16,
    PIXFMT_RGB16PC,
    PIXFMT_BGR16,
    PIXFMT_BGR16PC,
    PIXFMT_RGB15,
    PIXFMT_RGB15PC,
    PIXFMT_BGR15,
    PIXFMT_BGR15PC,
    PIXFMT_ARGB32,
    PIXFMT_BGRA32,
    PIXFMT_RGBA32,
    -1,
    PIXFMT_ARGB32,
    PIXFMT_BGRA32,
    PIXFMT_RGBA32,
    -1,
    PIXFMT_LUT8,
    -1
};

static const UBYTE rectfmt_bytes_per_pixel[] =
{
    3,    /* RECTFMT_RGB */
    4,    /* RECTFMT_RGBA */
    4,    /* RECTFMT_ARGB */
    1,    /* RECTFMT_LUT8 */
    1     /* RECTFMT_GREY8 */
};

static const UBYTE ext_rectfmt_bytes_per_pixel[] =
{
    2,    /* RECTFMT_RGB15 */
    2,    /* RECTFMT_BGR15 */
    2,    /* RECTFMT_RGB15PC */
    2,    /* RECTFMT_BGR15PC */
    2,    /* RECTFMT_RGB16 */
    2,    /* RECTFMT_BGR16 */
    2,    /* RECTFMT_RGB16PC */
    2,    /* RECTFMT_BGR16PC */
    0,
    3,    /* RECTFMT_BGR24 */
    0,
    4,    /* RECTFMT_BGRA32 */
    0,
    4,    /* RECTFMT_ABGR32 */
    4,    /* RECTFMT_0RGB32 */
    4,    /* RECTFMT_BGR032 */
    4,    /* RECTFMT_RGB032 */
    4     /* RECTFMT_0BGR32 */
};

static const HIDDT_StdPixFmt hidd_rectfmt[] =
{
    vHidd_StdPixFmt_RGB24,
    vHidd_StdPixFmt_RGBA32,
    vHidd_StdPixFmt_ARGB32,
    0,
    0,
    vHidd_StdPixFmt_Native
};

static const HIDDT_StdPixFmt ext_hidd_rectfmt[] =
{
    vHidd_StdPixFmt_RGB15,
    vHidd_StdPixFmt_BGR15,
    vHidd_StdPixFmt_RGB15_LE,
    vHidd_StdPixFmt_BGR15_LE,
    vHidd_StdPixFmt_RGB16,
    vHidd_StdPixFmt_BGR16,
    vHidd_StdPixFmt_RGB16_LE,
    vHidd_StdPixFmt_BGR16_LE,
    0,
    vHidd_StdPixFmt_BGR24,
    0,
    vHidd_StdPixFmt_BGRA32,
    0,
    vHidd_StdPixFmt_ABGR32,
    vHidd_StdPixFmt_0RGB32,
    vHidd_StdPixFmt_BGR032,
    vHidd_StdPixFmt_RGB032,
    vHidd_StdPixFmt_0BGR32
};

UBYTE GetRectFmtBytesPerPixel(UBYTE rectfmt, struct RastPort *rp,
    struct Library *CyberGfxBase)
{
    UBYTE result;
    OOP_Object *pf = 0;
    IPTR oop_result;

    if (rectfmt == RECTFMT_RAW)
    {
    	OOP_GetAttr(HIDD_BM_OBJ(rp->BitMap), aHidd_BitMap_PixFmt, (IPTR *)&pf);
        OOP_GetAttr(pf, aHidd_PixFmt_BytesPerPixel, &oop_result);
        result = oop_result;
    }
    else if (rectfmt < RECTFMT_RGB15)
        result = rectfmt_bytes_per_pixel[rectfmt];
    else
        result = ext_rectfmt_bytes_per_pixel[rectfmt - RECTFMT_RGB15];

    return result;
}

HIDDT_StdPixFmt GetHIDDRectFmt(UBYTE rectfmt, struct RastPort *rp,
    struct Library *CyberGfxBase)
{
    HIDDT_StdPixFmt result;

    if (rectfmt < RECTFMT_RGB15)
        result = hidd_rectfmt[rectfmt];
    else
        result = ext_hidd_rectfmt[rectfmt - RECTFMT_RGB15];

    return result;
}

