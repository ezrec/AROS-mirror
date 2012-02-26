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

uint64 udiv64(uint64 a, uint64 b)
{
   uint64 tmp;
   uint64 res = 0;
   uint64 val = 0;

   while (a >= b)
   {
      tmp = b;
      val = 1;
      while ((a >= (tmp<<1)) && (tmp < 0x8000000000000000ull))
      {
         val <<= 1;         
         tmp <<= 1;
      }
      res |= val;
      a   -= tmp;
   }

   return res;
}
