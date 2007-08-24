#include "Private.h"

#define BIG_ENDIAN_MACHINE 1

#ifdef __AROS__
 #include <aros/macros.h>
 #if !AROS_BIG_ENDIAN
  #undef BIG_ENDIAN_MACHINE
  #define BIG_ENDIAN_MACHINE 0
 #endif
#endif

#include <aros/debug.h>

//#define DB_BLIT

///typedef
typedef void (*feelin_func_blit)                (struct FeelinBitMapBlit *fbb, APTR source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h);

struct FeelinBitMapBlit
{
    feelin_func_blit                blit_func;
    
    uint16                          PixelSize;
    uint16                          ColorType;
    uint32                         *ColorArray;
    uint32                          Modulo;
    struct RastPort                *RPort;
};

typedef void (*feelin_func_scale)               (struct FeelinBitMapScale *fbs, APTR source);
typedef void (*feelin_func_scale_y)             (struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y);
typedef void (*feelin_func_scale_x)             (struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 tpy);
typedef void (*feelin_func_transform)           (struct FeelinBitMapScale *fbs, uint8 *source, uint32 *target, uint32 pixels);
typedef uint32 (*feelin_func_get_pixel)         (struct FeelinBitMapScale *fbs, uint32 *s, uint32 x, uint32 y);

struct FeelinBitMapScale
{
    struct FeelinBitMapBlit        *fbb;
    
    uint16                          source_w;
    uint16                          source_h;
    uint32                          source_modulo;
    uint32                          source_modulo_real;
    
    uint16                          target_x;
    uint16                          target_y;
    uint16                          target_w;
    uint16                          target_h;
    uint32                          target_modulo;
    
    uint32                          xfrac;
    uint32                          yfrac;
    uint32                          spy;

    feelin_func_transform           transform_func;
    uint32                         *transform_buffer;
    uint32                         *transform_colors;
    
