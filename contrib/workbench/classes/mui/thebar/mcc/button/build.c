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

#define ALLOCRASTER(w,h)   AllocVec(RAWIDTH(w)*((UWORD)(h)),MEMF_CHIP|MEMF_CLEAR)
#define FREERASTER(ra)     FreeVec(ra)

#define ALLOCRASTERCG(w,h) SharedAlloc(RAWIDTH(w)*((UWORD)(h)))
#define FREERASTERCG(ra)   SharedFree(ra)

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
    MFLG_Grey    = 1<<0,
    MFLG_Scaled  = 1<<1,
    MFLG_NtMask  = 1<<2,
    MFLG_Cyber   = 1<<3,
};

/***********************************************************************/

static UBYTE *
LUT8ToLUT8(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size, maskDone = FALSE;
    UWORD w, h;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    if (isFlagSet(flags, MFLG_Scaled))
    {
        w = copy->dw;
        h = copy->dh;
    }
    else
    {
        w = image->width;
        h = image->height;
    }

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

/*
        if (isFlagClear(flags, MFLG_NtMask))
        {
            if (isFlagSet(flags, MFLG_Cyber))
            {
                ULONG len = RAWIDTH(w)*h;

                if((copy->mask = SharedAlloc(len)))
                {
                    alpha = copy->mask;
                    memset(alpha,0,len);
                }
            }
            else if((copy->mask = ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }
*/

        if (isFlagSet(flags, MFLG_Scaled))
        {
            struct scale sce;

            sce.w  = image->dataWidth;
            sce.tw = image->dataTotalWidth;
            sce.sl = image->left;
            sce.st = image->top;
            sce.sw = image->width;
            sce.sh = image->height;
            sce.dw = copy->dw;
            sce.dh = copy->dh;

            D(DBF_STARTUP, "Scaling 1...");
            scale(&sce,image->data,chunky);
        }
        else
        {
            if (image->left!=0 || image->top!=0 || image->width!=image->dataWidth || image->height!=image->dataHeight || image->dataWidth!=image->dataTotalWidth)
            {
                ULONG trColor;
                UWORD tsw;
                UBYTE *src, *dest;
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

                            if (p!=trColor)
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

                maskDone = TRUE;
            }
            else
                memcpy(chunky,image->data,size);
        }

        if (isFlagSet(flags, MFLG_Grey))
            memcpy(copy->grey = chunky+size,chunky,size);

        if (!maskDone && alpha)
        {
            ULONG trColor;
            UBYTE *src;
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

                    if (*src++!=trColor)
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

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static UBYTE *
LUT8ToRGB(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *from, *chunky;
    ULONG flags = copy->flags, size;
    UWORD w, h, left, top, tsw;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    if (isFlagSet(flags, MFLG_Scaled))
    {
        struct copy    temp;
        UBYTE *cchunky;

        memcpy(&temp,copy,sizeof(temp));
        temp.flags = MFLG_Scaled|MFLG_NtMask;
        if ((cchunky = LUT8ToLUT8(image,&temp)) == NULL)
        {
            RETURN(NULL);
            return NULL;
        }

        from  = cchunky;
        w     = copy->dw;
        h     = copy->dh;
        tsw   = w;
        left  = 0;
        top   = 0;
    }
    else
    {
        from  = image->data;
        w     = image->width;
        h     = image->height;
        tsw   = image->dataTotalWidth;
        left  = image->left;
        top   = image->top;
    }

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
        int   x, y;

        if (isFlagClear(flags, MFLG_NtMask))
        {
            if((copy->mask = isFlagSet(flags, MFLG_Cyber) ? ALLOCRASTERCG(w,h) : ALLOCRASTER(w,h)))
              alpha = copy->mask;
        }

        copy->grey = gdest = isFlagSet(flags, MFLG_Grey) ? chunky+size : NULL;

        colors   = image->colors;
        trColor  = image->trColor;

        src  = from+left+top*tsw;
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

                    if (p!=trColor)
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

                    gcol =(((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
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

    if (isFlagSet(flags, MFLG_Scaled))
        SharedFree(from);

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

#if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
static UBYTE *
RGBToRGB(struct MUIS_TheBar_Brush *image,struct copy *copy)
#else
static UBYTE *
RGBToRGB(struct MUIS_TheBar_Brush *image,struct copy *copy, BOOL allowAlphaChannel)
#endif
{
    UBYTE *chunky;
    ULONG flags = copy->flags, size, maskDone = FALSE;
    UWORD w, h;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    if (isFlagSet(flags, MFLG_Scaled))
    {
        w = copy->dw;
        h = copy->dh;
    }
    else
    {
        w = image->width;
        h = image->height;
    }

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

        if (isFlagSet(flags, MFLG_Scaled))
        {
            struct scale sce;

            sce.w  = image->dataWidth;
            sce.tw = image->dataTotalWidth;
            sce.sl = image->left;
            sce.st = image->top;
            sce.sw = image->width;
            sce.sh = image->height;
            sce.dw = copy->dw;
            sce.dh = copy->dh;

            D(DBF_STARTUP, "Scaling 2...");
            scaleRGB(&sce,(ULONG *)image->data,(ULONG *)chunky);
        }
        else
        {
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
                            BOOL hi;

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
                            	hi = (allowAlphaChannel ? *src<0xFF : !(c & 0xFF000000));
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
                            gdest++;
                            *gdest++ = v;
                            *gdest++ = v;
                            *gdest++ = v;
                        }

                        *dest++ = *src++;
                        *dest++ = *src++;
                        *dest++ = *src++;
                        *dest++ = *src++;
                    }

                    src += tsw-4*w;
                    if (alpha)
                        alpha += RAWIDTH(w);
                }

                if (!reallyHasAlpha)
                    setFlag(image->flags, BRFLG_EmptyAlpha);
                else
                    clearFlag(image->flags, BRFLG_EmptyAlpha);

                maskDone = TRUE;
            }
            else
                memcpy(chunky,image->data,size);
        }

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
                        if (useAlpha) hi = *src<0xFF;
                        #else
                        if (useAlpha) hi = (allowAlphaChannel ? *src<0xFF : !(c & 0xFF000000));
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
    ULONG i, d, bestd = 196000;
    UWORD besti = 0, r, g, b, dr, dg, db;

    ENTER();

    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);

    for (i = 0; i<pal->numColors; i++)
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
        if (pal->colors[i]==rgb)
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

static UBYTE *
RGBToLUT8(struct MUIS_TheBar_Brush *image,struct copy *copy)
{
    UBYTE *from, *chunky;
    ULONG flags = copy->flags, size;
    UWORD w, h, left, top, tsw;

    ENTER();

    copy->mask = NULL;
    copy->grey = NULL;

    copy->pal->maxColors  = 256;
    copy->pal->numColors  = 0;
    copy->gpal->maxColors = 256;
    copy->gpal->numColors = 0;

    if (isFlagSet(flags, MFLG_Scaled))
    {
        if((from = SharedAlloc(4*copy->dw*copy->dh)))
        {
            struct scale sce;

            sce.w  = image->dataWidth;
            sce.tw = image->dataTotalWidth;
            sce.sl = image->left;
            sce.st = image->top;
            sce.sw = image->width;
            sce.sh = image->height;
            sce.dw = copy->dw;
            sce.dh = copy->dh;

            D(DBF_STARTUP, "Scaling 3...");
            scaleRGB(&sce,(ULONG *)image->data,(ULONG *)from);

            w    = copy->dw;
            h    = copy->dh;
            tsw  = 4*w;
            left = 0;
            top  = 0;
        }
        else return NULL;
    }
    else
    {
        from = image->data;

        w    = image->width;
        h    = image->height;
        tsw  = image->dataTotalWidth;
        left = image->left;
        top  = image->top;
    }

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

        src  = from+4*left+top*tsw;
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

                    if (useAlpha)
                        hi = !(c & 0xFF000000);
                    else
                        hi = (c & 0x00FFFFFF)==trColor;

                    if (!hi)
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

    if (from!=image->data)
        SharedFree(from);

    RETURN(chunky);
    return chunky;
}

/***********************************************************************/

static APTR
getSource(struct MUIS_TheBar_Brush *image)
{
    APTR src;

    ENTER();

    if (image->compressedSize != 0)
    {
        ULONG size = image->dataTotalWidth*image->dataHeight;

        if (isFlagSet(image->flags, BRFLG_ARGB))
            size *= 4;

        if ((src = SharedAlloc(size)) != NULL)
        {
            if(BRCUnpack(image->data,src,image->compressedSize,size) != 0)
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

static void
freeSource(struct MUIS_TheBar_Brush *image,UBYTE *back)
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

static BOOL
makeSources(struct InstData *data,struct make *make)
{
    ENTER();

    if (data->image->data)
    {
        struct copy    copy;
        UBYTE *back = data->image->data;

        if (!(data->image->data = getSource(data->image)))
        {
            data->image->data = back;

            RETURN(FALSE);
            return FALSE;
        }

        copy.dw    = make->dw;
        copy.dh    = make->dh;
        copy.flags = make->flags;

        if (isFlagSet(data->image->flags, BRFLG_ARGB))
        {
            copy.pal  = &make->pal;
            copy.gpal = &make->gpal;
            make->chunky = RGBToLUT8(data->image,&copy);
        }
        else
            make->chunky = LUT8ToLUT8(data->image,&copy);

        freeSource(data->image,back);
        if (!make->chunky)
        {
            RETURN(FALSE);
            return FALSE;
        }

        make->mask    = copy.mask;
        make->gchunky = copy.grey;

        if (isFlagClear(data->userFlags, UFLG_IgnoreSelImages) && DONT_SKIP_PIC(data->simage) == TRUE)
        {
            back = data->simage->data;

            if((data->simage->data = getSource(data->simage)))
            {
                if (isFlagSet(data->simage->flags, BRFLG_ARGB))
                {
                    copy.pal  = &make->spal;
                    copy.gpal = &make->sgpal;
                    make->schunky = RGBToLUT8(data->simage,&copy);
                }
                else
                    make->schunky = LUT8ToLUT8(data->simage,&copy);

                freeSource(data->simage,back);

                make->smask    = copy.mask;
                make->sgchunky = copy.grey;
            }
            else
                data->image->data = back;
        }

        if (isFlagClear(data->userFlags, UFLG_IgnoreDisImages) && DONT_SKIP_PIC(data->dimage) == TRUE)
        {
            back = data->dimage->data;

            if((data->dimage->data = getSource(data->dimage)))
            {
                if (isFlagSet(data->dimage->flags, BRFLG_ARGB))
                {
                    copy.pal  = &make->dpal;
                    copy.gpal = &make->dgpal;
                    make->dchunky = RGBToLUT8(data->dimage,&copy);
                }
                else
                    make->dchunky = LUT8ToLUT8(data->dimage,&copy);

                freeSource(data->dimage,back);

                make->dmask    = copy.mask;
                make->dgchunky = copy.grey;
            }
            else
                data->image->data = back;
        }

        RETURN(TRUE);
        return TRUE;
    }

    RETURN(FALSE);
    return FALSE;
}

/***********************************************************************/

static BOOL
makeSourcesRGB(struct InstData *data,struct make *make)
{
    ENTER();

    if (data->image->data)
    {
        struct copy    copy;
        UBYTE *back = data->image->data;

        if ((data->image->data = getSource(data->image)) == NULL)
        {
            data->image->data = back;

            RETURN(FALSE);
            return FALSE;
        }

        copy.dw    = make->dw;
        copy.dh    = make->dh;
        copy.flags = make->flags;

        if (isFlagSet(data->image->flags, BRFLG_ARGB))
        #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
            make->chunky = RGBToRGB(data->image,&copy);
        #else
            make->chunky = RGBToRGB(data->image,&copy,data->allowAlphaChannel);
        #endif
        else
            make->chunky = LUT8ToRGB(data->image,&copy);

        freeSource(data->image,back);
        if (!make->chunky)
        {
            RETURN(FALSE);
            return FALSE;
        }

        make->mask    = copy.mask;
        make->gchunky = copy.grey;

        if (isFlagClear(data->userFlags, UFLG_IgnoreSelImages) && DONT_SKIP_PIC(data->simage) == TRUE)
        {
            back = data->simage->data;

            if((data->simage->data = getSource(data->simage)))
            {
                if (isFlagSet(data->simage->flags, BRFLG_ARGB))
                #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                    make->schunky = RGBToRGB(data->simage,&copy);
                #else
                    make->schunky = RGBToRGB(data->simage,&copy,data->allowAlphaChannel);
                #endif
                else
                    make->schunky = LUT8ToRGB(data->simage,&copy);

                freeSource(data->simage,back);
                make->smask    = copy.mask;
                make->sgchunky = copy.grey;
            }
            else
                data->simage->data = back;
        }

        if (isFlagClear(data->userFlags, UFLG_IgnoreDisImages) && DONT_SKIP_PIC(data->dimage) == TRUE)
        {
            back = data->dimage->data;

            if((data->dimage->data = getSource(data->dimage)))
            {
                if (isFlagSet(data->dimage->flags, BRFLG_ARGB))
                #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
                    make->dchunky = RGBToRGB(data->dimage,&copy);
                #else
                    make->dchunky = RGBToRGB(data->dimage,&copy,data->allowAlphaChannel);
                #endif
                else
                    make->dchunky = LUT8ToRGB(data->dimage,&copy);

                freeSource(data->dimage,back);
                make->dmask    = copy.mask;
                make->dgchunky = copy.grey;
            }
            else
                data->dimage->data = back;
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

    if ((make = SharedAlloc(sizeof(struct make))) == NULL)
    {
        LEAVE();
        return;
    }
    memset(make,0,sizeof(struct make));

    if (isFlagSet(flags, FLG_Scaled))
    {
        w = ScalerDiv(data->image->width,data->scale,100);
        h = ScalerDiv(data->image->height,data->scale,100);
        make->flags = MFLG_Scaled;
    }
    else
    {
        w = data->image->width;
        h = data->image->height;
    }

    data->imgWidth  = make->dw = w;
    data->imgHeight = make->dh = h;

    if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
        setFlag(make->flags, MFLG_Grey);
    setFlag(make->flags, MFLG_Cyber);

    if (makeSourcesRGB(data,make) == FALSE)
    {
        SharedFree(make);

        LEAVE();
        return;
    }

    if((data->normalBM = AllocBitMap(w,h,24,BMF_MINPLANES|BMF_CLEAR,data->screen->RastPort.BitMap)))
    {
        struct RastPort rport;
        UWORD  tw;

        InitRastPort(&rport);
        rport.BitMap = data->normalBM;

        tw = w+w+w+w;

        WritePixelArray(make->chunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

        data->mask = make->mask;
        if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
             data->greyBM = greyBitMapCyber(data,make->gchunky,w,h);

        if (make->schunky)
        {
            if((data->snormalBM = AllocBitMap(w,h,24,BMF_MINPLANES|BMF_CLEAR,data->screen->RastPort.BitMap)))
            {
                InitRastPort(&rport);
                rport.BitMap = data->snormalBM;

                WritePixelArray(make->schunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

                data->smask = make->smask;
                if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
                    data->sgreyBM = greyBitMapCyber(data,make->sgchunky,w,h);
            }
        }

        if (make->dchunky)
        {
            if((data->dnormalBM = AllocBitMap(w,h,24,BMF_MINPLANES|BMF_CLEAR,data->screen->RastPort.BitMap)))
            {
                InitRastPort(&rport);
                rport.BitMap = data->dnormalBM;

                WritePixelArray(make->dchunky,0,0,tw,&rport,0,0,w,h,RECTFMT_ARGB);

                data->dmask = make->dmask;
                if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
                    data->dgreyBM = greyBitMapCyber(data,make->dgchunky,w,h);
            }
        }

        WaitBlit();
    }

    D(DBF_STARTUP, "%lx", isFlagSet(data->image->flags, BRFLG_AlphaMask));
    #if defined(WITH_ALPHA)
    if (isFlagSet(data->image->flags, BRFLG_AlphaMask))
    #else
    if (data->allowAlphaChannel && isFlagSet(data->image->flags, BRFLG_AlphaMask))
    #endif
    {
        data->nchunky  = make->chunky;
        data->gchunky  = make->gchunky;

        data->snchunky = make->schunky;
        data->sgchunky = make->sgchunky;

        data->dnchunky = make->dchunky;
        data->dgchunky = make->dgchunky;
    }
    else
    {
        // free unused chunky blocks
        if(make->chunky)
            SharedFree(make->chunky);
        if(make->schunky)
            SharedFree(make->schunky);
        if(make->dchunky)
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
             ULONG numColors,
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

    if((dest = AllocBitMap(width, height, MIN(8, data->screenDepth), flags, friend)) != NULL)
    {
        struct RastPort rport;

        if (colors && numColors && pens)
        {
            struct ColorMap *cm = data->screen->ViewPort.ColorMap;
            UBYTE           *buf;
            int             i;
            struct TagItem  tags[] =  { { OBP_Precision, 0 },
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
    ENTER();

    if (src)
    {
        ULONG greyColors[3*256];
        ULONG *gc;
        int   i;
        struct BitMap *bm;

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
                r = *colors++ >> 24;
                g = *colors++ >> 24;
                b = *colors++ >> 24;
            }

            v = (((r<<5)+(r<<2)+(r<<1))+((g<<6)+(g<<3)+(g<<2))+((b<<4)-(b<<1)))>>7;
            gcol =MAKE_ID(v,v,v,v);
            *gc++ = gcol;
            *gc++ = gcol;
            *gc++ = gcol;
        }

        bm = LUT8ToBitMap(data,src,width,height,greyColors,numColors,FALSE,pens);

        RETURN(bm);
        return bm;
    }

    RETURN(NULL);
    return NULL;
}

/***********************************************************************/

static void
buildBitMaps(struct InstData *data)
{
    struct make              *make;
    struct MUIS_TheBar_Brush *image = data->image;
    ULONG           flags = data->flags;
    UWORD           w, h;

    ENTER();

    if ((make = SharedAlloc(sizeof(struct make))) == NULL)
    {
        LEAVE();
        return;
    }

    memset(make,0,sizeof(struct make));

    if (isFlagSet(flags, FLG_Scaled))
    {
        w = ScalerDiv(image->width,data->scale,100);
        h = ScalerDiv(image->height,data->scale,100);
        setFlag(make->flags, MFLG_Scaled);
    }
    else
    {
        w = image->width;
        h = image->height;
    }

    data->imgWidth  = make->dw = w;
    data->imgHeight = make->dh = h;

    if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
        setFlag(make->flags, MFLG_Grey);
    if (isFlagSet(data->flags, FLG_CyberMap))
        setFlag(make->flags, MFLG_Cyber);

    if (!makeSources(data,make))
    {
        SharedFree(make);
        LEAVE();
        return;
    }

    if (isFlagSet(data->image->flags, BRFLG_ARGB))
        data->normalBM = LUT8ToBitMap(data,make->chunky,w,h,make->pal.colors,make->pal.numColors,TRUE,data->pens);
    else
        data->normalBM = LUT8ToBitMap(data,make->chunky,w,h,image->colors,image->numColors,isFlagSet(image->flags, BRFLG_ColorRGB8),data->pens);

    if (data->normalBM)
    {
        struct MUIS_TheBar_Brush *simage = data->simage, *dimage = data->dimage;

        data->mask = make->mask;

        if (make->schunky)
        {
            if (isFlagSet(data->simage->flags, BRFLG_ARGB))
                data->snormalBM = LUT8ToBitMap(data,make->schunky,w,h,make->spal.colors,make->spal.numColors,TRUE,data->spens);
            else
                data->snormalBM = LUT8ToBitMap(data,make->schunky,w,h,simage->colors,simage->numColors,isFlagSet(simage->flags, BRFLG_ColorRGB8),data->spens);
            data->smask = make->smask;
        }

        if (make->dchunky)
        {
            if (isFlagSet(data->dimage->flags, BRFLG_ARGB))
                data->dnormalBM = LUT8ToBitMap(data,make->dchunky,w,h,make->dpal.colors,make->dpal.numColors,TRUE,data->dpens);
            else
                data->dnormalBM = LUT8ToBitMap(data,make->dchunky,w,h,dimage->colors,dimage->numColors,isFlagSet(dimage->flags, BRFLG_ColorRGB8),data->dpens);
            data->dmask = make->dmask;
        }

        if (isFlagSet(flags, FLG_Sunny) || (data->disMode==MUIV_TheButton_DisMode_Sunny) || (data->disMode==MUIV_TheButton_DisMode_BlendGrey))
        {
            if (make->gchunky)
            {
                if (isFlagSet(data->image->flags, BRFLG_ARGB))
                    data->greyBM = LUT8ToBitMap(data,make->gchunky,w,h,make->gpal.colors,make->gpal.numColors,TRUE,data->gpens);
                else
                    data->greyBM = greyBitMap(data,make->gchunky,w,h,image->colors,image->numColors,isFlagSet(image->flags, BRFLG_ColorRGB8),data->gpens);
            }

            if (make->sgchunky)
            {
                if (isFlagSet(data->simage->flags, BRFLG_ARGB))
                    data->sgreyBM = LUT8ToBitMap(data,make->sgchunky,w,h,make->sgpal.colors,make->sgpal.numColors,TRUE,data->sgpens);
                else
                    data->sgreyBM = greyBitMap(data,make->sgchunky,w,h,simage->colors,simage->numColors,isFlagSet(simage->flags, BRFLG_ColorRGB8),data->sgpens);
            }

            if (make->dgchunky)
            {
                if (isFlagSet(data->dimage->flags, BRFLG_ARGB))
                    data->dgreyBM = LUT8ToBitMap(data,make->dgchunky,w,h,make->dgpal.colors,make->dgpal.numColors,TRUE,data->dgpens);
                else
                    data->dgreyBM = greyBitMap(data,make->dgchunky,w,h,dimage->colors,dimage->numColors,isFlagSet(dimage->flags, BRFLG_ColorRGB8),data->dgpens);
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

struct scaleBitMap
{
    UWORD sx;
    UWORD sy;
    UWORD sw;
    UWORD sh;
    UWORD dw;
    UWORD dh;
    ULONG ratio;
    UWORD tsw;
    UWORD tsh;
};

enum
{
    SCALEFLG_Mask      = 1<<0,
    SCALEFLG_CyberMap  = 1<<1,
    SCALEFLG_CyberDeep = 1<<2,
};

static struct BitMap *
scaleBitMap(APTR src,struct scaleBitMap *scale,ULONG flags)
{
    struct BitMap msbm, mdbm;
    struct BitMap *sbm=NULL;
    struct BitMap *dbm=NULL;
    APTR          dest;

    ENTER();

    if (isFlagSet(flags, SCALEFLG_Mask))
    {
        if((dest = isFlagSet(flags, SCALEFLG_CyberMap) ? ALLOCRASTERCG(scale->dw,scale->dh) : ALLOCRASTER(scale->dw,scale->dh)))
        {
            memset(&msbm,0,sizeof(msbm));
            msbm.BytesPerRow = RAWIDTH(scale->tsw);
            msbm.Rows        = scale->tsh;
            msbm.Depth       = 1;
            msbm.Planes[0]   = src;

            memset(&mdbm,0,sizeof(mdbm));
            mdbm.BytesPerRow = RAWIDTH(scale->dw);
            mdbm.Rows        = scale->dh;
            mdbm.Depth       = 1;
            mdbm.Planes[0]   = dest;

            sbm = &msbm;
            dbm = &mdbm;
        }
    }
    else
    {
        if((dest = AllocBitMap(scale->dw,scale->dh,GetBitMapAttr(src,BMA_DEPTH),isFlagSet(flags, SCALEFLG_CyberMap) ? BMF_MINPLANES : 0,isFlagSet(flags, SCALEFLG_CyberMap) ? src : NULL)))
        {
            sbm = src;
            dbm = dest;
        }
    }

    if (dest)
    {
        struct BitScaleArgs bsa;

        memset(&bsa,0,sizeof(bsa));

        bsa.bsa_SrcX        = scale->sx;
        bsa.bsa_SrcY        = scale->sy;

        bsa.bsa_SrcWidth    = scale->sw;
        bsa.bsa_SrcHeight   = scale->sh;

        bsa.bsa_XSrcFactor  = bsa.bsa_YSrcFactor  = 100;
        bsa.bsa_XDestFactor = bsa.bsa_YDestFactor = scale->ratio;

        bsa.bsa_SrcBitMap   = sbm;
        bsa.bsa_DestBitMap  = dbm;

        D(DBF_STARTUP, "Scaling 4 %ld %ld %ld %ld %ld %ld",
            bsa.bsa_SrcX,
            bsa.bsa_SrcY,
            bsa.bsa_SrcWidth,
            bsa.bsa_SrcHeight,
            bsa.bsa_XSrcFactor,
            bsa.bsa_XDestFactor);

        BitMapScale(&bsa);
    }

    RETURN(dest);
    return dest;
}

/***********************************************************************/

void
scaleStripBitMaps(struct InstData *data)
{
    ENTER();

    if (data->strip->normalBM)
    {
        struct scaleBitMap scale;
        ULONG     flags;

        D(DBF_STARTUP, "Scaling normal bitmap");

        scale.sx  = data->image->left;
        scale.sy  = data->image->top;
        scale.sw  = data->image->width;
        scale.sh  = data->image->height;
        scale.dw  = ScalerDiv(scale.sw,scale.ratio = data->scale,100);
        scale.dh  = ScalerDiv(scale.sh,scale.ratio,100);
        scale.tsw = data->image->dataWidth;
        scale.tsh = data->image->dataHeight;

        flags = isFlagSet(data->flags, FLG_CyberMap) ? SCALEFLG_CyberMap : 0;
        if (isFlagSet(data->flags, FLG_CyberDeep))
            setFlag(flags, SCALEFLG_CyberDeep);

        if((data->normalBM = scaleBitMap(data->strip->normalBM,&scale,flags)))
        {
            if (data->strip->greyBM)
                data->greyBM = scaleBitMap(data->strip->greyBM,&scale,flags);
            if (data->strip->mask)
                data->mask = scaleBitMap(data->strip->mask,&scale,flags | SCALEFLG_Mask);

            if (data->strip->snormalBM && (data->snormalBM = scaleBitMap(data->strip->snormalBM,&scale,flags)))
            {
                if (data->strip->sgreyBM)
                    data->sgreyBM = scaleBitMap(data->strip->sgreyBM,&scale,flags);
                if (data->strip->smask)
                    data->smask = scaleBitMap(data->strip->smask,&scale,flags | SCALEFLG_Mask);
            }

            if (data->strip->dnormalBM && (data->dnormalBM = scaleBitMap(data->strip->dnormalBM,&scale,flags)))
            {
                if (data->strip->dgreyBM)
                    data->dgreyBM = scaleBitMap(data->strip->dgreyBM,&scale,flags);
                if (data->strip->dmask)
                    data->dmask = scaleBitMap(data->strip->dmask,&scale,flags | SCALEFLG_Mask);
            }

            D(DBF_STARTUP, "Normal bitmap scaled %lx %lx", data->normalBM,data->mask);
        }

        data->imgWidth  = scale.dw;
        data->imgHeight = scale.dh;
    }

    LEAVE();
}

/***********************************************************************/

void
clearBitMaps(struct InstData *data)
{
    data->normalBM = data->greyBM    = data->snormalBM =
    data->sgreyBM  = data->dnormalBM = data->dgreyBM   = NULL;

    data->nchunky  = data->gchunky  = data->snchunky = data->sgchunky =
    data->dnchunky = data->dgchunky = NULL;

    data->mask = data->smask = data->dmask = NULL;
}

/***********************************************************************/

void
freeBitMaps(struct InstData *data)
{
  ENTER();

  #if defined(WITH_ALPHA)
  if(data->image != NULL &&
     isFlagSet(data->image->flags, BRFLG_AlphaMask))
  #else
  if(data->allowAlphaChannel &&
     data->image != NULL &&
     isFlagSet(data->image->flags, BRFLG_AlphaMask))
  #endif
  {
    if(data->nchunky)
      SharedFree(data->nchunky);

    if(data->snchunky)
      SharedFree(data->snchunky);

    if(data->dnchunky)
      SharedFree(data->dnchunky);
  }

  if(data->normalBM)
  {
    if(isFlagClear(data->flags, FLG_CyberDeep) && isFlagClear(data->flags, FLG_Strip))
    {
      struct ColorMap *cm = data->screen->ViewPort.ColorMap;
      struct pen *pens, *gpens, *spens, *sgpens, *dpens, *dgpens;
      int i;

      pens  = data->pens;
      gpens = (data->greyBM) ? data->gpens : NULL;

      if((spens = ((data->snormalBM) ? data->spens : NULL)))
        sgpens = (data->sgreyBM) ? data->sgpens : NULL;
      else
        sgpens = NULL;

      if((dpens = ((data->dnormalBM) ? data->dpens : NULL)))
        dgpens = (data->dgreyBM) ? data->dgpens : NULL;
      else
        dgpens = NULL;

      for(i=256; i--;)
      {
        if(pens)
        {
          if(pens->done)
          {
            ReleasePen(cm,pens->pen);
            pens->done = 0;
          }

          pens++;
        }

        if(gpens)
        {
          if(gpens->done)
          {
            ReleasePen(cm,gpens->pen);
            gpens->done = 0;
          }

          gpens++;
        }

        if(spens)
        {
          if(sgpens)
          {
            if(sgpens->done)
            {
              ReleasePen(cm,sgpens->pen);
              sgpens->done = 0;
            }

            sgpens++;
          }

          if(spens->done)
          {
            ReleasePen(cm,spens->pen);
            spens->done = 0;
          }

          spens++;
        }

        if(dpens)
        {
          if(dgpens)
          {
            if(dgpens->done)
            {
              ReleasePen(cm,dgpens->pen);
              dgpens->done = 0;
            }

            dgpens++;
          }

          if(dpens->done)
          {
            ReleasePen(cm,dpens->pen);
            dpens->done = 0;
          }

          dpens++;
        }
      }
    }

    FreeBitMap(data->normalBM);

    if(data->greyBM)
      FreeBitMap(data->greyBM);

    if(data->mask)
    {
      if(isFlagSet(data->flags, FLG_CyberMap))
        FREERASTERCG(data->mask);
      else
        FREERASTER(data->mask);
    }

    if(data->snormalBM)
    {
      if(data->sgreyBM)
        FreeBitMap(data->sgreyBM);

      if(data->smask)
      {
        if(isFlagSet(data->flags, FLG_CyberMap))
          FREERASTERCG(data->smask);
        else
          FREERASTER(data->smask);
      }

      FreeBitMap(data->snormalBM);
    }

    if(data->dnormalBM)
    {
      if(data->dgreyBM)
        FreeBitMap(data->dgreyBM);

      if(data->dmask)
      {
        if(isFlagSet(data->flags, FLG_CyberMap))
          FREERASTERCG(data->dmask);
        else
          FREERASTER(data->dmask);
      }

      FreeBitMap(data->dnormalBM);
    }
  }

  clearBitMaps(data);
  LEAVE();
}

/***********************************************************************/

void
build(struct InstData *data)
{
    ENTER();

    if (data->image)
    {
        if (isFlagSet(data->flags, FLG_Strip))
        {
            #if defined(WITH_ALPHA)
            if (isFlagSet(data->flags, FLG_CyberDeep))
            #else
            if (data->allowAlphaChannel && isFlagSet(data->flags, FLG_CyberDeep))
            #endif
            {
                buildBitMapsCyber(data);
                LEAVE();
                return;
            }

            if (isFlagSet(data->flags, FLG_Scaled))
            {
                scaleStripBitMaps(data);
            }
            else
            {
                data->imgWidth  = data->image->width;
                data->imgHeight = data->image->height;

                if (data->strip->mask)
                {
                    if((data->mask = (isFlagSet(data->flags, FLG_CyberMap) ? ALLOCRASTERCG(data->imgWidth,data->imgHeight) : ALLOCRASTER(data->imgWidth,data->imgHeight))))
                      memcpy(data->mask,data->strip->mask,RAWIDTH(data->imgWidth)*data->imgHeight);
                }
            }
        }
        else
        {
            if (isFlagSet(data->flags, FLG_CyberDeep))
                buildBitMapsCyber(data);
            else
                buildBitMaps(data);
        }
    }

    LEAVE();
}

/***********************************************************************/

