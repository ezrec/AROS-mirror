#include "Private.h"

///typedefs

typedef uint32 (*FPixelFunc)                    (struct FeelinBitMapRender *fbr, uint32 *source, uint32 x, uint32 y);
typedef void (*FLineYFunc)                      (struct FeelinBitMapRender *fbr, uint32 *source, APTR target);
typedef void (*FLineXFunc)                      (struct FeelinBitMapRender *fbr, uint32 *source, APTR target, uint32 tpy);
typedef void (*feelin_pixel_transform_func)     (struct FeelinBitMapRender *fbr, uint8 *source, uint32 number);

typedef struct FeelinBitMapRender
{
    uint32                          smod;
    uint32                          source_w;
    uint32                          source_h;
    uint16                          xfrac;
    uint16                          yfrac;

    uint32                          target_w;
    uint32                          target_h;

    uint32                         *pixel_transform_buffer;
    feelin_pixel_transform_func     pixel_transform_func;
 
    FPixelFunc                      code_render_pixel;
    FLineYFunc                      code_render_y;
    FLineXFunc                      code_render_x;

    uint32                         *color_array;
 
    /* the following members are used for remaping */

    struct RastPort                *rp1;
    struct RastPort                *rp2;
    uint8                          *line_buffer;
    FObject                         palette;
    uint32                         *colors;
    uint32                         *pens;
    uint32                          numpens;
    uint32                          id_Best;
}
FBRender;

#define PIXEL_A(x) ((x & 0xFF000000) >> 24)
#define PIXEL_R(x) ((x & 0x00FF0000) >> 16)
#define PIXEL_G(x) ((x & 0x0000FF00) >>  8)
#define PIXEL_B(x)  (x & 0x000000FF)

#define GSIMUL 4096
#define FV_PICTURE_AVERAGE_LEVEL                    3
//+

///pixel_transform_rgb
void pixel_transform_rgb(FBRender *fbr, uint8 *source, uint32 number)
{
    uint32 stop = (uint32)(source) + ((number - 1) * sizeof (uint8) * 3);
    uint32 *buf = fbr->pixel_transform_buffer;

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

        *buf++ = rgba;

//        n++;
    }

//    F_Log(0,"%ld pixels transformed (number %ld)",n, number);
}
//+
///pixel_transform_palette
void pixel_transform_palette(FBRender *fbr, uint8 *source, uint32 number)
{
    uint32 stop = (uint32)(source) + (number * sizeof (uint8));
    uint32 *buf = fbr->pixel_transform_buffer;

    while ((uint32)(source) < stop)
    {
        *buf++ = fbr->color_array[*source++];
    }
}
//+

#if 0
///pixels_scale_nearest
static uint32 pixels_scale_nearest(FBRender *fbr, uint32 *s, uint32 x, uint32 y)
{
    return *s;
}
//+
///pixels_scale_average
/*
 *    |   |   |
 * v00|v01|v02|v03
 * ---+---+---+---
 * v10|v11|v12|v13  For an 'x' in v11, we get the 4x4 grid surrounding then
 * ---+---+---+---  use the average of them.
 * v20|v21|v22|v23
 * ---+---+---+---
 * v30|v31|v32|v33
 *    |   |   |
 */
