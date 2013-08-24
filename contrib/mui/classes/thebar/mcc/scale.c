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

/***********************************************************************/

struct scaleData
{
  LONG cy;      // LONG
  LONG sourcey; // fixed point value
  LONG deltax;  // fixed point value
  LONG deltay;  // fixed point value
};

/***********************************************************************/

/// fixed_div
// calculate a/b and return the quotient as a fixed point value
static LONG fixed_div(LONG a, LONG b)
{
  // Intermediate results may become larger than 32bit,
  // hence we need 64bit values temporarily. The result
  // is a 16.16 fixed point value fitting into a 32bit
  // variable.
  // The two intermediate values must be declared as volatile,
  // else GCC's optimizer will always "optimize" the result to
  // zero.
  volatile long long _a;
  volatile long long _b;

  // prescale a by the double amount of fraction bits
  // the additional 16 bits will vanish again upon division
  _a = a;
  _a <<= 32;
  // prescale b by the normal amount of fraction bits
  _b = b;
  _b <<= 16;

  return (LONG)(_a / _b);
}

/***********************************************************************/

static void scaleLine(struct scale *sce, struct scaleData *data, UBYTE *src, UBYTE *dst)
{
  LONG w8 = (sce->dw>>3)+1;
  LONG cx = 0;
  LONG dx = data->deltax;

  ENTER();

  src += data->cy*sce->sw;

  switch(sce->dw & 7)
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

    } while(w8);
  }

  data->sourcey += data->deltay;
  // convert back to normal integers
  data->cy = data->sourcey >> 16;

  LEAVE();
}

/***********************************************************************/

void scale(struct scale *sce, UBYTE *src, UBYTE *dst)
{
  ENTER();

  if(sce && src && dst)
  {
    struct scaleData scdata;
    LONG y;

    D(DBF_SCALE, "sw=%d sh=%d dw=%d dh=%d", sce->sw, sce->sh, sce->dw, sce->dh);

    scdata.cy = 0;
    scdata.sourcey = 0;
    // calculate the scale factor
    scdata.deltax = fixed_div(sce->sw-1, sce->dw-1);
    scdata.deltay = fixed_div(sce->sh-1, sce->dh-1);

    for(y = 0; y<sce->dh; ++y)
    {
      scaleLine(sce, &scdata, src, dst);
      dst += sce->dw;
    }
  }

  LEAVE();
}

/***********************************************************************/
