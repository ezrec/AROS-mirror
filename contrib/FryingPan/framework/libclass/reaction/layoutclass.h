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

#ifndef __OOP_LAYOUTCLASS_H_
#define __OOP_LAYOUTCLASS_H_

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS

   BEGINDECL(Layout, "gadgets/layout.gadget")
      FUNC0(Class*,           GetLayoutClass,         5);
      FUNC4(bool,             ActivateLayoutGadget,   6,    class Gadget*,    gadget,  a0,   class Window*,       window,  a1,   class Requester*,    requester,  a2,   unsigned long,    object,     d0);
      FUNC1(struct Gadget*,   FlushLayoutDomainCache, 7,    class Gadget*,    gadget,  a0);
      FUNC4(bool,             RethinkLayout,          8,    class Gadget*,    gadget,  a0,   class Window*,       window,  a1,   class Requester*,    requester,  a2,   bool,             refresh,    d0);
      PROC4(                  LayoutLimits,           9,    class Gadget*,    gadget,  a0,   class LayoutLimits*, limits,  a1,   class TextFont*,     font,       a2,   class Screen*,    screen,     a3);
      FUNC0(Class*,           GetPageClass,           10);
      FUNC5(unsigned long,    SetPageGadgetAttrsA,    11,   class Gadget*,    gadget,  a0,   Object*,             object,  a1,   class Window*,       window,     a2,   class Requester*, requester,  a3,   class TagItem*,   tags,    d2);
      PROC4(                  RefreshPageGadget,      12,   class Gadget*,    gadget,  a0,   Object*,             object,  a1,   class Window*,       window,     a2,   class Requester*, requester,  a3);
   ENDDECL

#endif

