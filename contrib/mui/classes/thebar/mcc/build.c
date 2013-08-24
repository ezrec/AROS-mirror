/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"
#include "private.h"
#include "SetPatch.h"
#include <clib/macros.h>

/***********************************************************************/

#define ALLOCRASTER(w,h)        AllocVec(RAWIDTH(w)*((UWORD)(h)),MEMF_CHIP|MEMF_CLEAR)
#define FREERASTER(ra)          FreeVec(ra)

#define ALLOCRASTERCG(w,h)      SharedAlloc(RAWIDTH(w)*((UWORD)(h)))
#define FREERASTERCG(ra)        SharedFree(ra)

/***********************************************************************/

#define DONT_SKIP_PIC(p)   (((void *)(p)) != NULL && ((void *)(p)) != MUIV_TheBar_SkipPic)

/***********************************************************************/

struct palette
{
    ULONG colors[256];
    ULONG maxColors;
    ULONG numColors;
};

struct make
{
    UBYTE          *chunky;
    UBYTE          *gchunky;
    APTR           mask;
    struct palette pal;
    struct palette gpal;

    UBYTE          *schunky;
    UBYTE          *sgchunky;
    APTR           smask;
    struct palette spal;
    struct palette sgpal;

    UBYTE          *dchunky;
    UBYTE          *dgchunky;
    APTR           dmask;
    struct palette dpal;
    struct palette dgpal;

    UWORD          dw;
    UWORD          dh;

    ULONG          flags;
};

struct copy
{
    UWORD          dw;
    UWORD          dh;
    APTR           mask;
    UBYTE          *grey;
    ULONG          flags;
    struct palette *pal;
    struct palette *gpal;
};

enum
{
    MFLG_Grey   = 1<<0,
    MFLG_NtMask = 1<<1,
    MFLG_Cyber  = 1<<2,
};

/***********************************************************************/

