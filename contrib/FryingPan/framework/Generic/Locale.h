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

#ifndef _LOCALE_H_
#define _LOCALE_H_

#include "Types.h"
#include "HashT.h"
#include "String.h"

class LocaleIFace;
struct Locale;

namespace GenNS
{
   class Localization
   {
   protected:
      struct locale_set
      {
         String      str;
         const char* group;
         const char* id;
         char        accel[2];

         locale_set() :
            str(0), group(0), id(0)
         {
            accel[0] = accel[1] = 0;
         }

         locale_set(const char* a, const char* b, const char* c, char d=0) :
            str(a), group(b), id(c)
         {
            accel[0] = d;
            accel[1] = 0;
         }
      };

   protected:
      class ::LocaleIFace       *locale;
      struct ::Locale           *lang;
      locale_set                 deflt;
      HashT<IPTR, locale_set>  hash;

   public:
      struct LocaleSet 
      {
         IPTR      key;
         const char* value;
         const char* locale_id;

         enum 
         {
            Set_Last = 0xffffffff
         };
      };

      void              split(String s, char &accelerator, String &text); 
   public:
                        Localization(LocaleSet set[] = 0, const char* group=0);
      virtual          ~Localization();
      void              Add(LocaleSet set[], const char* group=0);
      const String&     operator[] (IPTR key);
      const String&     Str(IPTR key);
      const char        Accel(IPTR key);        // give back single char
      const char*       Shortcut(IPTR key);     // give back complete string
      bool              ReadCatalog(const char* name, sint version=0);

      bool              ExportCD(const char* filename, sint version=0);
      bool              ExportCT(const char* filename, sint version=0);
      String            FormatNumber(sint integer, sint millionth=0);
   };

};

#endif
