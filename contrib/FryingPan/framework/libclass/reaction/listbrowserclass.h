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

#ifndef __OOP_LISTBROWSERCLASS_H_
#define __OOP_LISTBROWSERCLASS_H_

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>

struct Node;
struct List;
struct TagItem;
struct ColumnInfo;

   BEGINDECL(ListBrowser, "gadgets/listbrowser.gadget")
      FUNC0(Class*,     GetListBrowserClass,          5);
      FUNC2(Node*,      AllocListBrowserNodeA,        6, uint16,           cols, d0, const TagItem*,     tags, a0);
#ifdef __AMIGAOS4__
      PROC0(            dummy0,                       6);
#endif
      PROC1(            FreeListBrowserNode,          7, Node*,            node, a0);
      PROC2(            SetListBrowserNodeAttrsA,     8, Node*,            node, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy1,                       8);
#endif
      PROC2(            GetListBrowserNodeAttrsA,     9, Node*,            node, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy2,                       9);
#endif
      PROC1(            ListBrowserSelectAll,        10, List*,            list, a0);
      PROC2(            ShowListBrowserNodeChildren, 11, Node*,            node, a0, uint16,             dpth, d0);
      PROC1(            HideListBrowserNodeChildren, 12, Node*,            node, a0);
      PROC1(            ShowAllListBrowserChildren,  13, List*,            list, a0);
      PROC1(            HideAllListBrowserChildren,  14, List*,            list, a0);
      PROC1(            FreeListBrowserList,         15, List*,            list, a0);
      FUNC2(ColumnInfo*,AllocLBColumnInfoA,          16, uint16,           cols, d0, const TagItem*,     tags, a0);
#ifdef __AMIGAOS4__
      PROC0(            dummy3,                      16);
#endif
      FUNC2(int32,      SetLBColumInfoAttrsA,        17, ColumnInfo*,      info, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy4,                      17);
#endif
      FUNC2(int32,      GetLBColumInfoAttrsA,        18, ColumnInfo*,      info, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy5,                      18);
#endif
      PROC1(            FreeLBColumnInfo,            19, ColumnInfo*,      info, a0);
      PROC1(            ListBrowserClearAll,         20, List*,            list, a0);

   ENDDECL

#undef MOS_DIRECT_OS
#endif