static uint32 pixels_scale_average(FBRender *fbr, uint32 *s, uint32 x, uint32 y)
{
    uint32 v00, v01, v02, v03;
    uint32 v10, v11, v12, v13;
    uint32 v20, v21, v22, v23;
    uint32 v30, v31, v32, v33;

    uint32 s00, s01, s02, s03;
    uint32 s10, s11, s12, s13;
    uint32 s20, s21, s22, s23;
    uint32 s30, s31, s32, s33;

    uint32 d;
    uint32 bckx;
    uint32 bcky;

    /* limit borders */
    
    if ((int32)(x) - 1 < 0)
    {
        bckx = 0;
    }
    else
    {
        bckx = sizeof (uint32);
    }

    if ((int32)(y) - 1 < 0)
    {
        bcky = 0;
    }
    else
    {
        bcky = fbr->smod;
    }

    /* fetch pixels */

    s00 = *((uint32 *)((uint32)(s) - bcky - bckx));
    s01 = *((uint32 *)((uint32)(s) - bcky));
    s02 = *((uint32 *)((uint32)(s) - bcky + 1 * sizeof (uint32)));
    s03 = *((uint32 *)((uint32)(s) - bcky + 2 * sizeof (uint32)));
    s10 = *((uint32 *)((uint32)(s) - bckx));
    s11 = *(s);
    s12 = *((uint32 *)((uint32)(s) + 1 * sizeof (uint32)));
    s13 = *((uint32 *)((uint32)(s) + 2 * sizeof (uint32)));
    s20 = *((uint32 *)((uint32)(s) + fbr->smod - bckx));
    s21 = *((uint32 *)((uint32)(s) + fbr->smod));
    s22 = *((uint32 *)((uint32)(s) + fbr->smod + 1 * sizeof (uint32)));
    s23 = *((uint32 *)((uint32)(s) + fbr->smod + 2 * sizeof (uint32)));
    s30 = *((uint32 *)((uint32)(s) + fbr->smod * 2 - bckx));
    s31 = *((uint32 *)((uint32)(s) + fbr->smod * 2));
    s32 = *((uint32 *)((uint32)(s) + fbr->smod * 2 + 1 * sizeof (uint32)));
    s33 = *((uint32 *)((uint32)(s) + fbr->smod * 2 + 2 * sizeof (uint32)));

    /* A */
    v00 = PIXEL_A(s00);
    v01 = PIXEL_A(s01);
    v02 = PIXEL_A(s02);
    v03 = PIXEL_A(s03);
    v10 = PIXEL_A(s10);
    v11 = PIXEL_A(s11);
    v12 = PIXEL_A(s12);
    v13 = PIXEL_A(s13);
    v20 = PIXEL_A(s20);
    v21 = PIXEL_A(s21);
    v22 = PIXEL_A(s22);
    v23 = PIXEL_A(s23);
    v30 = PIXEL_A(s30);
    v31 = PIXEL_A(s31);
    v32 = PIXEL_A(s32);
    v33 = PIXEL_A(s33);
    d = ((v00 + v01 + v02 + v03 + v10 + v11 + v12 + v13 + v20 + v21 + v22 + v23 + v30 + v31 + v32 + v33) / 16) << 24;

    /* R */
    v00 = PIXEL_R(s00);
    v01 = PIXEL_R(s01);
    v02 = PIXEL_R(s02);
    v03 = PIXEL_R(s03);
    v10 = PIXEL_R(s10);
    v11 = PIXEL_R(s11);
    v12 = PIXEL_R(s12);
    v13 = PIXEL_R(s13);
    v20 = PIXEL_R(s20);
    v21 = PIXEL_R(s21);
    v22 = PIXEL_R(s22);
    v23 = PIXEL_R(s23);
    v30 = PIXEL_R(s30);
    v31 = PIXEL_R(s31);
    v32 = PIXEL_R(s32);
    v33 = PIXEL_R(s33);
    d |= ((v00 + v01 + v02 + v03 + v10 + v11 + v12 + v13 + v20 + v21 + v22 + v23 + v30 + v31 + v32 + v33) / 16) << 16;

    /* G */
    v00 = PIXEL_G(s00);
    v01 = PIXEL_G(s01);
    v02 = PIXEL_G(s02);
    v03 = PIXEL_G(s03);
    v10 = PIXEL_G(s10);
    v11 = PIXEL_G(s11);
    v12 = PIXEL_G(s12);
    v13 = PIXEL_G(s13);
    v20 = PIXEL_G(s20);
    v21 = PIXEL_G(s21);
    v22 = PIXEL_G(s22);
    v23 = PIXEL_G(s23);
    v30 = PIXEL_G(s30);
    v31 = PIXEL_G(s31);
    v32 = PIXEL_G(s32);
    v33 = PIXEL_G(s33);
    d |= ((v00 + v01 + v02 + v03 + v10 + v11 + v12 + v13 + v20 + v21 + v22 + v23 + v30 + v31 + v32 + v33) / 16) << 8;

    /* B */
    v00 = PIXEL_B(s00);
    v01 = PIXEL_B(s01);
    v02 = PIXEL_B(s02);
    v03 = PIXEL_B(s03);
    v10 = PIXEL_B(s10);
    v11 = PIXEL_B(s11);
    v12 = PIXEL_B(s12);
    v13 = PIXEL_B(s13);
    v20 = PIXEL_B(s20);
    v21 = PIXEL_B(s21);
    v22 = PIXEL_B(s22);
    v23 = PIXEL_B(s23);
    v30 = PIXEL_B(s30);
    v31 = PIXEL_B(s31);
    v32 = PIXEL_B(s32);
    v33 = PIXEL_B(s33);
    d |= ((v00 + v01 + v02 + v03 + v10 + v11 + v12 + v13 + v20 + v21 + v22 + v23 + v30 + v31 + v32 + v33) / 16) & 0xff;

    return (d);
}
//+
#endif
///pixels_scale_bilinear
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
static uint32 pixels_scale_bilinear(FBRender *fbr, uint32 *s, uint32 x, uint32 y)
{
    uint32 v00, v10, v01, v11; /* ul, ll, ur, lr */
    uint32 v00m, v10m, v01m, v11m; /* multipliers */
    uint32 s00, s10, s01, s11;
    uint32 d;
    uint32 advx, advy;

    /* limit borders */

    if (x + 1 >= fbr->source_w)
    {
        advx = 0;
    }
    else
    {
        advx = sizeof (uint32);
    }

    if (y + 1 >= fbr->source_h)
    {
        advy = 0;
    }
    else
    {
        #if 0
        advy = fbr->smod;
        #else
        advy = fbr->smod;
        #endif
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

    v00m = (GSIMUL - fbr->xfrac) * (GSIMUL - fbr->yfrac);
    v01m = fbr->xfrac * (GSIMUL - fbr->yfrac);
    v10m = (GSIMUL - fbr->xfrac) * fbr->yfrac;
    v11m = fbr->xfrac * fbr->yfrac;

    /* A */

    v00 = v00m * PIXEL_A(s00);
    v01 = v01m * PIXEL_A(s01);
    v10 = v10m * PIXEL_A(s10);
    v11 = v11m * PIXEL_A(s11);
    d = ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 24;

    /* R */

    v00 = v00m * PIXEL_R(s00);
    v01 = v01m * PIXEL_R(s01);
    v10 = v10m * PIXEL_R(s10);
    v11 = v11m * PIXEL_R(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 16;

    /* G */

    v00 = v00m * PIXEL_G(s00);
    v01 = v01m * PIXEL_G(s01);
    v10 = v10m * PIXEL_G(s10);
    v11 = v11m * PIXEL_G(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) << 8;

    /* B */

    v00 = v00m * PIXEL_B(s00);
    v01 = v01m * PIXEL_B(s01);
    v10 = v10m * PIXEL_B(s10);
    v11 = v11m * PIXEL_B(s11);
    d |= ((v00 + v10 + v01 + v11 + GSIMUL * GSIMUL / 2) / (GSIMUL * GSIMUL)) & 0xff;

    return (d);
}
//+

///bitmap_render_free
void bitmap_render_free(FClass *Class, FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD->rendered)
    {
        if (FF_BITMAP_RENDERED_BITMAP & LOD->flags)
        {
            FreeBitMap(LOD->rendered);

            if (LOD->cm && LOD->pens)
            {
                uint32 i;

                for (i = 0 ; i < LOD->numpens ; i++)
                {
                    ReleasePen(LOD->cm,LOD->pens[i]);
                }
            }

            LOD->cm = NULL;
            LOD->numpens = NULL;

            F_Dispose(LOD->pens); LOD->pens = NULL;
        }
        else
        {
            F_Dispose(LOD->rendered);
        }

        LOD->rendered = NULL;
        LOD->rendered_w = 0;
        LOD->rendered_h = 0;
    }

    LOD->flags &= ~FF_BITMAP_RENDERED_BITMAP;
}
//+

///bitmap_render_x_scale
void bitmap_render_x_scale(struct FeelinBitMapRender *fbr, uint32 *source, uint32 *target, uint32 tpy)
{
    uint32 spx = fbr->source_w * GSIMUL / fbr->target_w;
    uint32 tpx;
    uint32 j;

    for (j = 0 ; j < fbr->target_w ; j++)
    {
        tpx = (spx * j);
        fbr->xfrac = tpx & (GSIMUL - 1);
        tpx /= GSIMUL;

        *target++ = fbr->code_render_pixel(fbr, source + tpx, tpx, tpy);
    }
}
//+
///bitmap_render_x_scale_remap
void bitmap_render_x_scale_remap(struct FeelinBitMapRender *fbr, uint32 *source, uint8 *target, uint32 tpy)
{
    uint32 spx = fbr->source_w * GSIMUL / fbr->target_w;
    uint32 tpx;
    uint32 j;

    uint32 rgb;
    uint32 pen;
    int16 r=0,g=0,b=0;
    uint16 dithbuf[] = { 0, 0, 0 };

    for (j = 0 ; j < fbr->target_w ; j++)
    {
        tpx = (spx * j);
        fbr->xfrac = tpx & (GSIMUL - 1);
        tpx /= GSIMUL;

        rgb = fbr->code_render_pixel(fbr, source + tpx, tpx, tpy);

///edd
        r += dithbuf[0];
        g += dithbuf[1];
        b += dithbuf[2];
        b += rgb & 0xff;
        rgb >>= 8;
        g += rgb & 0xff;
        rgb >>= 8;
        r += rgb & 0xff;
        if (r < 0) r = 0; else if (r > 255) r = 255;
        if (g < 0) g = 0; else if (g > 255) g = 255;
        if (b < 0) b = 0; else if (b > 255) b = 255;
        rgb = r;
        rgb <<= 8;
        rgb |= g;
        rgb <<= 8;
        rgb |= b;

        pen = F_Do(fbr->palette,fbr->id_Best,rgb);
        rgb = fbr->colors[pen];

        r -= (rgb & 0xff0000) >> 16;
        g -= (rgb & 0x00ff00) >> 8;
        b -= (rgb & 0x0000ff);
        r -= dithbuf[0] = r >> 1;
        g -= dithbuf[1] = g >> 1;
        b -= dithbuf[2] = b >> 1;
//+

        *target++ = fbr->pens[pen];
    }
}
//+
///bitmap_render_x_plain
void bitmap_render_x_plain(struct FeelinBitMapRender *fbr, uint32 *source, uint32 *target, uint32 tpy)
{
    uint32 j;

    for (j = 0 ; j < fbr->target_w ; j++)
    {
        *target++ = fbr->code_render_pixel(fbr, source + j, j, tpy);
    }
}
//+
///bitmap_render_x_plain_remap
void bitmap_render_x_plain_remap(struct FeelinBitMapRender *fbr, uint32 *source, uint8 *target, uint32 tpy)
{
    uint32 j;

    uint32 rgb;
    uint32 pen;
    int16 r=0,g=0,b=0;
    uint16 dithbuf[] = { 0, 0, 0 };

    for (j = 0 ; j < fbr->target_w ; j++)
    {
        rgb = fbr->code_render_pixel(fbr, source + j, j, tpy);

///edd
        r += dithbuf[0];
        g += dithbuf[1];
        b += dithbuf[2];
        b += rgb & 0xff;
        rgb >>= 8;
        g += rgb & 0xff;
        rgb >>= 8;
        r += rgb & 0xff;
        if (r < 0) r = 0; else if (r > 255) r = 255;
        if (g < 0) g = 0; else if (g > 255) g = 255;
        if (b < 0) b = 0; else if (b > 255) b = 255;
        rgb = r;
        rgb <<= 8;
        rgb |= g;
        rgb <<= 8;
        rgb |= b;

        pen = F_Do(fbr->palette,fbr->id_Best,rgb);
        rgb = fbr->colors[pen];

        r -= (rgb & 0xff0000) >> 16;
        g -= (rgb & 0x00ff00) >> 8;
        b -= (rgb & 0x0000ff);
        r -= dithbuf[0] = r >> 1;
        g -= dithbuf[1] = g >> 1;
        b -= dithbuf[2] = b >> 1;
//+

        *target++ = fbr->pens[pen];
    }
}
//+
#if 0
///bitmap_render_y_scale
void bitmap_render_y_scale(FBRender *fbr, uint32 *source, uint32 *target)
{
    uint32 spy = (fbr->source_h * GSIMUL) / fbr->target_h;
    uint32 tpy;
    uint32 *sp;
    uint32 i;

    for (i = 0; i < fbr->target_h; i++)
    {
        tpy = (spy * i);
        fbr->yfrac = tpy & (GSIMUL - 1);
        tpy /= GSIMUL;

        sp = source + tpy * fbr->source_w;

        fbr->code_render_x
        (
            fbr,
            sp,
            target,
            tpy
        );

        target += fbr->target_w;
    }
}
//+
#else
///bitmap_render_y_scale
void bitmap_render_y_scale(FBRender *fbr, uint32 *source, uint32 *target)
{
    uint32 spy = (fbr->source_h * GSIMUL) / fbr->target_h;
    uint32 tpy;
    uint32 *sp;
    uint32 i;

    uint32 modulo = fbr->smod;
 
    if (fbr->pixel_transform_func)
    {
        fbr->smod = fbr->source_w * sizeof(uint32);
 
        for (i = 0; i < fbr->target_h; i++)
        {
            tpy = (spy * i);
            fbr->yfrac = tpy & (GSIMUL - 1);
            tpy /= GSIMUL;

            sp = (APTR)((uint32)(source) + tpy * modulo);
            
            fbr->pixel_transform_func(fbr, (APTR)((uint32)(sp) - modulo), fbr->source_w * 3);

            fbr->code_render_x
            (
                fbr,
                (APTR)((uint32)(fbr->pixel_transform_buffer) + fbr->smod),
                target,
                tpy
            );

            target += fbr->target_w;
        }
    }
    else
    {
        for (i = 0; i < fbr->target_h; i++)
        {
            tpy = (spy * i);
            fbr->yfrac = tpy & (GSIMUL - 1);
            tpy /= GSIMUL;

            sp = (APTR)((uint32)(source) + tpy * modulo);

            fbr->code_render_x
            (
                fbr,
                sp,
                target,
                tpy
            );

            target += fbr->target_w;
        }

    }
}
//+
#endif
///bitmap_render_y_scale_remap
void bitmap_render_y_scale_remap(FBRender *fbr, uint32 *source, uint32 *target)
{
    uint32 spy = (fbr->source_h * GSIMUL) / fbr->target_h;
    uint32 tpy;
    uint32 *sp;
    uint32 i;

    for (i = 0; i < fbr->target_h; i++)
    {
        tpy = (spy * i);
        fbr->yfrac = tpy & (GSIMUL - 1);
        tpy /= GSIMUL;

        sp = source + tpy * fbr->source_w;

        fbr->code_render_x
        (
            fbr,
            sp,
            fbr->line_buffer,
            tpy
        );

        WritePixelLine8(fbr->rp1,0,i,fbr->target_w,fbr->line_buffer,fbr->rp2);
    }
}
//+
///bitmap_render_y_plain
void bitmap_render_y_plain(FBRender *fbr, uint32 *source, uint32 *target)
{
    uint32 i;

    for (i = 0; i < fbr->target_h; i++)
    {
        fbr->code_render_x
        (
            fbr,
            source + i * fbr->source_w,
            target + i * fbr->target_w,
            i
        );
    }
}
//+
///bitmap_render_y_plain_remap
void bitmap_render_y_plain_remap(FBRender *fbr, uint32 *source, uint32 *target)
{
    uint32 i;

    for (i = 0; i < fbr->target_h; i++)
    {
        fbr->code_render_x
        (
            fbr,
            source + i * fbr->source_w,
            fbr->line_buffer,
            i
        );

        WritePixelLine8(fbr->rp1,0,i,fbr->target_w,fbr->line_buffer,fbr->rp2);
    }
}
//+

///BitMap_Render
F_METHOD(uint32,BitMap_Render)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    uint32 target_w = LOD->source.Width;
    uint32 target_h = LOD->source.Height;
    int8 remap = TRUE;
    int8 depth = 0;
    uint32 maxcolors = 32;
    uint32 *rendered_w_ptr=NULL;
    uint32 *rendered_h_ptr=NULL;
    
    struct Screen *scr = NULL;
    
    uint32 done = FALSE;

    F_Do(Obj, FM_Lock, FF_Lock_Exclusive);
 
/*** read optional parameters **********************************************/

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_BitMap_TargetWidth: target_w = item.ti_Data; break;
        case FA_BitMap_TargetHeight: target_h = item.ti_Data; break;
        case FA_BitMap_TargetScreen: scr = (struct Screen *) item.ti_Data; break;
        case FA_BitMap_TargetMaxColors: maxcolors = item.ti_Data; break;
        case FA_BitMap_RenderedWidth: rendered_w_ptr = (uint32 *) item.ti_Data; break;
        case FA_BitMap_RenderedHeight: rendered_h_ptr = (uint32 *) item.ti_Data; break;
    }

    bitmap_render_free(Class,Obj);