static UBYTE * LUT8ToLUT8(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size;
    UWORD w, h;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    w = image->width;
    h = image->height;

    size = w*h;

    if (isFlagSet(flags, MFLG_Grey))
    {
        if ((chunky = SharedAlloc(size+size)) == NULL)
            clearFlag(flags, MFLG_Grey);
    }
    else
        chunky = NULL;

    if (chunky == NULL)
        chunky = SharedAlloc(size);

    if (chunky != NULL)
    {
        UBYTE *alpha = NULL;

        if (isFlagClear(flags, MFLG_NtMask))
        {
            if((copy->mask = isFlagSet(flags, MFLG_Cyber) ? ALLOCRASTERCG(w,h) : ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }

        if (image->left!=0 || image->top!=0 || image->width!=image->dataWidth || image->height!=image->dataHeight || image->dataWidth!=image->dataTotalWidth)
        {
            UBYTE *src, *dest;
            ULONG trColor;
            UWORD tsw;
            int   x, y;

            tsw     = image->dataTotalWidth;
            trColor = image->trColor;

            src  = (UBYTE *)image->data+image->left+image->top*tsw;
            dest = chunky;

            for (y = 0; y<h; y++)
            {
                int bitmask = 0x80, aflag = 0;

                for (x = 0; x<w; x++)
                {
                    if (alpha)
                    {
                        UBYTE p = *src++;

                              if (!aflag)
                        {
                            alpha[x>>3] = 0;
                            aflag = 1;
                        }

                        if (p==trColor)
                            alpha[x>>3] &= ~bitmask;
                        else
                            alpha[x>>3] |= bitmask;

                        if (!(bitmask >>= 1))
                        {
                            bitmask = 0x80;
                            aflag = 0;
                        }

                        *dest++ = p;
                    }
                    else
                        *dest++ = *src++;
                }

                src += tsw-w;
                if (alpha)
                    alpha += RAWIDTH(w);
            }
        }
        else
        {
            memcpy(chunky,image->data,size);

            if (alpha)
            {
                UBYTE *src;
                ULONG trColor;
                int   x, y;

                trColor = image->trColor;

                src = chunky;

                for (y = 0; y<h; y++)
                {
                    int bitmask = 0x80, aflag = 0;

                    for (x = 0; x<w; x++)
                    {
                        if (!aflag)
                        {
                            alpha[x>>3] = 0;
                            aflag = 1;
                        }

                        if (*src++==trColor)
                            alpha[x>>3] &= ~bitmask;
                        else
                            alpha[x>>3] |= bitmask;

                        if (!(bitmask >>= 1))
                        {
                            bitmask = 0x80;
                            aflag = 0;
                        }
                    }

                    alpha += RAWIDTH(w);
                }
            }
        }

        if (isFlagSet(flags, MFLG_Grey))
            memcpy(copy->grey = chunky+size,chunky,size);
    }

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static UBYTE * LUT8ToRGB(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size;
    UWORD w, h;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    w = image->width;
    h = image->height;

    size  = w*h;
    size += size+size+size;

    if (isFlagSet(flags, MFLG_Grey))
    {
        if ((chunky = SharedAlloc(size+size)) == NULL)
            clearFlag(flags, MFLG_Grey);
    }
    else
        chunky = NULL;

    if (chunky == NULL)
        chunky = SharedAlloc(size);

    if (chunky != NULL)
    {
        ULONG *colors;
        ULONG trColor;
        BOOL RGB8 = isFlagSet(image->flags, BRFLG_ColorRGB8);
        UBYTE *src, *dest, *alpha = NULL, *gdest;
        UWORD tsw;
        int   x, y;

        if (isFlagClear(flags, MFLG_NtMask))
        {
            if((copy->mask = isFlagSet(flags, MFLG_Cyber) ? ALLOCRASTERCG(w,h) : ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }

        copy->grey = gdest = isFlagSet(flags, MFLG_Grey) ? chunky+size : NULL;

        tsw     = image->dataTotalWidth;
        colors  = image->colors;
        trColor = image->trColor;

        src  = (UBYTE *)image->data+image->left+image->top*tsw;
        dest = chunky;

        for (y = 0; y<h; y++)
        {
            int bitmask = 0x80, aflag = 0;

            for (x = 0; x<w; x++)
            {
                ULONG p = *src++;
                ULONG *c = colors+p+(RGB8 ? 0 : p+p);

                if (alpha)
                {
                    if (!aflag)
                    {
                        alpha[x>>3] = 0;
                        aflag = 1;
                    }

                    if (p==trColor)
                        alpha[x>>3] &= ~bitmask;
                    else
                        alpha[x>>3] |= bitmask;

                    if (!(bitmask >>= 1))
                    {
                        bitmask = 0x80;
                        aflag = 0;
                    }
                }

                if (gdest)
                {
                    ULONG gcol;
                    UBYTE r, g, b;

                    if (RGB8)
                    {
                        ULONG cv = *c;

                        r = (cv & 0x00FF0000)>>16;
                        g = (cv & 0x0000FF00)>>8;
                        b = (cv & 0x000000FF);
                    }
                    else
                    {
                        ULONG *cp = c;

                        r = *cp++ >>24;
                        g = *cp++ >>24;
                        b = *cp   >>24;
                    }

                    gcol = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
                    gdest++;
                    *gdest++ = gcol;
                    *gdest++ = gcol;
                    *gdest++ = gcol;
                }

                dest++;

                if (RGB8)
                {
                    ULONG cv = *c;

                    *dest++ = 0x01010101 * ((cv & 0x00FF0000)>>16);
                    *dest++ = 0x01010101 * ((cv & 0x0000FF00)>>8);
                    *dest++ = 0x01010101 * ((cv & 0x000000FF));
                }
                else
                {
                    *dest++ = *c++ >>24;
                    *dest++ = *c++ >>24;
                    *dest++ = *c   >>24;
                }
            }

            src += tsw-w;
            if (alpha)
                alpha += RAWIDTH(w);
        }
    }

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static UBYTE * RGBToRGB(UNUSED struct InstData *data,struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size, maskDone = FALSE;
    UWORD w, h;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    w = image->width;
    h = image->height;

    size = w*h;
    size += size+size+size;

    if (isFlagSet(flags, MFLG_Grey))
    {
        if ((chunky = SharedAlloc(size+size)) == NULL)
            clearFlag(flags, MFLG_Grey);
    }
    else
        chunky = NULL;

    if (chunky == NULL)
        chunky = SharedAlloc(size);

    if (chunky != NULL)
    {
        UBYTE *alpha = NULL, *gdest;

        if (isFlagClear(flags, MFLG_NtMask))
        {
            if((copy->mask = isFlagSet(flags, MFLG_Cyber) ? ALLOCRASTERCG(w,h) : ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }

        copy->grey = gdest = isFlagSet(flags, MFLG_Grey) ? chunky+size : NULL;

        if (image->left!=0 || image->top!=0 || image->width!=image->dataWidth || image->height!=image->dataHeight || 4*image->dataWidth!=image->dataTotalWidth)
        {
            UBYTE *src, *dest;
            ULONG trColor;
            BOOL useAlpha, reallyHasAlpha = FALSE;
            UWORD tsw;
            int   x, y;

            tsw      = image->dataTotalWidth;
            trColor  = image->trColor & 0x00FFFFFF;
            useAlpha = isFlagSet(image->flags, BRFLG_AlphaMask);

            src  = (UBYTE *)image->data+4*image->left+image->top*tsw;
            dest = chunky;

            for (y = 0; y<h; y++)
            {
                int bitmask = 0x80, aflag = 0;

                for (x = 0; x<w; x++)
                {
                    ULONG c = *((ULONG *)src);

                    if (alpha)
                    {
                        ULONG hi;

                        if (!aflag)
                        {
                            alpha[x>>3] = 0;
                            aflag = 1;
                        }

                        #if defined(WITH_ALPHA)
                        if (useAlpha)
                            hi = *src<0xFF;
                        #else
                        if (useAlpha)
                            hi = (data->allowAlphaChannel ? *src<0xFF : !(c & 0xFF000000));
                        #endif
                        else
                            hi = (c & 0x00FFFFFF)==trColor;

                        if (!hi)
                            alpha[x>>3] |= bitmask;

                        if (!(bitmask >>= 1))
                        {
                            bitmask = 0x80;
                            aflag = 0;
                        }

                        if (*src)
                            reallyHasAlpha = TRUE;
                    }

                    if (gdest)
                    {
                        ULONG v;
                        UBYTE r, g, b;

                        r = (c & 0x00FF0000)>>16;
                        g = (c & 0x0000FF00)>>8;
                        b = (c & 0x000000FF);

                        v = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
                        *gdest++ = *src;
                        *gdest++ = v;
                        *gdest++ = v;
                        *gdest++ = v;
                    }

                    //dest++;
                    //src++;

                    *dest++ = *src++;
                    *dest++ = *src++;
                    *dest++ = *src++;
                    *dest++ = *src++;
                }

                src += tsw-4*w;
                if (alpha)
                    alpha += RAWIDTH(w);
            }

            if(!reallyHasAlpha)
              setFlag(image->flags, BRFLG_EmptyAlpha);
            else
              clearFlag(image->flags, BRFLG_EmptyAlpha);

            maskDone = TRUE;
        }
        else
            memcpy(chunky,image->data,size);


        if (!maskDone && (alpha || gdest))
        {
            UBYTE *src;
            ULONG trColor, useAlpha, reallyHasAlpha = FALSE;
            int   x, y;

            trColor = image->trColor & 0x00FFFFFF;
            useAlpha = isFlagSet(image->flags, BRFLG_AlphaMask);

            src = chunky;

            for (y = 0; y<h; y++)
            {
                int bitmask = 0x80, aflag = 0;

                for (x = 0; x<w; x++)
                {
                    ULONG c = *((ULONG *)src);

                    if (alpha)
                    {
                        ULONG hi;

                        if (!aflag)
                        {
                            alpha[x>>3] = 0;
                            aflag = 1;
                        }

                        #if defined(WITH_ALPHA)
                        if (useAlpha)
                            hi = *src<0xFF;
                        #else
                        if (useAlpha)
                            hi = (data->allowAlphaChannel ? *src<0xFF : !(c & 0xFF000000));
                        #endif
                        else
                            hi = (c & 0x00FFFFFF)==trColor;

                        if (!hi)
                            alpha[x>>3] |= bitmask;

                        if (!(bitmask>>=1))
                        {
                            bitmask = 0x80;
                            aflag = 0;
                        }

                        if (*src)
                            reallyHasAlpha = TRUE;
                    }

                    if (gdest)
                    {
                        ULONG v;
                        UBYTE r, g, b;

                        r = (c & 0x00FF0000)>>16;
                        g = (c & 0x0000FF00)>>8;
                        b = (c & 0x000000FF);

                        v = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
                        *gdest++ = *src;
                        *gdest++ = v;
                        *gdest++ = v;
                        *gdest++ = v;
                    }

                    src += 4;
                }

                if (alpha)
                    alpha += RAWIDTH(w);
            }

            if (!reallyHasAlpha)
                setFlag(image->flags, BRFLG_EmptyAlpha);
            else
                clearFlag(image->flags, BRFLG_EmptyAlpha);
        }
    }

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static LONG
calcPen(struct palette *pal,ULONG rgb)
{
    ULONG i;
    LONG d, bestd = 196000;
    WORD besti = 0, r, g, b, dr, dg, db;

    ENTER();

    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);

    for(i = 0; i<pal->numColors; i++)
    {
        rgb = pal->colors[i];
        dr = r - ((rgb & 0xff0000) >> 16);
        dg = g - ((rgb & 0x00ff00) >> 8);
        db = b - (rgb & 0x0000ff);
        d = dr*dr + dg*dg + db*db;
        if (d<bestd)
        {
            besti = i;
            bestd = d;
            if (bestd==0)
                break;
        }
    }

    RETURN(besti);
    return besti;
}

static LONG
addColor(struct palette *pal,ULONG rgb)
{
    LONG p;
    ULONG i;

    ENTER();

    for (i = 0; i<pal->numColors; i++)
    {
        if(pal->colors[i]==rgb)
        {
          RETURN(i);
          return i;
        }
    }

    if (pal->numColors<pal->maxColors)
    {
        pal->colors[pal->numColors] = rgb;
        p = pal->numColors++;
    }
    else
        p = -1;

    RETURN(p);
    return p;
}

static LONG
bestColor(struct palette *pal,ULONG rgb)
{
    LONG p = addColor(pal,rgb);
    LONG best;

    ENTER();

    best = (p<0) ? calcPen(pal,rgb) : p;

    RETURN(best);
    return best;
}

/***********************************************************************/

static UBYTE * RGBToLUT8(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size;
    UWORD w, h, left, top, tsw;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    copy->pal->maxColors  = 256;
    copy->pal->numColors  = 0;
    copy->gpal->maxColors = 256;
    copy->gpal->numColors = 0;

    w    = image->width;
    h    = image->height;
    tsw  = image->dataTotalWidth;
    left = image->left;
    top  = image->top;

    size = w*h;

    if (isFlagSet(flags, MFLG_Grey))
    {
        if ((chunky = SharedAlloc(size+size)) == NULL)
            clearFlag(flags, MFLG_Grey);
    }
    else
        chunky = NULL;

    if (chunky == NULL)
        chunky = SharedAlloc(size);

    if (chunky != NULL)
    {
        UBYTE *src, *dest, *alpha = NULL, *gdest;
        ULONG trColor;
        BOOL useAlpha;
        int   x, y;

        if (isFlagClear(flags, MFLG_NtMask))
        {
            if((copy->mask = isFlagSet(flags, MFLG_Cyber) ? ALLOCRASTERCG(w,h) : ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }

        copy->grey = gdest = isFlagSet(flags, MFLG_Grey) ? chunky+size : NULL;

        trColor = image->trColor & 0x00FFFFFF;
        useAlpha = isFlagSet(image->flags, BRFLG_AlphaMask);

        src  = (UBYTE *)image->data+4*left+top*tsw;
        dest = chunky;

        for (y = 0; y<h; y++)
        {
            int bitmask = 0x80, aflag = 0;

            for (x = 0; x<w; x++)
            {
                ULONG c = *((ULONG *)src);

                if (alpha)
                {
                    BOOL hi;

                    if (!aflag)
                    {
                        alpha[x>>3] = 0;
                        aflag = 1;
                    }

                    if (useAlpha)
                        hi = (c & 0xFF000000) ? FALSE : TRUE;
                    else
                        hi = (c & 0x00FFFFFF)==trColor;

                    if (hi)
                        alpha[x>>3] &= ~bitmask;
                    else
                        alpha[x>>3] |= bitmask;

                    if (!(bitmask >>= 1))
                    {
                        bitmask = 0x80;
                        aflag = 0;
                    }
                }

                if (gdest)
                {
                    ULONG v;
                    UBYTE r, g, b;

                    r = (c & 0x00FF0000)>>16;
                    g = (c & 0x0000FF00)>>8;
                    b = (c & 0x000000FF);

                    v = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;

                    *gdest++ = bestColor(copy->gpal,MAKE_ID(0,v,v,v));
                }

                *dest++ = bestColor(copy->pal,c);

                src += 4;
            }

            src += tsw-4*w;
            if (alpha)
                alpha += RAWIDTH(w);
        }
    }

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static UBYTE * getSource(struct MUIS_TheBar_Brush *image)
{
    UBYTE *src;

    ENTER();

    if (image->compressedSize != 0)
    {
        ULONG size = image->dataWidth*image->dataHeight;

        if((src = SharedAlloc(size)) != NULL)
        {
            if (BRCUnpack(image->data, (signed char*)src,image->compressedSize,size) != 0)
            {
                SharedFree(src);
                src = NULL;
            }
        }
    }
    else
    {
        src = image->data;
    }

    RETURN(src);
    return src;
}

/***********************************************************************/

static void freeSource(struct MUIS_TheBar_Brush *image,UBYTE *back)
{
    ENTER();

    if (image->data && image->data!=back)
    {
        SharedFree(image->data);
        image->data = back;
    }

    LEAVE();
}

/***********************************************************************/

static ULONG makeSources(struct InstData *data,struct make *make)
{
    ENTER();

    if (data->image.data)
    {
        struct copy    copy;
        UBYTE *back = data->image.data;

        if (!(data->image.data = getSource(&data->image)))
        {
            data->image.data = back;

            RETURN(FALSE);
            return FALSE;
        }

        copy.dw    = make->dw;
        copy.dh    = make->dh;
        copy.flags = make->flags;

        if (isFlagSet(data->image.flags, BRFLG_ARGB))
        {
            copy.pal  = &make->pal;
            copy.gpal = &make->gpal;
            make->chunky = RGBToLUT8(&data->image,&copy);
        }
        else
            make->chunky = LUT8ToLUT8(&data->image,&copy);

        freeSource(&data->image,back);
        if (!make->chunky)
        {
          RETURN(FALSE);
          return FALSE;
        }

        make->mask    = copy.mask;
        make->gchunky = copy.grey;

        if (isFlagClear(data->userFlags, UFLG_IgnoreSelImages) && DONT_SKIP_PIC(data->simage.data) == TRUE)
        {
            back = data->simage.data;

            if((data->simage.data = getSource(&data->simage)))
            {
                if (isFlagSet(data->simage.flags, BRFLG_ARGB))
                {
                    copy.pal  = &make->spal;
                    copy.gpal = &make->sgpal;
                    make->schunky = RGBToLUT8(&data->simage,&copy);
                }
                else
                    make->schunky = LUT8ToLUT8(&data->simage,&copy);

                freeSource(&data->simage,back);

                make->smask    = copy.mask;
                make->sgchunky = copy.grey;
            }
            else
                data->simage.data = back;
        }

        if (isFlagClear(data->userFlags, UFLG_IgnoreDisImages) && DONT_SKIP_PIC(data->dimage.data) == TRUE)
        {
            back = data->dimage.data;

            if((data->dimage.data = getSource(&data->dimage)))
            {
                if (isFlagSet(data->dimage.flags, BRFLG_ARGB))
                {
                    copy.pal  = &make->dpal;
                    copy.gpal = &make->dgpal;
                    make->dchunky = RGBToLUT8(&data->dimage,&copy);
                }
                else
                    make->dchunky = LUT8ToLUT8(&data->dimage,&copy);

                freeSource(&data->dimage,back);

                make->dmask    = copy.mask;
                make->dgchunky = copy.grey;
            }
            else
                data->dimage.data = back;
        }

        RETURN(TRUE);
        return TRUE;
    }

    RETURN(FALSE);
    return FALSE;
}

/***********************************************************************/

static ULONG
makeSourcesRGB(struct InstData *data,struct make *make)
{
    ENTER();

	if (data->image.data)
    {
        struct copy    copy;
        UBYTE *back = data->image.data;

        if (!(data->image.data = getSource(&data->image)))
        {
            data->image.data = back;
            return FALSE;
        }

	    copy.dw    = make->dw;
        copy.dh    = make->dh;
        copy.flags = make->flags;

        if (isFlagSet(data->image.flags, BRFLG_ARGB))
            make->chunky = RGBToRGB(data, &data->image, &copy);
        else
            make->chunky = LUT8ToRGB(&data->image, &copy);

        freeSource(&data->image,back);
        if(!make->chunky)
        {
          RETURN(FALSE);
          return FALSE;
        }

        make->mask    = copy.mask;
        make->gchunky = copy.grey;

        if (isFlagClear(data->userFlags, UFLG_IgnoreSelImages) && DONT_SKIP_PIC(data->simage.data) == TRUE)
        {
            back = data->simage.data;

            if((data->simage.data = getSource(&data->simage)))
            {
                if (isFlagSet(data->simage.flags, BRFLG_ARGB))
                    make->schunky = RGBToRGB(data, &data->simage, &copy);
                else
                    make->schunky = LUT8ToRGB(&data->simage,&copy);


                freeSource(&data->simage,back);

                make->smask    = copy.mask;
                make->sgchunky = copy.grey;
            }
            else
                data->simage.data = back;
        }

        if (isFlagClear(data->userFlags, UFLG_IgnoreDisImages) && DONT_SKIP_PIC(data->dimage.data) == TRUE)
        {
            back = data->dimage.data;

            if((data->dimage.data = getSource(&data->dimage)))
            {
                if (isFlagSet(data->dimage.flags, BRFLG_ARGB))
                    make->dchunky = RGBToRGB(data, &data->dimage, &copy);
                else
                    make->dchunky = LUT8ToRGB(&data->dimage,&copy);

                freeSource(&data->dimage,back);

                make->dmask    = copy.mask;
                make->dgchunky = copy.grey;
            }
            else
                data->dimage.data = back;
        }

        RETURN(TRUE);
        return TRUE;
    }

    RETURN(FALSE);
    return FALSE;
}

/***********************************************************************/

static struct BitMap *
greyBitMapCyber(struct InstData *data,UBYTE *chunky,UWORD w,UWORD h)
{
    struct BitMap *dest;

    ENTER();

    if (chunky)
    {
        if((dest = AllocBitMap(w,h,24,BMF_MINPLANES,data->screen->RastPort.BitMap)))
        {
            struct RastPort rport;

            InitRastPort(&rport);
            rport.BitMap = dest;

            WritePixelArray(chunky,0,0,w+w+w+w,&rport,0,0,w,h,RECTFMT_ARGB);
        }
    }
    else
        dest = NULL;

    RETURN(dest);
    return dest;
}

/***********************************************************************/

static void
buildBitMapsCyber(struct InstData *data)
{
    struct make     *make;
    ULONG  flags = data->flags;
    UWORD  w, h;

    ENTER();

    if(!(make = SharedAlloc(sizeof(struct make))))
    {
      LEAVE();
      return;
    }

    memset(make,0,sizeof(struct make));

    w = make->dw = data->image.width;
    h = make->dh = data->image.height;

    if ((flags & FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny) || (data->disMode==MUIV_TheBar_DisMode_BlendGrey))
        setFlag(make->flags, MFLG_Grey);
    setFlag(make->flags, MFLG_Cyber);

    if (!makeSourcesRGB(data,make))
    {
        SharedFree(make);

        LEAVE();
        return;
    }

    if((data->strip.normalBM = AllocBitMap(w,h,24,BMF_MINPLANES,data->screen->RastPort.BitMap)))
    {
        struct RastPort rport;
        UWORD  tw;

        InitRastPort(&rport);
        rport.BitMap = data->strip.normalBM;

        tw = w+w+w+w;

        WritePixelArray(make->chunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

        data->strip.mask = make->mask;
        if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny))
            data->strip.greyBM = greyBitMapCyber(data,make->gchunky,w,h);

        if (make->schunky)
        {
            if((data->strip.snormalBM = AllocBitMap(w,h,24,BMF_MINPLANES,data->screen->RastPort.BitMap)))
            {
                InitRastPort(&rport);
                rport.BitMap = data->strip.snormalBM;

                WritePixelArray(make->schunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

                data->strip.smask = make->smask;
                if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny))
                    data->strip.sgreyBM = greyBitMapCyber(data,make->sgchunky,w,h);
            }
        }

        if (make->dchunky)
        {
            if((data->strip.dnormalBM = AllocBitMap(w,h,24,BMF_MINPLANES,data->screen->RastPort.BitMap)))
            {
                InitRastPort(&rport);
                rport.BitMap = data->strip.dnormalBM;

                WritePixelArray(make->dchunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

                data->strip.dmask = make->dmask;
                if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny))
                    data->strip.dgreyBM = greyBitMapCyber(data,make->dgchunky,w,h);
            }
        }

        WaitBlit();
    }

    #if defined(WITH_ALPHA)
    if (isFlagSet(data->image.flags, BRFLG_AlphaMask))
    #else
    if (data->allowAlphaChannel && isFlagSet(data->image.flags, BRFLG_AlphaMask))
    #endif
    {
        data->strip.nchunky  = make->chunky;
        data->strip.gchunky  = make->gchunky;

        data->strip.snchunky = make->schunky;
        data->strip.sgchunky = make->sgchunky;

        data->strip.dnchunky = make->dchunky;
        data->strip.dgchunky = make->dgchunky;
    }
    else
    {
        // free unused chunky blocks
        if (make->chunky)
            SharedFree(make->chunky);
        if (make->schunky)
            SharedFree(make->schunky);
        if (make->dchunky)
            SharedFree(make->dchunky);
    }

    SharedFree(make);

    LEAVE();
}

/***********************************************************************/

#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
#define WCP(rp, xstart, ystart, xstop, ystop, array, bpr) WriteChunkyPixels(rp, xstart, ystart, xstop, ystop, array, bpr)
#else // __amigaos4 || __MORPHOS__ || __AROS__
static void _WriteChunkyPixels(struct RastPort *rp, UWORD xstart, UWORD ystart, UWORD xstop, UWORD ystop, const UBYTE *array, LONG bytesPerRow)
{
  UWORD y;
  const UBYTE *cptr = &array[ystart*bytesPerRow];

  for(y = ystart; y <= ystop; y++)
  {
    UWORD x;

    for(x = xstart; x <= xstop; x++)
    {
      SetAPen(rp, cptr[x]);
      WritePixel(rp, x, y);
    }

    cptr += bytesPerRow;
  }
}

#define WCP(rp, xstart, ystart, xstop, ystop, array, bpr) \
{ \
  if(setPatchVersion >= ((43UL << 16) | 0UL)) \
    WriteChunkyPixels(rp, xstart, ystart, xstop, ystop, array, bpr); \
  else \
    _WriteChunkyPixels(rp, xstart, ystart, xstop, ystop, array, bpr); \
}
#endif // __amigaos4 || __MORPHOS__ || __AROS__

static struct BitMap *
LUT8ToBitMap(struct InstData *data,
             UBYTE *src,
             UWORD width,
             UWORD height,
             ULONG *colors,
             BOOL RGB8,
             struct pen *pens)
{
    ULONG flags;
    struct BitMap *friend;
    struct BitMap *dest;

    ENTER();

    flags = BMF_CLEAR;
    friend = NULL;
    if(isFlagSet(data->flags, FLG_CyberMap))
    {
      setFlag(flags, BMF_MINPLANES);
      friend = data->screen->RastPort.BitMap;
    }

    if((dest = AllocBitMap(width,height, MIN(8, data->screenDepth), flags, friend)) != NULL)
    {
        struct RastPort rport;

        if (colors && pens)
        {
            struct ColorMap *cm = data->screen->ViewPort.ColorMap;
            UBYTE           *buf;
            int             i;
            struct TagItem  tags[] = { { OBP_Precision, 0 },
                                       { TAG_DONE,      0 } };

            tags[0].ti_Data = data->precision;

            for (buf = src, i = width*height; --i;)
            {
                UBYTE p = *buf;

                if (!pens[p].done)
                {
                    ULONG *c = colors+p+(RGB8 ? 0 : p+p);

                    if (RGB8)
                    {
                        ULONG cv = *c;

                        pens[p].pen  = ObtainBestPenA(cm,0x01010101 * ((cv & 0x00FF0000)>>16),0x01010101 * ((cv & 0x0000FF00)>>8),0x01010101 * (cv & 0x000000FF),tags);
                    }
                    else
                    {
                        pens[p].pen  = ObtainBestPenA(cm,c[0],c[1],c[2],tags);
                    }

                    pens[p].done = 1;
                }

                *buf++ = pens[p].pen;
            }
        }

        InitRastPort(&rport);
        rport.BitMap = dest;

        if (isFlagSet(data->flags, FLG_CyberMap))
            WritePixelArray(src,0,0,width,&rport,0,0,width,height,RECTFMT_LUT8);
        else
            WCP(&rport,0,0,width-1,height-1,src,width);
    }

    RETURN(dest);
    return dest;
}

/****************************************************************************/

static struct BitMap *
greyBitMap(struct InstData *data,
           UBYTE *src,
           UWORD width,
           UWORD height,
           ULONG *colors,
           ULONG numColors,
           BOOL RGB8,
           struct pen *pens)
{
    struct BitMap *result = NULL;

    ENTER();

    if (src)
    {
        ULONG greyColors[3*256];
        ULONG *gc;
        int   i;

        gc = greyColors;

        for (i = numColors; i--; )
        {
            ULONG gcol;
            UBYTE r, g, b, v;

            if (RGB8)
            {
                ULONG cv = *colors++;

                r = (cv & 0x00FF0000)>>16;
                g = (cv & 0x0000FF00)>>8;
                b = (cv & 0x000000FF);
            }
            else
            {
                r = *colors++ >>24;
                g = *colors++ >>24;
                b = *colors++ >>24;
            }

            v = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
            gcol = MAKE_ID(v,v,v,v);
            *gc++ = gcol;
            *gc++ = gcol;
            *gc++ = gcol;
        }

        result = LUT8ToBitMap(data,src,width,height,greyColors,FALSE,pens);
    }

    RETURN(result);
    return result;
}

/***********************************************************************/

static void
buildBitMaps(struct InstData *data)
{
    struct make              *make;
    struct MUIS_TheBar_Brush *image = &data->image;
    ULONG           flags = data->flags;
    UWORD           w, h;

    ENTER();

    if (!(make = SharedAlloc(sizeof(struct make))))
    {
      LEAVE();
      return;
    }

    memset(make,0,sizeof(struct make));

    w = make->dw = image->width;
    h = make->dh = image->height;

    if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny) || (data->disMode==MUIV_TheBar_DisMode_BlendGrey))
        setFlag(make->flags, MFLG_Grey);

    if (isFlagSet(flags, FLG_CyberMap))
        setFlag(make->flags, MFLG_Cyber);

    if (!makeSources(data,make))
    {
        SharedFree(make);

        LEAVE();
        return;
    }

    if (isFlagSet(data->image.flags, BRFLG_ARGB))
        data->strip.normalBM = LUT8ToBitMap(data,make->chunky,w,h,make->pal.colors,TRUE,data->pens);
    else
        data->strip.normalBM = LUT8ToBitMap(data,make->chunky,w,h,image->colors,isFlagSet(image->flags, BRFLG_ColorRGB8),data->pens);

    if (data->strip.normalBM)
    {
        struct MUIS_TheBar_Brush *simage = &data->simage, *dimage = &data->dimage;

        data->strip.mask = make->mask;

        if (make->schunky)
        {
            if (isFlagSet(data->simage.flags, BRFLG_ARGB))
                data->strip.snormalBM = LUT8ToBitMap(data,make->schunky,w,h,make->spal.colors,TRUE,data->spens);
            else
                data->strip.snormalBM = LUT8ToBitMap(data,make->schunky,w,h,simage->colors,isFlagSet(simage->flags, BRFLG_ColorRGB8),data->spens);
            data->strip.smask = make->smask;
        }

        if (make->dchunky)
        {
            if (isFlagSet(data->simage.flags, BRFLG_ARGB))
                data->strip.dnormalBM = LUT8ToBitMap(data,make->dchunky,w,h,make->dpal.colors,TRUE,data->dpens);
            else
                data->strip.dnormalBM = LUT8ToBitMap(data,make->dchunky,w,h,dimage->colors,isFlagSet(dimage->flags, BRFLG_ColorRGB8),data->dpens);
            data->strip.dmask = make->dmask;
        }

        if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheBar_DisMode_Sunny))
        {
            if(make->gchunky)
            {
              if(isFlagSet(data->image.flags, BRFLG_ARGB))
                data->strip.greyBM = LUT8ToBitMap(data,make->gchunky,w,h,make->gpal.colors,TRUE,data->gpens);
              else
                data->strip.greyBM = greyBitMap(data,make->gchunky,w,h,image->colors,image->numColors,isFlagSet(image->flags, BRFLG_ColorRGB8),data->gpens);
            }

            if(make->sgchunky)
            {
              if(isFlagSet(data->simage.flags, BRFLG_ARGB))
                data->strip.sgreyBM = LUT8ToBitMap(data,make->sgchunky,w,h,make->sgpal.colors,TRUE,data->sgpens);
              else
                data->strip.sgreyBM = greyBitMap(data,make->sgchunky,w,h,simage->colors,simage->numColors,isFlagSet(simage->flags, BRFLG_ColorRGB8),data->sgpens);
            }

            if(make->dgchunky)
            {
              if(isFlagSet(data->dimage.flags, BRFLG_ARGB))
                data->strip.dgreyBM = LUT8ToBitMap(data,make->dgchunky,w,h,make->dgpal.colors,TRUE,data->dgpens);
              else
                data->strip.dgreyBM = greyBitMap(data,make->dgchunky,w,h,dimage->colors,dimage->numColors,isFlagSet(dimage->flags, BRFLG_ColorRGB8),data->dgpens);
            }
        }

        WaitBlit();
    }

    if (make->chunky)
        SharedFree(make->chunky);
    if (make->schunky)
        SharedFree(make->schunky);
    if (make->dchunky)
        SharedFree(make->dchunky);

    SharedFree(make);

    LEAVE();
}

/***********************************************************************/

void
build(struct InstData *data)
{
    ENTER();

    if (isFlagSet(data->flags, FLG_CyberDeep))
        buildBitMapsCyber(data);
    else
        buildBitMaps(data);

    LEAVE();
}

/***********************************************************************/

void
freeBitMaps(struct InstData *data)
{
    struct MUIS_TheBar_Strip *strip = &data->strip;

    ENTER();

    #if defined(WITH_ALPHA)
    if (isFlagSet(data->image.flags, BRFLG_AlphaMask))
    #else
    if (data->allowAlphaChannel && isFlagSet(data->image.flags, BRFLG_AlphaMask))
    #endif
    {
        if (data->strip.nchunky)
            SharedFree(data->strip.nchunky);

        if (data->strip.snchunky)
            SharedFree(data->strip.snchunky);

        if (data->strip.dnchunky)
            SharedFree(data->strip.dnchunky);
    }

    if(strip->normalBM)
    {
        if (isFlagClear(data->flags, FLG_CyberDeep))
        {
            struct ColorMap *cm = data->screen->ViewPort.ColorMap;
            struct pen      *pens, *gpens, *spens, *sgpens, *dpens, *dgpens;
            int             i;

            pens  = data->pens;
            gpens = (strip->greyBM) ? data->gpens  : NULL;

            if((spens = (strip->snormalBM) ? data->spens : NULL))
              sgpens = (strip->sgreyBM) ? data->sgpens : NULL;
            else
              sgpens = NULL;

            if((dpens = (strip->dnormalBM) ? data->dpens : NULL))
              dgpens = (strip->dgreyBM) ? data->dgpens : NULL;
            else
              dgpens = NULL;

            for (i = 256; i--; )
            {
                if (pens)
                {
                    if (pens->done)
                    {
                        ReleasePen(cm,pens->pen);
                        pens->done = 0;
                    }

                    pens++;
                }

                if (gpens)
                {
                    if (gpens->done)
                    {
                        ReleasePen(cm,gpens->pen);
                        gpens->done = 0;
                    }

                    gpens++;
                }

                if (spens)
                {
                    if (sgpens)
                    {
                        if (sgpens->done)
                        {
                            ReleasePen(cm,sgpens->pen);
                            sgpens->done = 0;
                        }

                        sgpens++;
                    }

                    if (spens->done)
                    {
                        ReleasePen(cm,spens->pen);
                        spens->done = 0;
                    }

                    spens++;
                }

                if (dpens)
                {
                    if (dgpens)
                    {
                        if (dgpens->done)
                        {
                            ReleasePen(cm,dgpens->pen);
                            dgpens->done = 0;
                        }

                        dgpens++;
                    }

                    if (dpens->done)
                    {
                        ReleasePen(cm,dpens->pen);
                        dpens->done = 0;
                    }

                    dpens++;
                }
            }
        }

        if (strip->greyBM)
            FreeBitMap(strip->greyBM);

        if (strip->mask)
        {
            if (isFlagSet(data->flags, FLG_CyberMap))
                FREERASTERCG(strip->mask);
            else
                FREERASTER(strip->mask);
        }

        FreeBitMap(strip->normalBM);

        if (strip->snormalBM)
        {
            if (strip->sgreyBM)
                FreeBitMap(strip->sgreyBM);

            if (strip->smask)
            {
                if (isFlagSet(data->flags, FLG_CyberMap))
                    FREERASTERCG(strip->smask);
                else
                    FREERASTER(strip->smask);
            }

            FreeBitMap(strip->snormalBM);
        }

        if (strip->dnormalBM)
        {
            if (strip->dgreyBM)
                FreeBitMap(strip->dgreyBM);

            if (strip->dmask)
            {
                if (isFlagSet(data->flags, FLG_CyberMap))
                    FREERASTERCG(strip->dmask);
                else
                    FREERASTER(strip->dmask);
            }

            FreeBitMap(strip->dnormalBM);
        }
    }

    memset(strip,0,sizeof(*strip));

    LEAVE();
}

/***********************************************************************/
