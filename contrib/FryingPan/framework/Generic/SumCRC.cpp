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

#include "SumCRC.h"

using namespace GenNS;

SumCRC::SumCRC(Polynominals poly)
{
   switch (poly)
   {
      case CRC_16_CCITT:
         setPoly(0x1021, 16);
         break;

      case CRC_16_IBM:
         setPoly(0x8005, 16);
         break;

      case CRC_32_MPEG2:
      case CRC_32_IEEE:
         setPoly(0x4c11db7, 32);
         break;

      case CRC_32_Castagnoli:
         setPoly(0x1edc6f41, 32);
         break;

      case CRC_64_ISO:
         setPoly(0x1b, 64);
         break;

      case CRC_64_ECMA:
         setPoly(0x42f0e1eba9ea3693ull, 64);
         break;
   };

   Initialize();
}
   
void SumCRC::setPoly(uint64 poly, int8 size)
{
   polySize = size;
   polyWord = poly << (64 - size);
}

SumCRC::SumCRC(uint64 poly, int8 size)
{
   setPoly(poly, size);
   Initialize();
}

SumCRC::~SumCRC()
{
}
   
void SumCRC::Initialize()
{
   lastWord = 0;
   crc      = 0;
}

void SumCRC::Update(const uint8 * mem, uint32 size)
{
   bool bit = false;

   while (size--)
   {
      for (int i=0; i<8; i++)
      {
         bit = (lastWord & 0x8000000000000000ull) ? true : false;
         lastWord <<= 1;
         if (bit)
            lastWord ^= polyWord;
      }
      lastWord ^= *mem++;
   }  
}

void SumCRC::Finalize()
{
   bool   bit = false;

   for (int i=63; i>=0; i--)
   {
      bit = (lastWord & 0x8000000000000000ull) ? true : false;
      lastWord <<= 1;
      if (bit)
         lastWord ^= polyWord;
   }
   crc = (lastWord >> (64-polySize)) & ((1ull << polySize) - 1);
}

uint64 SumCRC::GetSum()
{
   return crc;
}

