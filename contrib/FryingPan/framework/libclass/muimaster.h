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

#ifndef _LIBCLASS_MUIMASTER_H_
#define _LIBCLASS_MUIMASTER_H_

#if defined (_PROTO_MUIMASTER_H) || defined (PROTO_MUIMASTER_H)
#error "Please remove proto/muimaster includes first!"
#endif

#define _PROTO_MUIMASTER_H
#define PROTO_MUIMASTER_H

#include <libclass/exec.h>

extern class MUIMasterIFace *MUIMaster;

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#include <intuition/classusr.h>

struct IClass;
struct TagItem;
struct MUI_CustomClass;
struct Library;
struct MUI_RenderInfo;
struct MUI_PenSpec;
struct Region;

   BEGINDECL(MUIMaster, "muimaster.library")
	   FUNC2(Object*,          MUI_NewObjectA,         5, const char*,      name,    a0, const TagItem*,     tags,    a1);
#ifdef __AMIGAOS4__
      PROC0(                  MUI_dummy1,             5);
#endif
      PROC1(                  MUI_DisposeObject,      6, Object*,          object,  a0);
      FUNC7(int32,            MUI_RequestA,           7, Object*,          app,     d0, Object*,            window,  d1, uint32,          flags,      d2, const char*,     title,   a0, const char*,     gadgets,    a1, const char*,     format,  a2, void*,        params,  a3);
#ifdef __AMIGAOS4__
      PROC0(                  MUI_dummy2,             7);
#endif
      FUNC2(void*,            MUI_AllocAslRequest,    8, uint32,           type,    d0, const  TagItem*,    tags,    a0);
#ifdef __AMIGAOS4__
      PROC0(                  MUI_dummy3,             8);
#endif
      FUNC2(bool,             MUI_AslRequest,         9, void*,            req,     a0, const  TagItem*,    tags,    a1); 
#ifdef __AMIGAOS4__
      PROC0(                  MUI_dummy4,             9);
#endif
      PROC1(                  MUI_FreeAslRequest,    10, void*,            req,     a0);
      FUNC0(int32,            MUI_Error,             11);
      FUNC1(int32,            MUI_SetError,          12, int32,            last,    d0);
      FUNC1(IClass*,          MUI_GetClass,          13, const char*,      name,    a0);
      PROC1(                  MUI_FreeClass,         14, IClass*,          iclass,  a0);
      PROC2(                  MUI_RequestIDCMP,      15, Object*,          object,  a0, uint32,             idcmp,   d0);
      PROC2(                  MUI_RejectIDCMP,       16, Object*,          object,  a0, uint32,             idcmp,   d0);
      PROC2(                  MUI_Redraw,            17, Object*,          object,  a0, uint32,             flags,   d0);
      FUNC5(MUI_CustomClass*, MUI_CreateCustomClass, 18, Library*,         base,    a0, const char*,        name,    a1, MUI_CustomClass*,superclass, a2, int32,        datasize,   d0, void*,        dispatcher,    a3);
      FUNC1(bool,             MUI_DeleteCustomClass, 19, MUI_CustomClass*, cclass,  a0);
      FUNC2(Object*,          MUI_MakeObjectA,       20, IPTR,           type,    d0, IPTR,            params,  a0);
#ifdef __AMIGAOS4__
      PROC0(                  MUI_dummy5,            20);
#endif
      FUNC6(bool,             MUI_Layout,            21, Object*,          object,  a0, int32,              left,    d0, int32,           top,        d1, int32,        width,      d2, int32,        height,        d3, int32,        flags,      d4);
      PROC0(                  MUI_dummy6,            22);
      PROC0(                  MUI_dummy7,            23);
      PROC0(                  MUI_dummy8,            24);
      PROC0(                  MUI_dummy9,            25);
      FUNC3(int32,            MUI_ObtainPen,         26, MUI_RenderInfo*,  rinfo,   a0, MUI_PenSpec*,       penspec, a1, uint32,          flags,      d0);
      PROC2(                  MUI_ReleasePen,        27, MUI_RenderInfo*,  rinfo,   a0, int32,              pennum,  d0);
      FUNC5(void*,            MUI_AddClipping,       28, MUI_RenderInfo*,  rinfo,   a0, int32,              left,    d0, int32,           top,        d1, int32,        width,      d2, int32,        height,        d3);
      PROC2(                  MUI_RemoveClipping,    29, MUI_RenderInfo*,  rinfo,   a0, void*,              clip,    a1);
      FUNC2(void*,            MUI_AddClipRegion,     30, MUI_RenderInfo*,  rinfo,   a0, Region*,            region,  a1);
      PROC2(                  MUI_RemoveClipRegion,  31, MUI_RenderInfo*,  rinfo,   a0, void*,              clip,    a1);
      FUNC2(bool,             MUI_BeginRefresh,      32, MUI_RenderInfo*,  rinfo,   a0, uint32,             flags,   d0);
      PROC2(                  MUI_EndRefresh,        33, MUI_RenderInfo*,  rinfo,   a0, uint32,             flags,   d0);

   ENDDECL

#undef MOS_DIRECT_OS

#endif /*_LIBCLASS_MUIMASTER_H_*/