    feelin_func_scale               scale_func;
    feelin_func_scale_y             scale_y_func;
    feelin_func_scale_x             scale_x_func;
    feelin_func_get_pixel           get_pixel_func;
};
//+

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///bitmap_blit_palette
static void bitmap_blit_palette(struct FeelinBitMapBlit *fbb, uint8 *source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    uint32 target_mod = target_w * sizeof (uint32);
    uint32 *line_buffer = F_New(target_w * sizeof (uint32));

    if (line_buffer != NULL)
    {
        uint32 target_x2 = target_x + target_w;
        uint32 target_y2 = target_y + target_h;

        for ( ; target_y < target_y2 ; target_y++, source_y++)
        {
            uint8 *src = (APTR)((uint32)(source) + source_x * sizeof (uint8) + source_y * fbb->Modulo);
            uint32 *buf = line_buffer;
            uint32 j;

            ReadPixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);

            for (j = target_x ; j < target_x2 ; j++)
            {
                *buf++ = fbb->ColorArray[*src++];
            }

            WritePixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);
        }

        F_Dispose(line_buffer);
    }
}
//+
///bitmap_blit_palette_alpha
static void bitmap_blit_palette_alpha(struct FeelinBitMapBlit *fbb, uint8 *source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    uint32 target_mod = target_w * sizeof (uint32);
    uint32 *line_buffer = F_New(target_w * sizeof (uint32));

    if (line_buffer != NULL)
    {
        uint32 target_x2 = target_x + target_w;
        uint32 target_y2 = target_y + target_h;

        for ( ; target_y < target_y2 ; target_y++, source_y++)
        {
            uint8 *src = (APTR)((uint32)(source) + source_x * sizeof (uint8) + source_y * fbb->Modulo);
            uint32 *buf = line_buffer;
            uint32 j;

    	#if BIG_ENDIAN_MACHINE
            ReadPixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);
    	#else
            ReadPixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_ABGR32);
	#endif
            for (j = target_x ; j < target_x2 ; j++)
            {
                uint32 rc;
                uint32 s_rgba = *buf;
                uint32 d_rgba = fbb->ColorArray[*src++];

    	    #if !BIG_ENDIAN_MACHINE
	    	d_rgba = AROS_BE2LONG(d_rgba);
	    #endif

                int32 a = /*255 -*/ F_PIXEL_A(d_rgba);
                int32 s,d;

                s = F_PIXEL_R(s_rgba);
                d = F_PIXEL_R(d_rgba);

                rc = s + (((d - s) * a) >> 8);
                rc <<= 8;

                s = F_PIXEL_G(s_rgba);
                d = F_PIXEL_G(d_rgba);

                rc |= s + (((d - s) * a) >> 8);
                rc <<= 8;

                s = F_PIXEL_B(s_rgba);
                d = F_PIXEL_B(d_rgba);

                rc |= s + (((d - s) * a) >> 8);
                rc <<= 8;

                *buf++ = rc;
            }

    	#if BIG_ENDIAN_MACHINE
            WritePixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);
	#else
            WritePixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_ABGR32);
	#endif
        }

        F_Dispose(line_buffer);
    }
}
//+
///bitmap_blit_rgb
static void bitmap_blit_rgb(struct FeelinBitMapBlit *fbb, APTR source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    WritePixelArray
    (
        source,
        source_x, source_y,

        fbb->Modulo,
        fbb->RPort,

        target_x, target_y,
        target_w, target_h,

        RECTFMT_RGB
    );
}
//+
///bitmap_blit_rgb_alpha
static void bitmap_blit_rgb_alpha(struct FeelinBitMapBlit *fbb,uint32 *source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    uint32 target_mod = target_w * sizeof (uint32);
    uint32 *line_buffer = F_New(target_mod);

    if (line_buffer != NULL)
    {
        uint32 target_x2 = target_x + target_w;
        uint32 target_y2 = target_y + target_h;

        for ( ; target_y < target_y2 ; target_y++, source_y++)
        {
            uint32 *src = (APTR)((uint32)(source) + source_x * sizeof (uint32) + source_y * fbb->Modulo);
            uint32 *buf = line_buffer;
            uint32 j;

    	#if BIG_ENDIAN_MACHINE
            ReadPixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);
    	#else
            ReadPixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_ABGR32);
	#endif
            for (j = target_x ; j < target_x2 ; j++)
            {
                uint32 rc;
                uint32 s_rgba = *buf;
                uint32 d_rgba = *src++;

    	    #if !BIG_ENDIAN_MACHINE
	    	d_rgba = AROS_BE2LONG(d_rgba);
	    #endif
	    
                int32 a = /*255 -*/ F_PIXEL_A(d_rgba);
                int32 s,d;

                s = F_PIXEL_R(s_rgba);
                d = F_PIXEL_R(d_rgba);

                rc = s + (((d - s) * a) >> 8);
                rc <<= 8;

                s = F_PIXEL_G(s_rgba);
                d = F_PIXEL_G(d_rgba);

                rc |= s + (((d - s) * a) >> 8);
                rc <<= 8;

                s = F_PIXEL_B(s_rgba);
                d = F_PIXEL_B(d_rgba);

                rc |= s + (((d - s) * a) >> 8);
                rc <<= 8;

                *buf++ = rc;
            }

    	#if BIG_ENDIAN_MACHINE
            WritePixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_RGBA);
    	#else
            WritePixelArray(line_buffer, 0,0,target_mod, fbb->RPort, target_x,target_y, target_w,1, RECTFMT_ABGR32);
	#endif	    
        }

        F_Dispose(line_buffer);
    }
}
//+
///bitmap_blit_rgb_0
static void bitmap_blit_rgb_0(struct FeelinBitMapBlit *fbb, APTR source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    WritePixelArray
    (
        source,
        source_x, source_y,

        fbb->Modulo,
        fbb->RPort,

        target_x, target_y,
        target_w, target_h,

        RECTFMT_RGBA
    );
}
//+
///bitmap_blit_bitmap
static void bitmap_blit_bitmap(struct FeelinBitMapBlit *fbb, APTR source, uint16 source_x, uint16 source_y, uint16 target_x, uint16 target_y, uint16 target_w, uint16 target_h)
{
    BltBitMapRastPort
    (
        source,
        source_x, source_y,

        fbb->RPort,

        target_x, target_y,
        target_w, target_h,

        0x0C0
    );
}
//+

