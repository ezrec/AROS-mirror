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

#ifndef __OOP_CLICKTABCLASS_H_
#define __OOP_CLICKTABCLASS_H_

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>

struct Node;
struct TagItem;

   BEGINDECL(ClickTab, "gadgets/clicktab.gadget")
      FUNC0(Class*,     GetClickTabClass,       5);
      FUNC1(Node*,      AllocClickTabNodeA,     6, const TagItem*,   tags, a0);
#ifdef __AMIGAOS4__
      PROC0(            dummy0,                 6);
#endif
      PROC1(            FreeClickTabNode,       7, Node*,            node, a0);
      PROC2(            SetClickTabNodeAttrsA,  8, Node*,            node, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy1,                 8);
#endif
      PROC2(            GetClickTabNodeAttrsA,  9, Node*,            node, a0, const TagItem*,     tags, a1);
#ifdef __AMIGAOS4__
      PROC0(            dummy2,                 9);
#endif

   ENDDECL

#undef MOS_DIRECT_OS
#endif

