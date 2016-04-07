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

#define DUMP     0
#define RUN      1
#define MINRUN   3
#define MAXRUN 128
#define MAXDAT 128

#define BRCUnpackOK    0
#define BRCUnpackError 1

/***********************************************************************/

int
BRCUnpack(APTR pSource, APTR pDest, LONG srcBytes0, LONG dstBytes0)
{
  signed char *source = pSource;
  signed char *dest = pDest;
  LONG srcBytes = srcBytes0;
  LONG dstBytes = dstBytes0;
  int result = BRCUnpackOK;

  ENTER();

  while(dstBytes > 0)
  {
    WORD n;

    if(--srcBytes < 0)
    {
      result = BRCUnpackError;
      break;
    }

    n = *source++;

    if(n >= 0)
    {
      n++;

      srcBytes -= n;
      dstBytes -= n;
      if(srcBytes < 0 || dstBytes < 0)
      {
        result = BRCUnpackError;
        break;
      }

      do
      {
        *dest++ = *source++;
      }
      while(--n > 0);
    }
    else if(n != -128)
    {
      signed char c;

      n = -n + 1;

      srcBytes--;
      dstBytes -= n;
      if(srcBytes < 0 || dstBytes < 0)
      {
        result = BRCUnpackError;
        break;
      }

      c = *source++;

      do
      {
        *dest++ = c;
      }
      while(--n > 0);
    }
  }

  RETURN(result);
  return result;
}

/***********************************************************************/
