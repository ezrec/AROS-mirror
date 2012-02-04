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

#ifndef LOCALE_IFACE_H_
#define LOCALE_IFACE_H_

#include <libclass/exec.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>

struct Catalog;
struct Locale;
struct TagItem;
struct DateStamp;
struct Hook;

   BEGINDECL(Locale, "locale.library")
      PROC1(            CloseCatalog,        6, Catalog*,      catalog,    a0);
      PROC1(            CloseLocale,         7, Locale*,       locale,     a0);
      FUNC2(uint,       ConvToLower,         8, Locale*,       locale,     a0, uint,         chr,     d0); 
      FUNC2(uint,       ConvToUpper,         9, Locale*,       locale,     a0, uint,         chr,     d1);
      PROC4(            FormatDate,         10, Locale*,       locale,     a0, const char*,  tmplate, a1, struct DateStamp*,  date,    a2, const Hook*,     putChHook,  a3);
      PROC4(            FormatString,       11, Locale*,       locale,     a0, const char*,  tmplate, a1, void*,              data,    a2, const Hook*,     putChHook,  a3);
      FUNC3(const char*,GetCatalogStr,      12, Catalog*,      catalog,    a0, uint,         num,     d0, const char*,        defStr,  a1);
      FUNC2(const char*,GetLocaleStr,       13, Locale*,       locale,     a0, uint,         num,     d0);

      FUNC2(sint,       IsAlNum,            14, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsAlpha,            15, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsCntrl,            16, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsDigit,            17, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsGraph,            18, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsLower,            19, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsPrint,            20, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsPunct,            21, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsSpace,            22, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsUpper,            23, Locale*,       locale,     a0, uint,         chr,     d0);
      FUNC2(sint,       IsXDigit,           24, Locale*,       locale,     a0, uint,         chr,     d0);

      FUNC3(Catalog*,   OpenCatalogA,       25, Locale*,       locale,     a0, const char*,  name,    a1, const TagItem*,     tags,    a2);
      PROC0(            os4reserved1,       25);
      FUNC1(Locale*,    OpenLocale,         26, const char*,   name,       a0);
      FUNC4(sint,       ParseDate,          27, Locale*,       locale,     a0, struct DateStamp*, ds, a1, const char*,        format,  a2, const Hook*,     getChHook,  a3);
      PROC0(            reserved1,          28);
      FUNC5(uint,       StrConvert,         29, Locale*, locale, a0, const char*, string, a1, void*, buffer, a2, uint, buffersize, d0, uint, type, d1);
      FUNC5(sint,       StrnCmp,            30, Locale*, locale, a0, const char*, str1, a1, const char*, str2, a2, sint, length, d0, uint, type, d1);

   ENDDECL
#endif