///bitmap_blit_tile
static void bitmap_blit_tile
(
    struct FeelinBitMapBlit *fbb,
    
    APTR source,

    uint16 source_x,
    uint16 source_y,
    uint16 source_w,
    uint16 source_h,

    uint16 target_x,
    uint16 target_y,
    uint16 target_w,
    uint16 target_h
)
{

    /* the width/height of the rectangle to blit as the  first  column/row.
    the  width  of  the  first tile is either the rest of the tile right to
    offx or the width of the dest rect, if the rect is narrow */

    int32 first_w = MIN(source_w - source_x, target_w);
    int32 first_h = MIN(source_h - source_y, target_h);

    /* the left/top edge of the second column (if used) */

    int32 second_x = target_x + first_w;
    int32 second_y = target_y + first_h;

    /* the width/height of the second column */

    int32 second_w = MIN(source_x, target_x + target_w - second_x);
    int32 second_h = MIN(source_y, target_y + target_h - second_y);

    /* 'pos' is used as starting position in the "exponential" blit,  while
    'size' is used as bitmap size */

    uint32 pos;
    uint32 size;

    /* the width of the second tile (we want the whole tile to be  source_w
    pixels  wide,  if we use 'source_w - source_x' pixels for the left part
    we'll use 'source_x' for the right part) */

/*** blit the source with offsets (if any) *********************************/

///DB_BLIT
    #ifdef DB_BLIT
    F_Log(0,"01> %3ld : %3ld TO %3ld : %3ld, %3ld x %3ld",
        source_x,source_y, target_x, target_y, first_w, first_h);
    #endif
//+

    fbb->blit_func
    (
        fbb,
 
        source,
        source_x, source_y,

        target_x, target_y,
        first_w, first_h
    );


/*** blit the width offset part (if any) ***********************************/

    if (second_w > 0)
    {

        /* if second_w was 0 or the dest rect was to narrow, we won't  need
        a second column */

///DB_BLIT
    #ifdef DB_BLIT
        F_Log(0,"02> %3ld : %3ld >> %3ld : %3ld, [3m%3ld[0m x %3ld",
            0,source_y, second_x, target_y, second_w, first_h);
    #endif
//+

        fbb->blit_func
        (
            fbb,
 
            source,
            0, source_y,

            second_x, target_y,
            second_w, first_h
        );

    }

/*** blit the height offset part (if any) **********************************/

    if (second_h > 0)
    {
///DB_BLIT
    #ifdef DB_BLIT
        F_Log(0,"03> %3ld : %3ld >> %3ld : %3ld, %3ld x %3ld",
            source_x,0, target_x, second_y, first_w, second_h);
    #endif
//+

        fbb->blit_func
        (
            fbb,
 
            source,
            source_x, 0,

            target_x, second_y,
            first_w, second_h
        );


        if (second_w > 0)
        {
///DB_BLIT
            #ifdef DB_BLIT
            F_Log(0,"04> %3ld : %3ld >> %3ld : %3ld, %3ld x %3ld",
                0,0, second_x, second_y, second_w, second_h);
            #endif
//+

            fbb->blit_func
            (
                fbb,
 
                source,
                0, 0,

                second_x, second_y,
                second_w, second_h
            );
        }
    }

/*** blit the first row ****************************************************/

    for (pos = target_x + source_w, size = MIN(source_w,target_x + target_w - 1 - pos + 1) ; pos <= target_x + target_w - 1 ; )
    {
///DB_BLIT
    #ifdef DB_BLIT
        F_Log(0,"!5> %3ld : %3ld >> %3ld : %3ld, %3ld x %3ld",
            target_x,target_y, pos, target_y, size, MIN(source_h, target_h));
    #endif
//+

        ClipBlit
        (
            fbb->RPort,
            
            target_x,
            target_y,
            
            fbb->RPort,
            
            pos, target_y,
            size, MIN(source_h, target_h),

            0x0C0
        );

        pos += size;

        size = MIN(size << 1, target_x + target_w - 1 - pos + 1);
    }

/*** blit the first row down several times *********************************/

    for (pos = target_y + source_h, size = MIN(source_h, target_y + target_h - 1 - pos + 1) ; pos <= target_y + target_h - 1 ; )
    {
///DB_BLIT
    #ifdef DB_BLIT
        F_Log(0,"!6> %3ld : %3ld >> %3ld : %3ld, %3ld x %3ld",
            target_x,target_y, target_x, pos, target_w, size);
    #endif
//+

        ClipBlit
        (
            fbb->RPort,

            target_x,
            target_y,

            fbb->RPort,

            target_x, pos,
            target_w, size,

            0x0C0
        );

        pos += size;
        size = MIN(size << 1, target_y + target_h - 1 - pos + 1);
    }
}
//+

/*** scale *****************************************************************/

///bitmap_get_pixel_nearest
static uint32 bitmap_get_pixel_nearest(struct FeelinBitMapScale *fbs, uint32 *s, uint32 x, uint32 y)
{
    return *s;
}
//+
///bitmap_get_pixel_bilinear
/*
 * v00|     |v01
 *   -+-----+-
 *    |     |      For 'x' we get the 4 corners with their intensity depending on the 'x' placement.
 *    |     |      We sum up the whole then divide. The resulting pixel has the correct value of
 *    |   x |      the 4 pixels surrounding it.
 *    |     |
 *   -+-----+-
 * v10|     |v11
 */
