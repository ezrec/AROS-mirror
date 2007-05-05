/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

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

#include <math.h>

/*
#if !defined(_FFP)
#ifndef __MORPHOS__
void STDARGS _CXFERR(int code) {}
#endif
double __except(int a,const char *b,double c,double e,double f) { return f; }
#endif
*/

/***********************************************************************/

struct scaleData
{
    LONG  cy;
    FLOAT sourcey;
    FLOAT deltax;
    FLOAT deltay;
};

/***********************************************************************/

static void
scaleLine(struct scale *sce,struct scaleData *data,UBYTE *src,UBYTE *dst)
{
    LONG w8 = (sce->dw>>3)+1, cx = 0, dx = data->deltax*65536;

    src += sce->sl+(data->cy+sce->st)*sce->tw;

    switch (sce->dw & 7)
    {
        do
        {
                    *dst++ = src[cx>>16]; cx += dx;
            case 7: *dst++ = src[cx>>16]; cx += dx;
            case 6: *dst++ = src[cx>>16]; cx += dx;
            case 5: *dst++ = src[cx>>16]; cx += dx;
            case 4: *dst++ = src[cx>>16]; cx += dx;
            case 3: *dst++ = src[cx>>16]; cx += dx;
            case 2: *dst++ = src[cx>>16]; cx += dx;
            case 1: *dst++ = src[cx>>16]; cx += dx;
            case 0: w8--;

        } while (w8);
    }

    data->cy = data->sourcey += data->deltay;
}

/***********************************************************************/

void
scale(struct scale *sce,UBYTE *src,UBYTE *dst)
{
    if (sce && src && dst && sce->dw-1>0 && sce->dh-1>0)
    {
        struct scaleData scdata;
        LONG             y;

        scdata.cy       = 0;
        scdata.sourcey  = 0;

        scdata.deltax   = sce->sw-1;
        scdata.deltax  /= (sce->dw-1);

        scdata.deltay   = sce->sh-1;
        scdata.deltay  /= (sce->dh-1);

        for (y = 0; y<sce->dh; y++)
        {
            scaleLine(sce,&scdata,src,dst);
            dst += sce->dw;
        }
    }
}

/***********************************************************************/

static void
scaleLineRGB(struct scale *sce,struct scaleData *data,ULONG *src,ULONG *dst)
{
    LONG w8 = (sce->dw>>3)+1, cx = 0, dx = data->deltax*65536;

    src = (ULONG *)((UBYTE *)src+4*sce->sl+(data->cy+sce->st)*sce->tw);

    //kprintf("src:%lx dst:%lx\n",src,dst);

    switch (sce->dw & 7)
    {
        do
        {
                    *dst++ = src[cx>>16]; cx += dx;
            case 7: *dst++ = src[cx>>16]; cx += dx;
            case 6: *dst++ = src[cx>>16]; cx += dx;
            case 5: *dst++ = src[cx>>16]; cx += dx;
            case 4: *dst++ = src[cx>>16]; cx += dx;
            case 3: *dst++ = src[cx>>16]; cx += dx;
            case 2: *dst++ = src[cx>>16]; cx += dx;
            case 1: *dst++ = src[cx>>16]; cx += dx;
            case 0: w8--;

        } while (w8);
    }

    data->cy = data->sourcey += data->deltay;
}

/***********************************************************************/

void
scaleRGB(struct scale *sce,ULONG *src,ULONG *dst)
{
    if (sce && src && dst && sce->dw-1>0 && sce->dh-1>0)
    {
        struct scaleData scdata;
        LONG             y;

        scdata.cy       = 0;
        scdata.sourcey  = 0;

        scdata.deltax   = sce->sw-1;
        scdata.deltax  /= (sce->dw-1);

        scdata.deltay   = sce->sh-1;
        scdata.deltay  /= (sce->dh-1);

        for (y = 0; y<sce->dh; y++)
        {
            scaleLineRGB(sce,&scdata,src,dst);
            dst += sce->dw;
        }
    }
}

/***********************************************************************/

