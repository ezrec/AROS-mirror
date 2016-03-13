/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "LibC.h"
#include <Generic/Types.h>

struct quad
{
   union
   {
      uint32 l[2];
      uint64 q;
   } data;
};

uint64 __ashldi3(uint64 a, int32 sh)
{
   struct quad m;
   uint32 x, y;

#if (1) // just to quieten gcc ...
   m.data.l[0] = 0;
   m.data.l[1] = 0;
#endif
   m.data.q = a;
   x = m.data.l[0];
   y = m.data.l[1];

   if (sh >= 64)
   {
      return 0;
   }

   if (sh == 0)
   {
      return a;
   }

   if (sh >= 32)
   {
      x = y;
      y = 0;
      sh -= 32;
   }

   x <<= sh;
   x  |= (y >> (32 - sh));
   y <<= sh;

   m.data.l[0] = x;
   m.data.l[1] = y;

   return (uint64)m.data.q;
}
