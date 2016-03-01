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
#include <proto/exec.h>

void *realloc(void* mem, size_t len)
{
   void          *newmem = malloc(len);
   IPTR *oldmem = (IPTR *)mem;
   // verify in case new block is smaller than the last one
   // 8 extra bytes are included for memory block header (pool & len)

   if (0 == len)
      return 0;

   if (NULL == mem)
      return newmem;

#ifndef DEBUG
   if (len > oldmem[-1]-8)
      len = oldmem[-1]-8;
#else
   if (len > oldmem[-(1+(128>>3))]-8)
      len = oldmem[-(1+(128>>3))]-8;
#endif

#ifndef __amigaos4__
   CopyMem(oldmem, newmem, len);
#else
   IExec->CopyMem(oldmem, newmem, len);
#endif
   free(oldmem);
   return newmem;
}

