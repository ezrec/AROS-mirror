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

#ifndef _GEN_STRING_H_
#define _GEN_STRING_H_

#include <dos/dos.h>
#include "Types.h"
#include "VectorT.h"

#ifdef String
#undef String
#endif

namespace GenNS
{
   class String 
   {
      unsigned long  lMaxLen;
      unsigned long  lLength;
      unsigned char *sContents;

   protected:
      void           Clone(const char* sStr, unsigned int count);
   public:
      String(void);
      String(const char* s);
      String(const String &s);
      ~String();
      const char    *Data(void) const;
      void           Assign(const char *s);
      void           Assign(const String *s);
      int            Length(void) const;
      void           SetLength(unsigned long lLength);                  // sets length of element + adds pad zero
      void           Update();                                          // recalc length
      int            FormatStr(const char *sFmtStr, IPTR pParams);
      operator char*() const;
      operator unsigned char*() const;
      String        &operator =  (const String & sStr);
      String        &operator =  (const int64 sVal);
      String         operator +  (const char *sStr);
      String         operator +  (const String & sStr);
      String        &operator += (const char *sStr);
      String        &operator += (const char cChar);
      int            operator < (const String sStr) const;
      int            operator < (const char* sStr) const;
      int            operator == (const String sStr) const;
      int            operator == (const char *sStr) const;
      bool           Equals(const char* sOther);
      bool           EqualsIgnoreCase(const char* sOther);
      int            Compare(const char* sOther) const;
      int            CompareIgnoreCase(const char* sOther) const;
      char          &operator [] (int lOffset);
      void           AddPath(const char* sElement);

      void           AllocBuf(unsigned int lSize);
      void           ReallocBuf(unsigned int lSize);

      unsigned long  TrimChars(const char* sChars);
      int32          ToLong();
      int64          ToQuad();
      String         SubString(int lFirst, int lLen);
      String        &Substitute(const char* src, const char* dst);
      String         LeftString(int lLen);
      String         RightString(int lLen);

      String        &UpperCase();
      String        &LowerCase();

      void           StrLCpy(const char* src, int len);
      void           BstrCpy(BSTR src);

      String        &ToUTF8();
      String        &FromUTF8();

      VectorT<String> Explode(); // splits string into words "and phrases"

   };

   String      operator + (const char *sStr1, const String sStr2);
}

#endif //_STRING_H_
