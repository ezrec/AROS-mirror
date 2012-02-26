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

#ifndef _GENERIC_SUMCRC_H_
#define _GENERIC_SUMCRC_H_

#include "Types.h"

namespace GenNS
{
   class SumCRC 
   {
   public:
      enum Polynominals
      {
         CRC_16_CCITT,        // x^16 + x^12 + x^5 + 1
         CRC_16_IBM,          // x^16 + x^15 + x^2 + 1 
         CRC_32_MPEG2,        // x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
         CRC_32_IEEE,         // as above
         CRC_32_Castagnoli,   // x^32 + x^28 + x^27 + x^26 + x^25 + x^23 + x^22 + x^20 + x^19 + x^18 + x^14 + x^13 + x^11 + x^10 + x^9 + x^8 + x^6 + 1
         CRC_64_ISO,          // x^64 + x^4 + x^3
         CRC_64_ECMA,         // no way.
      };

   protected:
      uint64         crc;
      uint64         lastWord;

      int8           polySize;
      uint64         polyWord;
   protected:
      void           setPoly(uint64 poly, int8 size);

   public:
      SumCRC(Polynominals poly);
      SumCRC(uint64 poly, int8 size);
      virtual       ~SumCRC();

      virtual void   Initialize();
      virtual void   Update(const uint8 *, uint32);
      virtual void   Finalize();

      virtual uint64 GetSum();
   };
};

#endif

