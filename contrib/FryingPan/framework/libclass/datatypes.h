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

#ifndef _LIBCLASS_DATATYPES_H_
#define _LIBCLASS_DATATYPES_H_ 

#if defined (_PROTO_DATATYPES_H) || defined (PROTO_DATATYPES_H)
#error "Please remove proto/datatypes includes first!"
#endif

#define _PROTO_DATATYPES_H
#define PROTO_DATATYPES_H

#include <libclass/exec.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#include <Generic/Types.h>
#include <intuition/classusr.h>

extern class DatatypesIFace *Datatypes;

struct DataType;
struct TagItem;
struct Window;
struct Requester;
struct gpLayout;
struct DTMethods;
struct dtPrint;
struct RastPort;

   BEGINDECL(Datatypes, "datatypes.library")
      PROC0(                  private0,                              5);
      FUNC3(DataType*,        ObtainDataTypeA,                       6, uint32,     type,       d0, void*,     handle,     a0, TagItem*,  attrs,      a1);
#ifdef __AMIGAOS4__
      PROC0(                  private1,                              6);
#endif
      PROC1(                  ReleaseDataType,                       7, DataType*,  dt,         a0);
      FUNC2(Object*,          NewDTObjectA,                          8, void*,      name,       d0, TagItem*,  tags,       a0);
#ifdef __AMIGAOS4__
      PROC0(                  private2,                              8);
#endif
      PROC1(                  DisposeDTObject,                       9, Object*,    obj,        a0);
      FUNC4(int32,            SetDTAttrsA,                          10, Object*,    obj,        a0, Window*,   win,        a1, Requester*, req,       a2, TagItem*,     tags,    a3);
#ifdef __AMIGAOS4__
      PROC0(                  private3,                             10);
#endif
      FUNC2(int32,            GetDTAttrsA,                          11, Object*,    obj,        a0, TagItem*,  tags,       a2);
#ifdef __AMIGAOS4__
      PROC0(                  private4,                             11);
#endif
      FUNC4(int32,            AddDTObject,                          12, Window*,    win,        a0, Requester*, req,       a1, Object*,   obj,        a2, int32,        pos,     d0);
      FUNC4(int32,            RefreshDTObjectA,                     13, Object*,    obj,        a0, Window*,   win,        a1, Requester*, req,       a2, TagItem*,     attrs,   a3);
#ifdef __AMIGAOS4__
      PROC0(                  private5,                             13);
      PROC0(                  private6,                             13);
#endif
      FUNC2(uint32,           DoAsyncLayout,                        14, Object*,    obj,        a0, gpLayout*, layout,     a1);
      FUNC4(IPTR,           DoDTMethodA,                          15, Object*,    obj,        a0, Window*,   win,        a1, Requester*, req,       a2, IPTR,      message, a3);
#ifdef __AMIGAOS4__
      PROC0(                  private7,                             15);
#endif
      FUNC2(int32,            RemoveDTObject,                       16, Window*,    win,        a0, Object*,   obj,        a1);
      FUNC1(uint32*,          GetDTMethods,                         17, Object*,    obj,        a0);
      FUNC1(DTMethods*,       GetDTTriggerMethods,                  18, Object*,    obj,        a0);
      FUNC4(uint32,           PrintDTObjectA,                       19, Object*,    obj,        a0, Window*,   win,        a1, Requester*, req,       a2, dtPrint*,     print,   a3);
#ifdef __AMIGAOS4__
      PROC0(                  private8,                             19);
#endif
      FUNC2(void*,            ObtainDTDrawInfoA,                    20, Object*,    obj,        a0, TagItem*,  attrs,      a1);
#ifdef __AMIGAOS4__
      PROC0(                  private9,                             20);
#endif
      FUNC9(int32,            DrawDTObjectA,                        21, RastPort*, rast, a0, Object*, obj, a1, int32, x, d0, int32, y, d1, int32, w, d2, int32, h, d3, int32, th, d4, int32, tv, d5, TagItem*, attrs, a2);
#ifdef __AMIGAOS4__
      PROC0(                  private10,                            21);
#endif
      PROC2(                  ReleaseDTDrawInfo,                    22, Object*,    obj,        a0, void*,     handle,     a1);
      FUNC1(const char*,      GetDTString,                          23, uint32,     id,         d0);

   ENDDECL

#undef MOS_DIRECT_OS

#endif 
