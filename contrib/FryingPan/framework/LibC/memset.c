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

void* memset(void* dest, int c, size_t count)
{
   char          *ptr = (char*)dest;  
   unsigned long *pl;
   unsigned long  pat;

   c&=255;
   pat = (c<<24) | (c<<16) | (c<<8) | c;

   while (((IPTR)ptr)&3 && count)
   {
      *ptr++ = c;
      --count;
   }

   pl = (unsigned long*)ptr;
   while (count > sizeof(unsigned long))
   {
      *pl++ = pat;
      count -= sizeof(unsigned long);
   }

   ptr = (char*)pl;
   while (count--)
      *ptr++ = c;

    return dest;
} 