/*** check screen & and remap needs ****************************************/

//    F_Log(0,"target (%ld x %ld) prt (0x%08lx & 0x%08lx)",target_w,target_h,rendered_w_ptr,rendered_h_ptr);
 
    if (scr)
    {
        depth = GetBitMapAttr(scr->RastPort.BitMap,BMA_DEPTH);

        if (depth > 8)
        {
            remap = FALSE;
        }
    }
    else if (CyberGfxBase)
    {
        remap = FALSE;
    }
    else
    {
        F_Log(0,"If you don't have cybergraphics.library, you must provide a Screen to remap your BitMap");
        
        goto __end;
    }

//    remap = TRUE; // uncomment this line to force remapping

/*** select rendering ******************************************************/

    #if 0
    if ((remap == FALSE) && (target_w == LOD->source.Width) && (target_h == LOD->source.Height))
    {
        LOD->rendered_w = target_w,
        LOD->rendered_h = target_h,
        
        done = TRUE; goto __end;
    }
    else
    {
        uint32 *source = LOD->source.PixelArray;
        APTR target = NULL;
    
        FBRender fbr;

        fbr.smod = LOD->source.PixelArrayMod;
        fbr.source_w = LOD->source.Width;
        fbr.source_h = LOD->source.Height;
        fbr.xfrac = 0;
        fbr.yfrac = 0;
        fbr.target_w = target_w;
        fbr.target_h = target_h;
        
        fbr.pixel_transform_buffer = NULL;

        fbr.code_render_y = NULL;
        fbr.code_render_x = NULL;

        fbr.rp1 = NULL;
        fbr.rp2 = NULL;
        fbr.line_buffer = NULL;
        fbr.palette = NULL;
        fbr.colors = NULL;
        fbr.pens = NULL;
        fbr.numpens = 0;
        fbr.id_Best = 0;
        
        fbr.color_array = LOD->source.ColorArray;
        
        switch (LOD->source.PixelType)
        {
            case FV_PIXEL_TYPE_RGBA:
            break;
            
            case FV_PIXEL_TYPE_PALETTE:
            {
                fbr.pixel_transform_buffer = F_New(fbr.source_w * sizeof (uint32) * 3);

                if (fbr.pixel_transform_buffer)
                {
                    fbr.pixel_transform_func = pixel_transform_palette;

                    LOD->rendered_pixel_type = FV_PIXEL_TYPE_RGB0;
                }
                else
                {
                    goto __end;
                }
            }
            break;

            case FV_PIXEL_TYPE_RGB:
            {
                fbr.pixel_transform_buffer = F_New(fbr.source_w * sizeof (uint32) * 3);

                if (fbr.pixel_transform_buffer)
                {
                    fbr.pixel_transform_func = pixel_transform_rgb;

                    LOD->rendered_pixel_type = FV_PIXEL_TYPE_RGB0;
                }
                else
                {
                    goto __end;
                }
            }
            break;

            case FV_PIXEL_TYPE_RGB0:
            {
                LOD->rendered_pixel_type = FV_PIXEL_TYPE_RGB0;
            }
            break;

            default:
            {
                F_Log(0,"Unkown PixelType (%ld)", LOD->source.PixelType);
                
                goto __end;
            }
            break;
        }

        #if 0
        if (((LOD->source.Width * LOD->source.Height) / (target_w * target_h)) > FV_PICTURE_AVERAGE_LEVEL)
        {
            fbr.code_render_pixel = pixels_scale_average;
        }
        else
        #endif
        {
            fbr.code_render_pixel = pixels_scale_bilinear;
        }

/*** update or create ******************************************************/

        if (remap)
        {
///remap
            FObject histogram = HistogramObject, End;

            if (histogram)
            {
                F_Do(histogram,(uint32) "FM_Histogram_AddPixels", LOD->source.PixelArray, LOD->source.Width, LOD->source.Height);

                fbr.palette = PaletteObject,

                    "FA_Palette_Histogram", histogram,
                    "FA_Palette_MaxColors", maxcolors,

                    End;

                F_DisposeObj(histogram);
            }

//            F_Log(0,"palette (0x%08lx)",fbr.palette);

            if (fbr.palette)
            {
                fbr.pens = (uint32 *) F_Do(fbr.palette,(uint32) "FM_Palette_ObtainPens", scr->ViewPort.ColorMap);
                fbr.id_Best = F_DynamicFindID("FM_Palette_Best");

                F_Do(fbr.palette, FM_Get,

                    "FA_Palette_NumColors", &fbr.numpens,
                    "FA_Palette_Colors", &fbr.colors,

                    TAG_DONE);

//                F_Log(0,"colors (0x%08lx) >> pens (0x%08lx)(%ld)",fbr.colors,fbr.pens,fbr.numpens);

                if (fbr.pens && fbr.numpens)
                {
                    fbr.rp1 = F_New(sizeof (struct RastPort) * 2 + target_w + 16);

                    if (fbr.rp1)
                    {
                        fbr.rp2 = (struct RastPort *)((uint32)(fbr.rp1) + sizeof (struct RastPort));
                        fbr.line_buffer = (uint8 *)((uint32)(fbr.rp2) + sizeof (struct RastPort));

                        InitRastPort(fbr.rp1);
                        InitRastPort(fbr.rp2);

                        target = AllocBitMap(target_w,target_h,depth,0,scr->RastPort.BitMap);

                        if (target)
                        {
                            fbr.rp1->BitMap = target;
                            fbr.rp2->BitMap = AllocBitMap(target_w,1,depth,0,fbr.rp1->BitMap);

                            if (fbr.rp2->BitMap)
                            {
                                fbr.code_render_x = (target_w == LOD->source.Width) ? bitmap_render_x_plain_remap : bitmap_render_x_scale_remap;
                                fbr.code_render_y = (target_h == LOD->source.Height) ? bitmap_render_y_plain_remap : bitmap_render_y_scale_remap;
                            }
                        }
                    }
                }
            }
//+
        }
        else
        {
            fbr.code_render_x = (target_w == LOD->source.Width) ? bitmap_render_x_plain : bitmap_render_x_scale;
            fbr.code_render_y = (target_h == LOD->source.Height) ? bitmap_render_y_plain : bitmap_render_y_scale;
 
            target = F_New(target_w * target_h * sizeof (uint32));
        }

/*** rendering *************************************************************/

        if (fbr.code_render_y &&
            fbr.code_render_x && source && target)
        {
            fbr.code_render_y(&fbr, source, target);

            done = TRUE;
        }

/*** dispose remap temporary objects ***************************************/

        F_Dispose(fbr.pixel_transform_buffer);

        if (remap)
        {
            if (fbr.rp1)
            {
                if (fbr.rp2->BitMap)
                {
                    FreeBitMap(fbr.rp2->BitMap);
                }

                /* the bitmap attached to 'rp1' is the  'target'.  It  will  be
                freed later */

            #ifdef __AROS__
	        DeinitRastPort(fbr.rp1);
		DeinitRastPort(fbr.rp2);
	    #endif
                F_Dispose(fbr.rp1);
            }
            F_DisposeObj(fbr.palette);
        } 

/*** done ? free resources on failure **************************************/

        if (done)
        {
            LOD->rendered = target;
            LOD->rendered_w = target_w;
            LOD->rendered_h = target_h;
            
            if (remap)
            {
                LOD->pens = fbr.pens;
                LOD->numpens = fbr.numpens;
                LOD->cm = scr->ViewPort.ColorMap;
                
                LOD->flags |= FF_BITMAP_RENDERED_BITMAP;
            }
            else
            {
                LOD->flags &= ~FF_BITMAP_RENDERED_BITMAP;
            }
        }
        else
        {
            if (remap)
            {
                if (target)
                {
                    FreeBitMap(target);
                }

                /*** free associated pens as well **************************/

                if (fbr.pens)
                {
                    uint32 i;

                    for (i = 0 ; i < fbr.numpens ; i++)
                    {
                        ReleasePen(scr->ViewPort.ColorMap,fbr.pens[i]);
                    }

                    F_Dispose(fbr.pens);
                }
            }
            else
            {
                F_Dispose(target);
            }
        }
    }
    #else
    
    LOD->rendered_w = target_w;
    LOD->rendered_h = target_h;
    
    #endif

/*** done ******************************************************************/
            
__end:

    if (rendered_w_ptr)
    {
        *rendered_w_ptr = LOD->rendered_w;
    }

    if (rendered_h_ptr)
    {
        *rendered_h_ptr = LOD->rendered_h;
    }

    F_Do(Obj, FM_Unlock);
    
    return done;
}
//+

