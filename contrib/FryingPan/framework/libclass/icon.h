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

#ifndef _LIBCLASS_ICON_H_
#define _LIBCLASS_ICON_H_

extern class IconIFace *Icon;

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#include <Generic/Types.h>

struct DiskObject;
struct TagList;
struct FreeList;
struct RastPort;
struct Rectangle;
struct ColorRegister;
struct Screen;

   BEGINDECL(Icon, "icon.library")
      PROC0(                  obsolete0,              5);
      PROC0(                  obsolete1,              6);
      FUNC3(uint32,           GetIcon,                7, const char*,         name, a0, const DiskObject*,  obj,  a1, FreeList*,       list, a2);
      FUNC2(bool,             PutIcon,                8, const char*,         name, a0, const DiskObject*,  obj,  a1);
      PROC1(                  FreeFreeList,           9, FreeList*,           list, a0);
      PROC0(                  obsolete2,             10);
      PROC0(                  obsolete3,             11);
      FUNC3(bool,             AddFreeList,           12, FreeList*,           list, a0, void*,              mem,  a1, uint32,          size, d0);
      FUNC1(DiskObject*,      GetDiskObject,         13, const char*,         name, a0);
      FUNC2(bool,             PutDiskObject,         14, const char*,         name, a0, const DiskObject*,  obj,  a1);
      PROC1(                  FreeDiskObject,        15, DiskObject*,         obj,  a0);
      FUNC2(char*,            FindToolType,          16, const char**,        types,a0, const char*,        name, a1);
      FUNC2(bool,             MatchToolValue,        17, const char*,         type, a0, const char*,        val,  a1);
      FUNC2(char*,            BumpRevision,          18, const char*,         newn, a0, const char*,        oldn, a1);
      FUNC3(void*,            FreeAlloc,             19, FreeList*,           list, a0, uint32,             len,  d0, uint32,          type, d1);
      FUNC1(DiskObject*,      GetDefDiskObject,      20, uint32,              type, d0);
      FUNC1(bool,             PutDefDiskObject,      21, const DiskObject*,   obj,  a0);
      FUNC1(DiskObject*,      GetDiskObjectNew,      22, const char*,         name, a0);
      FUNC1(bool,             DeleteDiskObject,      23, const char*,         name, a0);
      PROC2(                  FreeFree,              24, FreeList*,           list, a0, void*,              mem,  a1);
   
      FUNC2(DiskObject*,      DupDiskObjectA,        25, const DiskObject*,   obj,  a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(                  reserved0,             25);
#endif
      FUNC2(uint32,           IconControlA,          26, DiskObject*,         obj,  a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(                  reserved1,             26);
#endif
      PROC7(                  DrawIconStateA,        27, RastPort*,           rp,   a0, const DiskObject*,  obj,  a1, const char*,     lbl,  a2, int32,        left, d0, int32,           top, d1, int32,   state, d2, const TagItem*, tags, a3);
#ifdef __AMIGAOS4__
      PROC0(                  reserved2,             27);
#endif
      FUNC5(bool,             GetIconRectangleA,     28, RastPort*,           rp,   a0, const DiskObject*,  obj,  a1, const char*,     lbl,  a2, Rectangle*,   rect, a3, const TagItem*,  tags,a4);
#ifdef __AMIGAOS4__
      PROC0(                  reserved3,             28);
#endif
      FUNC1(DiskObject*,      NewDiskObject,         29, int32,               type, d0);
      FUNC2(DiskObject*,      GetIconTagList,        30, const char*,         name, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(                  reserved4,             30);
#endif
      FUNC3(bool,             PutIconTagList,        31, const char*,         name, a0, const DiskObject*,  obj,  a1, const TagItem*,  tags, a2);
#ifdef __AMIGAOS4__
      PROC0(                  reserved5,             31);
#endif
      FUNC3(bool,             LayoutIconA,           32, DiskObject*,         icon, a0, Screen*,            scrn, a1, TagItem*,        tags, a2);
#ifdef __AMIGAOS4__
      PROC0(                  reserved6,             32);
#endif
      PROC1(                  ChangeToSelectedIconColor, 33, ColorRegister*, cr, a0);

   ENDDECL

#undef MOS_DIRECT_OS

#endif 
