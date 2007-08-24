#include "Private.h"

///memfill32
void memfill32(uint32 *mem, uint32 len, uint32 fill)
{
    uint32 len8;

    len >>= 2;
    len8 = (len >> 3) + 1;

    switch (len & 7)
    {
        do
        {
            *mem++ = fill;

            case 7:  *mem++ = fill;
            case 6:  *mem++ = fill;
            case 5:  *mem++ = fill;
            case 4:  *mem++ = fill;
            case 3:  *mem++ = fill;
            case 2:  *mem++ = fill;
            case 1:  *mem++ = fill;
            case 0:  len8--;

        }
        while (len8);
    }
}
//+

///Palette_New
F_METHOD(uint32,Palette_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    FObject histogram=NULL;
    uint32 maxcolors=0;
    uint32 numcolors=0;
    uint32 resolution=0;
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Palette_Colors:     LOD -> colors = (uint32 *) item.ti_Data; break;
        case FA_Palette_NumColors:  numcolors = item.ti_Data; break;
        case FA_Palette_MaxColors:  maxcolors = item.ti_Data; break;
        case FA_Palette_Histogram:  histogram = (FObject) item.ti_Data; break;
        case FA_Palette_Resolution: resolution = item.ti_Data; break;
    }

    if (maxcolors == 0)
    {
        return NULL;
    }
 
/*** resolve missing values ************************************************/

    if (histogram)
    {
        if (numcolors == 0)
        {
            LOD -> numcolors = F_Get(histogram,(uint32) "FA_Histogram_NumColors");
            
            if (maxcolors < LOD -> numcolors)
            {
                LOD -> numcolors = maxcolors;
            }
        }
    
        if (resolution == 0)
        {
            resolution = F_Get(histogram,(uint32) "FA_Histogram_Resolution");
        }
    
        if (LOD -> colors == NULL && LOD -> numcolors != 0)
        {
            LOD -> colors = (uint32 *) F_Do(histogram,(uint32) "FM_Histogram_Extract", LOD -> numcolors);
            
            LOD -> flags |= FF_PALETTE_EXTRACTED;
        }
    }
    else if (LOD -> colors)
    {
        LOD -> numcolors = maxcolors;
    }

/*** process values ********************************************************/


    if (resolution < 12)
    {
        LOD -> bitspergun = 4;
    }
    else if (resolution > 24)
    {
        LOD -> bitspergun = 8;
    }
    else
    {
        switch (resolution)
        {
            case 13: case 14: case 15: LOD -> bitspergun = 5; break;
            case 16: case 17: case 18: LOD -> bitspergun = 6; break;
            case 19: case 20: case 21: LOD -> bitspergun = 7; break;
            case 22: case 23: case 24: LOD -> bitspergun = 8; break;
        }
    }

/*** create object *********************************************************/
    
    if (LOD -> colors && LOD -> numcolors)
    {
        uint32 lookup_size = (1L << (3 * LOD -> bitspergun)) * sizeof(uint16);
/*
        uint32 i;

        for (i = 0 ; i < LOD -> numcolors ; i++)
        {
            F_Log(0,"color[%03ld] 0x%06lx",i,LOD -> colors[i]);
        }
*/
        LOD -> lut = F_New(lookup_size);

        if (LOD -> lut)
        {
            memfill32((uint32 *) LOD -> lut, lookup_size, 0xffffffff);

            LOD -> lutmask = 0xFF >> (8 - LOD -> bitspergun); /* ...11111 */

            return F_SUPERDO();
        }
    }
    return NULL;
}
//+
///Palette_Dispose
F_METHOD(void,Palette_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_Dispose(LOD -> lut); LOD -> lut = NULL;

    if (FF_PALETTE_EXTRACTED & LOD -> flags)
    {
        F_Dispose(LOD -> colors);
    }

    LOD -> colors = NULL;
 
    F_SUPERDO();
}
//+
///Palette_Get
F_METHOD(void,Palette_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Palette_Colors:     F_STORE(LOD -> colors); break;
        case FA_Palette_NumColors:  F_STORE(LOD -> numcolors); break;
        case FA_Palette_Resolution: F_STORE(LOD -> bitspergun * 3); break;
    }

    F_SUPERDO();
}
//+

struct FS_Palette_Best                          { uint32 rgb; };
struct FS_Palette_ObtainPens                    { struct ColorMap *CM; };

///Palette_Best
///palette_lookup_index
static uint32 palette_lookup_index(struct LocalObjectData *LOD, uint32 rgb)
{
   uint32 foo;

   rgb >>= 8 - LOD -> bitspergun;
   foo = (rgb & LOD -> lutmask);
   foo <<= LOD -> bitspergun;
   rgb >>= 8;
   foo |= (rgb & LOD -> lutmask);
   foo <<= LOD -> bitspergun;
   rgb >>= 8;
   foo |= rgb;

   return foo;
}
//+
///palette_calc_pen
STATIC int16 palette_calc_pen(struct LocalObjectData *LOD, uint32 rgb)
{
    int32 i, d;
    int16 besti;
    int32 bestd = 196000;
    int16 r, g, b;
    int16 dr, dg, db;

    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);

    for (i = 0; i < LOD -> numcolors; i++)
    {
        rgb = LOD -> colors[i];

        dr = r - ((rgb & 0xff0000) >> 16);
        dg = g - ((rgb & 0x00ff00) >> 8);
        db = b - (rgb & 0x0000ff);
        d = dr*dr + dg*dg + db*db;

        if (d < bestd)
        {
            besti = i;
            bestd = d;

            if (bestd == 0)
            {
                break;
            }
        }
    }
    return besti;
}
//+

F_METHODM(int32,Palette_Best,FS_Palette_Best)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
     
    int16 *lut = LOD -> lut + palette_lookup_index(LOD, Msg -> rgb);
    int32 pen = *lut;

    if (pen < 0)
    {
        pen = palette_calc_pen(LOD, Msg -> rgb);
        *lut = pen;
    }
    return pen;
}
//+
///Palette_ObtainPens

    /* create and array of pens, and allocate them from a  given  colormap.
    The  pen  array  doesn't  belong  to  the  palette  object  but  to the
    requester, you. Thus, once all your precious pens have  been  allocated
    and  your  image  remaped  using  FM_Palette_Best,  you can dispose the
    palette object, and keep the pen array to release pens later. */

F_METHODM(uint32 *,Palette_ObtainPens,FS_Palette_ObtainPens)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    if (Msg -> CM)
    {
        uint32 *pens = F_New(sizeof (uint32) * LOD -> numcolors);

        if (pens)
        {
            uint32 i;

            for (i = 0 ; i < LOD -> numcolors ; i++)
            {
                uint32 r,g,b,rgb = LOD -> colors[i];

                r = (0x00FF0000 & rgb) >> 16; r = r | (r << 8) | (r << 16) | (r << 24);
                g = (0x0000FF00 & rgb) >>  8; g = g | (g << 8) | (g << 16) | (g << 24);
                b = (0x000000FF & rgb) >>  0; b = b | (b << 8) | (b << 16) | (b << 24);

                pens[i] = ObtainBestPen(Msg -> CM, r,g,b, OBP_Precision, PRECISION_IMAGE, TAG_DONE);

//                F_Log(0,"obtained[%03ld] %06lx >> pen (%ld)",i,rgb,pens[i]);
            }

            return pens;
        }
    }
    return NULL;
}
//+

