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

#include <exec/types.h>

#include "Debug.h"

/***********************************************************************/

#define DUMP     0
#define RUN      1
#define MINRUN   3
#define MAXRUN 128
#define MAXDAT 128

/***********************************************************************/

UWORD
BRCUnpack(signed char *pSource,signed char *pDest,LONG srcBytes0,LONG dstBytes0)
{
    signed char *source = pSource, *dest = pDest, c;
    WORD n;
    LONG  srcBytes = srcBytes0, dstBytes = dstBytes0;
    UWORD          error = TRUE;
    WORD           minus128 = -128;

    ENTER();

    while(dstBytes>0)
    {
        if ((srcBytes-=1)<0) goto errorExit;
        n = *source++;

        if (n>=0)
        {
            n += 1;

            if ((srcBytes -= n) <0) goto errorExit;
            if ((dstBytes -= n) <0) goto errorExit;

            do
            {
                *dest++ = *source++;
            } while(--n>0);
        }
        else
            if (n!=minus128)
            {
                n = -n+1;
                if ((srcBytes -= 1)<0) goto errorExit;
                if ((dstBytes -= n)<0) goto errorExit;
                c = *source++;

                do
                {
                    *dest++ = c;
                } while(--n>0);
            }
    }

    error = FALSE;

    errorExit:

    RETURN(error);
    return error;
}

/***********************************************************************/
