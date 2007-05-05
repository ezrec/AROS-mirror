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

#include "Debug.h"

#include <math.h>

/*
#if !defined(_FFP)
void __stdargs _CXFERR(int code) {}
#endif
double __except(int a,const char *b,double c,double e,double f) { return f; }
*/

/***********************************************************************/

struct ScaleData
{
    APTR  data;
    LONG  cy;
    FLOAT sourcey;
    FLOAT deltax;
    FLOAT deltay;
};

/***********************************************************************/

static void
scaleLine(struct scale *sce,struct ScaleData *data,UBYTE *src,UBYTE *dst)
{
    LONG w8 = (sce->dw>>3)+1, cx = 0, dx = data->deltax*65536;

    ENTER();

    src += data->cy*sce->sw;

    switch (sce->dw & 7)
    {
        do
        {
            *dst++ = src[cx>>16];         cx += dx;
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

    LEAVE();
}

/***********************************************************************/

void
scale(struct scale *sce,UBYTE *src,UBYTE *dst)
{
    ENTER();

    if (sce && src && dst)
    {
        struct ScaleData scdata;
        LONG             y;

        scdata.cy      = 0;
        scdata.sourcey = 0;

        scdata.deltax  = sce->sw - 1;
        scdata.deltax  /= (sce->dw - 1);

        scdata.deltay  = sce->sh - 1;
        scdata.deltay  /= (sce->dh - 1);

        for (y = 0; y<sce->dh; ++y)
        {
            scaleLine(sce,&scdata,src,dst);
            dst += sce->dw;
        }
    }

    LEAVE();
}

/***********************************************************************/
