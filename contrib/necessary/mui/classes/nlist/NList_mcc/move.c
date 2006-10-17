/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#define NO_PROTOS
#include "private.h"

/*
 *
 * Warning: the number of octets have to be a multiple of 4
 * and to be efficient, src and dest also have to be a multiple of 4
 *
 * extern void  NL_Move(void *dest,void *src,LONG len);
 *
 */

void  NL_Move( long *dest, long *src, long len, long newpos )
{
	if ( len > 0 )
	{
		len /= 4;
		len--;

		do
		{
			(*(struct TypeEntry **)src)->entpos = newpos++;
			*(dest++) = *(src++);
		}
		while ( --len >= 0 );
	}
}


void  NL_MoveD( long *dest, long *src, long len, long newpos )
{
	if ( len > 0 )
	{
		len /= 4;
		len--;

		do
		{
			*(--dest) = *(--src);
			(*(struct TypeEntry **)dest)->entpos = --newpos;
		}
		while ( --len >= 0 );
	}
}



/*
void  NL_Move(long *dest,long *src,long len)
{
  if (len > 0)
  {
    len /= 4;
    len--;
    do {
      *(dest++) = *(src++);
    } while (--len >= 0);
  }
}


void  NL_MoveD(long *dest,long *src,long len)
{
  if (len > 0)
  {
    len /= 4;
    len--;
    do {
      *(--dest) = *(--src);
    } while (--len >= 0);
  }
}
*/
