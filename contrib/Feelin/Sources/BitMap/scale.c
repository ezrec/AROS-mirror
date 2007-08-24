#include "Private.h"

#if 0
///typedefs

typedef void (*feelin_scale_y)                  (struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y);
typedef void (*feelin_scale_x)                  (struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 tpy);
typedef uint32 (*feelin_get_pixel)              (struct FeelinBitMapScale *fbs, uint32 *s, uint32 x, uint32 y);
typedef void (*feelin_func_transform)           (struct FeelinBitMapScale *fbs, APTR source, uint32 *target, uint32 pixels);

struct FeelinBitMapScale
{
    uint16                          source_w;
    uint16                          source_h;
    uint32                          source_modulo;
    uint32                          source_modulo_real;

    uint16                          target_w;
    uint16                          target_h;
    uint32                          target_modulo;

    uint32                          xfrac;
    uint32                          yfrac;

    uint32                          spy;

    feelin_scale_y                  scale_y_func;
    feelin_scale_x                  scale_x_func;
    feelin_get_pixel                get_pixel_func;

    // transform

    uint32                         *transform_buffer;
    feelin_func_transform           transform_func;

    uint32                         *color_array;
};
//+

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
        bcky = fbs->source_modulo;
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
void bitmap_x_scale(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 tpy)
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
void bitmap_y_scale(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y)
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
void bitmap_y_scale_transform(struct FeelinBitMapScale *fbs, uint32 *source, uint32 *target, uint32 y)
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

///bitmap_transform_rgb
void bitmap_transform_rgb(struct FeelinBitMapScale *fbs, uint8 *source, uint32 *target, uint32 pixels)
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
void bitmap_transform_palette(struct FeelinBitMapScale *fbs, uint8 *source, uint32 *target, uint32 pixels)
{
    uint32 stop = (uint32)(source) + (pixels * sizeof (uint8));

    while ((uint32)(source) < stop)
    {
        *target++ = fbs->color_array[*source++];
    }
}
//+
///bitmap_blit_scale
void bitmap_blit_scale
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
    #if 0
    struct FeelinBitMapScale fbs;

    uint32 *target;

    fbs.source_w = source_w;
    fbs.source_h = source_h;
    fbs.source_modulo = modulo;
    fbs.source_modulo_real = modulo;

    fbs.target_w = target_w;
    fbs.target_h = target_h;
    fbs.target_modulo = target_w * sizeof (uint32);

    fbs.xfrac = 0;
    fbs.yfrac = 0;

    fbs.scale_y_func = bitmap_y_scale;
    fbs.scale_x_func = bitmap_x_scale;
    #if 0
    fbs.get_pixel_func = bitmap_get_pixel_nearest;
    #else
    fbs.get_pixel_func = bitmap_get_pixel_bilinear;
    #endif

    fbs.color_array = ColorArray;

    fbs.spy = (fbs.source_h * GSIMUL) / fbs.target_h;

    fbs.transform_func=NULL;
    fbs.transform_buffer=NULL;

    target = F_New(fbs.target_modulo);
/*
        F_Log(0,"source (%ld : %ld, %ld x %ld) target (%ld : %ld, %ld x %ld)",
            source_x, source_y, source_w, source_h,
            target_x, target_y, target_w, target_h);
*/
    source = (APTR)((uint32)(source) + source_y * modulo);
    // + source_x * sizeof(uint32));

    if (target)
    {
        uint32 i;

        if (PixelType != FV_PIXEL_TYPE_RGBA)
        {
            fbs.source_modulo = source_w * sizeof (uint32);
            fbs.transform_buffer = F_New(fbs.source_modulo * 3);
            fbs.scale_y_func = bitmap_y_scale_transform;

            if (fbs.transform_buffer)
            {
                switch (PixelType)
                {
                    case FV_PIXEL_TYPE_PALETTE:
                    {
                        fbs.transform_func = bitmap_transform_palette;

                        source = (APTR)((uint32)(source) + source_x * sizeof (uint8));

                        PixelType = FV_PIXEL_TYPE_RGB0;
                    }
                    break;

                    case FV_PIXEL_TYPE_RGB:
                    {
                        fbs.transform_func = bitmap_transform_rgb;

                        source = (APTR)((uint32)(source) + source_x * sizeof (uint8) * 3);

                        PixelType = FV_PIXEL_TYPE_RGB0;
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
        else
        {
            source = (APTR)((uint32)(source) + source_x * sizeof(uint32));
        }

        fbs.scale_func(&bfs, source, target);

__done:

        F_Dispose(fbs.transform_buffer);
        F_Dispose(target);
    }
    #endif
}
//+
#endif
