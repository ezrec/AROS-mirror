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

#ifndef _GENERIC_SUMMD5_H_
#define _GENERIC_SUMMD5_H_

#include "Types.h"

namespace GenNS
{
   class SumMD5 
   {
      uint32         state[4];				/* state (ABCD) */
      uint32         count[2];				/* number of bits, modulo 2^64 (lsb first) */
      uint8          buffer[64];	         /* input buffer */
      bool           done;

   protected:
      void           MD5Transform(const uint8[64]);
      void           Encode(uint8*, const uint32*, uint32);
      void           Decode(uint32*, const uint8*, uint32);

   public:
      SumMD5();
      virtual       ~SumMD5();

      virtual void   Initialize();
      virtual void   Update(const uint8 *, uint32);
      virtual void   Finalize();

      virtual void   GetSum(uint8 [16]);
      virtual void   GetSum(uint32 [4]);
      virtual void   GetSum(uint64 [2]);
   };
};

#endif