static uint32 bitmap_get_pixel_bilinear(struct FeelinBitMapScale *fbs, uint32 *s, uint32 x, uint32 y)
{
    uint32 v00, v10, v01, v11; /* ul, ll, ur, lr */
    uint32 v00m, v10m, v01m, v11m; /* multipliers */
    uint32 s00, s10, s01, s11;
    uint32 d;
    uint32 advx, advy;

    /* limit borders */

    if (x + 1 >= fbs->source_w)
    {
        advx = 0;
    }
    else
    {
        advx = sizeof (uint32);
    }

    if (y + 1 >= fbs->source_h)
    {
        advy = 0;
    }
    else
    {
        advy = fbs->source_modulo;
    }

    /* fetch pixels */

    s00 = *s;
    #if 0
    s01 = *(s + advx);
    s10 = *(s + advy);
    s11 = *(s + advy + advx);
    #else
    s01 = *((uint32 *)((uint32)(s) + advx));
    s10 = *((uint32 *)((uint32)(s) + advy));
    s11 = *((uint32 *)((uint32)(s) + advy + advx));
    #endif

    /* pre-multiply */

    v00m = (GSIMUL - fbs->xfrac) * (GSIMUL - fbs->yfrac);
    v01m = fbs->xfrac * (GSIMUL - fbs->yfrac);
    v10m = (GSIMUL - fbs->xfrac) * fbs->yfrac;
    v11m = fbs->xfrac * fbs->yfrac;

    /* R */

    v00 = v00m * F_PIXEL_R(s00);
    v01 = v01m * F_PIXEL_R(s01);
    v10 = v10m * F_PIXEL_R(s10);
    v11 = v11m * F_PIXEL_R(s11);
    d = ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 24;

    /* G */

    v00 = v00m * F_PIXEL_G(s00);
    v01 = v01m * F_PIXEL_G(s01);
    v10 = v10m * F_PIXEL_G(s10);
    v11 = v11m * F_PIXEL_G(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 16;

    /* B */

    v00 = v00m * F_PIXEL_B(s00);
    v01 = v01m * F_PIXEL_B(s01);
    v10 = v10m * F_PIXEL_B(s10);
    v11 = v11m * F_PIXEL_B(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 8;

    /* A */

    v00 = v00m * F_PIXEL_A(s00);
    v01 = v01m * F_PIXEL_A(s01);
    v10 = v10m * F_PIXEL_A(s10);
    v11 = v11m * F_PIXEL_A(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) & 0xff;

//    F_Log(0,"0x%08lx >> 0x%08lx",s00, d);

    return (d);
}
//+
///bitmap_get_pixel_average
/*
 *    |   |
 * v00|v01|v02
 * ---+---+---
 * v10|v11|v12  For an 'x' in v11, we get the 4x4 grid surrounding then
 * ---+---+---  use the average of them.
 * v20|v21|v22
 * ---+---+---
 *    |   |
 */
static uint32 bitmap_get_pixel_average(struct FeelinBitMapScale *fbs, uint32 *s, uint32 x, uint32 y)
{
    uint32 v00, v01, v02;
    uint32 v10, v11, v12;
    uint32 v20, v21, v22;

    uint32 s00, s01, s02;
    uint32 s10, s11, s12;
    uint32 s20, s21, s22;

    uint32 d;
    uint32 bckx;
    uint32 bcky;

    /* limit borders */

    if (((int32)(x) - 1 > 0) && ((int32)(x) + 1 < fbs->source_w))
    {
        bckx = sizeof (uint32);
    }
    else
    {
        bckx = 0;
    }

    if (((int32)(y) - 1 > 0) && ((int32)(y) + 1 < fbs->source_h))
    {
        bcky = fbs->source_modulo;
    }
    else
    {
        bcky = 0;
    }
 
    /* fetch pixels */

    s00 = *((uint32 *)((uint32)(s) - bcky - bckx));
    s01 = *((uint32 *)((uint32)(s) - bcky));
    s02 = *((uint32 *)((uint32)(s) - bcky + 1 * sizeof (uint32)));

    s10 = *((uint32 *)((uint32)(s) - bckx));
    s11 = *(s);
    s12 = *((uint32 *)((uint32)(s) + 1 * sizeof (uint32)));

    s20 = *((uint32 *)((uint32)(s) + bcky - bckx));
    s21 = *((uint32 *)((uint32)(s) + bcky));
    s22 = *((uint32 *)((uint32)(s) + bcky + 1 * sizeof (uint32)));

    /* R */
    v00 = F_PIXEL_R(s00);
    v01 = F_PIXEL_R(s01);
    v02 = F_PIXEL_R(s02);
    v10 = F_PIXEL_R(s10);
    v11 = F_PIXEL_R(s11);
    v12 = F_PIXEL_R(s12);
    v20 = F_PIXEL_R(s20);
    v21 = F_PIXEL_R(s21);
    v22 = F_PIXEL_R(s22);
    d = ((v00 + v01 + v02 + v10 + v11 + v12 + v20 + v21 + v22) / 9) << 24;

    /* G */
    v00 = F_PIXEL_G(s00);
    v01 = F_PIXEL_G(s01);
    v02 = F_PIXEL_G(s02);
    v10 = F_PIXEL_G(s10);
    v11 = F_PIXEL_G(s11);
    v12 = F_PIXEL_G(s12);
    v20 = F_PIXEL_G(s20);
    v21 = F_PIXEL_G(s21);
    v22 = F_PIXEL_G(s22);
    d |= ((v00 + v01 + v02 + v10 + v11 + v12 + v20 + v21 + v22) / 9) << 16;

    /* B */
    v00 = F_PIXEL_B(s00);
    v01 = F_PIXEL_B(s01);
    v02 = F_PIXEL_B(s02);
    v10 = F_PIXEL_B(s10);
    v11 = F_PIXEL_B(s11);
    v12 = F_PIXEL_B(s12);
    v20 = F_PIXEL_B(s20);
    v21 = F_PIXEL_B(s21);
    v22 = F_PIXEL_B(s22);
    d |= ((v00 + v01 + v02 + v10 + v11 + v12 + v20 + v21 + v22) / 9) << 8;

    /* A */
    v00 = F_PIXEL_A(s00);
    v01 = F_PIXEL_A(s01);
    v02 = F_PIXEL_A(s02);
    v10 = F_PIXEL_A(s10);
    v11 = F_PIXEL_A(s11);
    v12 = F_PIXEL_A(s12);
    v20 = F_PIXEL_A(s20);
    v21 = F_PIXEL_A(s21);
    v22 = F_PIXEL_A(s22);
    d |= ((v00 + v01 + v02 + v10 + v11 + v12 + v20 + v21 + v22) / 9) & 0xff;

    return (d);
}
//+

///bitmap_x_scale
static void bitmap_x_scale(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 tpy)
{
    uint32 spx = fbs->source_w * GSIMUL / fbs->target_w;
    uint32 tpx;
    uint32 j;

    for (j = 0 ; j < fbs->target_w ; j++)
    {
        tpx = (spx * j);
        fbs->xfrac = tpx & (GSIMUL - 1);
        tpx /= GSIMUL;

//        F_Log(0,"%ld >> %ld", j, tpx);

        *target++ = fbs->get_pixel_func(fbs, source + tpx, tpx, tpy);
    }
}
//+
///bitmap_y_scale
static void bitmap_y_scale(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y)
{
    uint32 spy = (fbs->source_h * GSIMUL) / fbs->target_h;
    uint32 tpy;

    tpy = (spy * y);
    fbs->yfrac = tpy & (GSIMUL - 1);
    tpy /= GSIMUL;

    source = (uint32 *)((uint32)(source) + tpy * fbs->source_modulo);

    fbs->scale_x_func
    (
        fbs,
        source,
        target,
        tpy
    );
}
//+
///bitmap_y_scale_transform
static void bitmap_y_scale_transform(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y)
{
    uint32 tpy = fbs->spy * y;
    uint32 *sp;

    fbs->yfrac = tpy & (GSIMUL - 1);
    tpy /= GSIMUL;

    sp = (APTR)((uint32)(source) + tpy * fbs->source_modulo_real);

    fbs->transform_func(fbs, (APTR)((uint32)(sp) - fbs->source_modulo_real), fbs->transform_buffer, fbs->source_w * 3);

    fbs->scale_x_func
    (
        fbs,
        (APTR)((uint32)(fbs->transform_buffer) + fbs->source_modulo),
        target,
        tpy
    );

}
//+
///bitmap_scale_draw
static void bitmap_scale_draw(struct FeelinBitMapScale *fbs, APTR source)
{
    uint32 *target = F_New(fbs->target_modulo);

    if (target)
    {
        struct FeelinBitMapBlit fbb;
        
        uint32 i;

        if (FF_COLOR_TYPE_ALPHA & fbs->fbb->ColorType)
        {
            fbb.blit_func = bitmap_blit_rgb_alpha;
        }
        else
        {
            fbb.blit_func = bitmap_blit_rgb_0;
        }

        fbb.Modulo = fbs->target_modulo;
        fbb.RPort = fbs->fbb->RPort;

        for (i = 0 ; i < fbs->target_h ; i++)
        {
            fbs->scale_y_func(fbs, source, target, i);

            fbb.blit_func
            (
                &fbb,
                
                target,

                0,
                0,

                fbs->target_x,
                fbs->target_y + i,
                fbs->target_w,
                1
            );
        }

        F_Dispose(target);
    }
}
//+

///bitmap_transform_rgb
static void bitmap_transform_rgb(struct FeelinBitMapScale *fbs, uint8 *source, uint32 *target, uint32 pixels)
{
    uint32 stop = (uint32)(source) + ((pixels - 1) * sizeof (uint8) * 3);

//    uint32 n=1;

    while ((uint32)(source) < stop)
    {
        uint32 rgba;

        rgba = *source++;
        rgba <<= 8;
        rgba |= *source++;
        rgba <<= 8;
        rgba |= *source++;
        rgba <<= 8;
        rgba |= 0xFF;

        *target++ = rgba;

//        n++;
    }

//    F_Log(0,"%ld pixels transformed (number %ld)",n, number);
}
//+
///bitmap_transform_palette
static void bitmap_transform_palette(struct FeelinBitMapScale *fbs, uint8 *source, uint32 *target, uint32 pixels)
{
    uint32 stop = (uint32)(source) + (pixels * sizeof (uint8));

    while ((uint32)(source) < stop)
    {
        *target++ = fbs->transform_colors[*source++];
    }
}
//+

///bitmap_blit_scale
static void bitmap_blit_scale
(
    struct FeelinBitMapBlit *fbb,
    
    APTR source,

    uint16 source_x,
    uint16 source_y,
    uint16 source_w,
    uint16 source_h,

    uint16 target_x,
    uint16 target_y,
    uint16 target_w,
    uint16 target_h,
    
    uint32 filter
)
{
    #if 1
    struct FeelinBitMapScale fbs;
    
    fbs.fbb = fbb;
    
    fbs.source_w = source_w;
    fbs.source_h = source_h;
    fbs.source_modulo = fbb->Modulo;
    fbs.source_modulo_real = fbb->Modulo;

    fbs.target_x = target_x;
    fbs.target_y = target_y;
    fbs.target_w = target_w;
    fbs.target_h = target_h;
    fbs.target_modulo = target_w * sizeof (uint32);

    fbs.xfrac = 0;
    fbs.yfrac = 0;
    fbs.spy = (fbs.source_h * GSIMUL) / fbs.target_h;

    fbs.scale_func = bitmap_scale_draw;
    fbs.scale_y_func = bitmap_y_scale;
    fbs.scale_x_func = bitmap_x_scale;
    
    fbs.transform_func   = NULL;
    fbs.transform_buffer = NULL;
    fbs.transform_colors = fbb->ColorArray;

    switch (filter)
    {
        case FV_BitMap_ScaleFilter_Bilinear:
        {
            fbs.get_pixel_func = bitmap_get_pixel_bilinear;
        }
        break;
                
        case FV_BitMap_ScaleFilter_Average:
        {
            fbs.get_pixel_func = bitmap_get_pixel_average;
        }
        break;
             
        default:
        {
            fbs.get_pixel_func = bitmap_get_pixel_nearest;
        }
        break;
    }

/*
        F_Log(0,"source (%ld : %ld, %ld x %ld) target (%ld : %ld, %ld x %ld)",
            source_x, source_y, source_w, source_h,
            target_x, target_y, target_w, target_h);
*/
    source = (APTR)((uint32)(source) + source_y * fbb->Modulo + source_x * fbb->PixelSize);
    // + source_x * sizeof(uint32));

    if (fbb->PixelSize != 4)
    {
        fbs.source_modulo = source_w * sizeof (uint32);
        fbs.transform_buffer = F_New(fbs.source_modulo * 3);
        fbs.scale_y_func = bitmap_y_scale_transform;

        if (fbs.transform_buffer)
        {
            switch (fbb->PixelSize)
            {
                case 1: //FV_PIXEL_TYPE_PALETTE:
                {
                    fbs.transform_func = bitmap_transform_palette;

//                    source = (APTR)((uint32)(source) + source_x * sizeof (uint8));
                }
                break;

                case 3: //FV_PIXEL_TYPE_RGB:
                {
                    fbs.transform_func = bitmap_transform_rgb;

//                    source = (APTR)((uint32)(source) + source_x * sizeof (uint8) * 3);
                }
                break;

                default:
                {
                    goto __done;
                }
            }
        }
        else goto __done;
    }
    /*
    else
    {
        source = (APTR)((uint32)(source) + source_x * sizeof(uint32));
    }
    */

    if (fbs.scale_func && fbs.scale_y_func && fbs.scale_x_func)
    {
        fbs.scale_func(&fbs, source);
    }

__done:

    F_Dispose(fbs.transform_buffer);
    
    #endif
}
//+

#if 0
///bitmap_blit_frame
void bitmap_blit_frame(FClass *Class, FObject Obj, struct FS_BitMap_Blit *Msg)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = (struct TagItem *) &Msg->Tag1, item;

    uint16 source_w = LOD->source.Width;
    uint16 source_h = LOD->source.Height;
    
    uint16 target_x = Msg->TargetBox->x;
    uint16 target_y = Msg->TargetBox->y;
    uint16 target_w = Msg->TargetBox->w;
    uint16 target_h = Msg->TargetBox->h;

    FInner *margins = NULL;
    
    #if 1
    uint16 x,y,w,h;
    #endif
    
    while  (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_BitMap_Margins: margins = (FInner *) item.ti_Data; break;
    }
                        
    if (margins == NULL)
    {
        F_Log(0,"You need to defined the FA_BitMap_Margins attribute while using the 'Frame' blit mode");
        
        return;
    }

    #if 1
///
    bitmap_blit_select
    (
        LOD->source.PixelArray,

        0,
        0,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x,
        target_y,
        margins->l,
        margins->t,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );

    bitmap_blit_select
    (
        LOD->source.PixelArray,

        source_w - margins->r,
        0,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x + target_w - margins->r,
        target_y,
        margins->r,
        margins->t,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );

    bitmap_blit_select
    (
        LOD->source.PixelArray,

        0,
        source_h - margins->b,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x,
        target_y + target_h - margins->b,
        margins->l,
        margins->b,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );
                
    bitmap_blit_select
    (
        LOD->source.PixelArray,

        source_w - margins->r,
        source_h - margins->b,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x + target_w - margins->r,
        target_y + target_h - margins->b,
        margins->r,
        margins->b,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );

    x = target_x + margins->l;
    y = target_y;
    w = target_w - margins->l - margins->r;
    h = margins->t;
    
    while (w)
    {
        uint32 draw_w = MIN(source_w - margins->l - margins->r, w);

        bitmap_blit_select
        (
            LOD->source.PixelArray,
            margins->l, 0,

            LOD->source.PixelArrayMod,

            Msg->rp,

            x, y,
            draw_w, MIN(source_h, h),

            LOD->source.PixelType,
            LOD->source.ColorArray
        );

        x += draw_w;
        w -= draw_w;
    }
    
    x = target_x + margins->l;
    y = target_y + target_h - margins->b;
    w = target_w - margins->l - margins->r;
    h = margins->b;

    while (w)
    {
        uint32 draw_w = MIN(source_w - margins->l - margins->r, w);

        bitmap_blit_select
        (
            LOD->source.PixelArray,
            margins->l, source_h - margins->b,

            LOD->source.PixelArrayMod,

            Msg->rp,

            x, y,
            draw_w, MIN(source_h, h),

            LOD->source.PixelType,
            LOD->source.ColorArray
        );

        x += draw_w;
        w -= draw_w;
    }
                            
    x = target_x;
    y = target_y + margins->t;
    w = margins->l;
    h = target_h - margins->t - margins->b;

    while (h)
    {
        uint32 draw_h = MIN(source_h - margins->t - margins->b, h);

        bitmap_blit_select
        (
            LOD->source.PixelArray,
            0, margins->t,

            LOD->source.PixelArrayMod,

            Msg->rp,

            x, y,
            w, draw_h,

            LOD->source.PixelType,
            LOD->source.ColorArray
        );

        y += draw_h;
        h -= draw_h;
    }

    x = target_x + target_w - margins->r;
    y = target_y + margins->t;
    w = margins->r;
    h = target_h - margins->t - margins->b;

    while (h)
    {
        uint32 draw_h = MIN(source_h - margins->t - margins->b, h);

        bitmap_blit_select
        (
            LOD->source.PixelArray,
            source_w - margins->r, margins->t,

            LOD->source.PixelArrayMod,

            Msg->rp,

            x, y,
            w, draw_h,

            LOD->source.PixelType,
            LOD->source.ColorArray
        );

        y += draw_h;
        h -= draw_h;
    }
//+
    #endif
  
    #if 1
    bitmap_blit_scale
    (
        LOD->source.PixelArray,

        margins->l,
        margins->t,
        source_w - margins->l - margins->r,
        source_h - margins->t - margins->b,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x + margins->l,
        target_y + margins->t,
        target_w - margins->l - margins->r,
        target_h - margins->t - margins->b,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );
    #else
    bitmap_blit_select
    (
        LOD->source.PixelArray,

        margins->l,
        margins->t,

        LOD->source.PixelArrayMod,

        Msg->rp,

        target_x + margins->l,
        target_y + margins->t,
        source_w - margins->l - margins->r,
        source_h - margins->t - margins->b,

        LOD->source.PixelType,
        LOD->source.ColorArray
    );
    #endif
}
//+
#endif

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///BitMap_Blit
F_METHODM(void,BitMap_Blit,FS_BitMap_Blit)
{
    #if 1
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = (struct TagItem *) &Msg->Tag1, item;
    
    APTR source;
    uint16 source_w;
    uint16 source_h;
    uint16 source_x = Msg->SourceX;
    uint16 source_y = Msg->SourceY;
    uint32 blit_mode=FV_BitMap_BlitMode_Copy;
    uint32 scale_filter = FV_BitMap_ScaleFilter_Nearest;

    struct FeelinBitMapBlit fbb;

    fbb.blit_func = NULL;
    fbb.RPort = Msg->rp;
    
    #if 0
    if (LOD->rendered)
    {
        source = LOD->rendered;
        source_w = LOD->rendered_w;
        source_h = LOD->rendered_h;
        modulo = source_w * sizeof (uint32);
        type = (FF_BITMAP_RENDERED_BITMAP & LOD->flags) ? FV_COLOR_TYPE_BITMAP : LOD->rendered_pixel_type;
        
//        F_Log(0,"pixeltype1 (%ld)",pixel_type);
    }
    else
    #endif
    {
        source = LOD->source.PixelArray;
        source_w = LOD->source.Width;
        source_h = LOD->source.Height;

        fbb.PixelSize = LOD->source.PixelSize;
        fbb.ColorType = LOD->source.ColorType;
        fbb.ColorArray = LOD->source.ColorArray;
        fbb.Modulo = LOD->source.PixelArrayMod;

//        F_Log(0,"pixeltype2 (%ld)",pixel_type);
    }

//    F_Log(0,"source (0x%08lx)(%ld x %ld)",source, source_w, source_h);

    switch (fbb.PixelSize)
    {
        case 1:
        {
            if (FF_BITMAP_RENDERED_BITMAP & LOD->flags)
            {
                fbb.blit_func = bitmap_blit_bitmap;
            }
            else if (fbb.ColorType == FV_COLOR_TYPE_PALETTE)
            {
                fbb.blit_func = bitmap_blit_palette;
            }
            else if (fbb.ColorType == FV_COLOR_TYPE_PALETTE_ALPHA)
            {
                fbb.blit_func = bitmap_blit_palette_alpha;
            }
        }
        break;

        case 3:
        {
            fbb.blit_func = bitmap_blit_rgb;
        }
        break;

        case 4:
        {
            if (FF_COLOR_TYPE_ALPHA & fbb.ColorType)
            {
                fbb.blit_func = bitmap_blit_rgb_alpha;
            }
            else
            {
                fbb.blit_func = bitmap_blit_rgb_0;
            }
        }
        break;

        default:
        {
            F_Log(0,"PixelSize %ld is not supported", fbb.PixelSize);
            
            return;
        }
    }
    
    if (fbb.blit_func == NULL)
    {
        F_Log(0,"No suitable function to blit bitmap");
        
        return;
    }
    
    
    
    
    if (((0xFF000000 & (uint32)(Tags)) == DYNA_ATTR) || TypeOfMem((APTR)(Msg->Tag1)))
    {
        while  (F_DynamicNTI(&Tags, &item, Class))
        switch (item.ti_Tag)
        {
/*
            case FA_BitMap_Width:
            {
                source_w = MIN(source_w, item.ti_Data);
            }
            break;
            
            case FA_BitMap_Height:
            {
                source_h = MIN(source_h, item.ti_Data);
            }
            break;
*/
            case FA_BitMap_BlitMode:
            {
                blit_mode = item.ti_Data;
            }
            break;
        
            case FA_BitMap_ScaleFilter:
            {
                scale_filter = item.ti_Data;
            }
            break;
        }
    }
    else
    {
        F_Log(0,"BAD TAGS 0x%08lx: RPort (0x%08lx) Source (%ld : %ld) Target (%ld : %ld, %ld x %ld)",
            Msg->Tag1, Msg->rp, Msg->SourceX, Msg->SourceY,
            Msg->TargetBox->x, Msg->TargetBox->y, Msg->TargetBox->w, Msg->TargetBox->h);
        
        return;
    }
/*
    if (source_x + 1 > source_w)
    {
        source_x = 0;
    }

    if (source_y + 1 > source_h)
    {
        source_y = 0;
    }
*/

    switch (blit_mode)
    {
        case FV_BitMap_BlitMode_Copy:
        {
            fbb.blit_func
            (
                &fbb,
 
                source,
                source_x, source_y,

                Msg->TargetBox->x, Msg->TargetBox->y,
                MIN(source_w - source_x, Msg->TargetBox->w), MIN(source_h - source_y, Msg->TargetBox->h)
            );
        }
        break;
        
        case FV_BitMap_BlitMode_Tile:
        {
            bitmap_blit_tile
            (
                &fbb,

                source,
                source_x, source_y,
                source_w, source_h,

                Msg->TargetBox->x, Msg->TargetBox->y,
                Msg->TargetBox->w, Msg->TargetBox->h
            );
        }
        break;
         
        #if 1
        case FV_BitMap_BlitMode_Scale:
        {
            bitmap_blit_scale
            (
                &fbb,
                
                source,
                source_x, source_y,
                source_w - source_x, source_h - source_y,

                Msg->TargetBox->x, Msg->TargetBox->y,
                Msg->TargetBox->w, Msg->TargetBox->h,
                
                scale_filter
            );
        }
        break;
        #endif
         
        #if 0
        case FV_BitMap_BlitMode_Frame:
        {
            bitmap_blit_frame(Class, Obj, Msg);
        }
        break;
        #endif
        
        default:
        {
            F_Log(0,"%ld is not a valid blit mode",blit_mode);
        }
        break;
    }

    #else
    F_Log(0,"disabled");
    #endif
}
//+

