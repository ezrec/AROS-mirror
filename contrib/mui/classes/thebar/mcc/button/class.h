/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#ifndef _CLASS_H
#define _CLASS_H

#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <graphics/rpattr.h>
#include <graphics/scale.h>
#include <utility/pack.h>
#include <datatypes/pictureclass.h>
#include <cybergraphx/cybergraphics.h>
#include <diskfont/glyph.h>

#include <mui/TheBar_mcc.h>
#include <mui/TheBar_mcp.h>

#include <string.h>
#include <stdlib.h>

#include <mcc_common.h>
#include <SDI_compiler.h>
#include <SDI_stdarg.h>
#include <SDI_hook.h>

#if defined(__amigaos4__)
#include <hardware/blit.h>
#endif

#include "debug.h"

/****************************************************************************/
/*
** Libraries shared stuff
*/

#if defined(__amigaos4__)
extern struct Library          *SysBase;
extern struct Library          *DOSBase;
extern struct Library          *GfxBase;
extern struct Library          *IntuitionBase;
#else
extern struct ExecBase         *SysBase;
extern struct DosLibrary       *DOSBase;
extern struct IntuitionBase    *IntuitionBase;
extern struct GfxBase          *GfxBase;
#endif
#if defined(__AROS__)
extern struct UtilityBase      *UtilityBase;
#else
extern struct Library          *UtilityBase;
#endif
extern struct Library          *MUIMasterBase;

extern struct Library          *DataTypesBase;
extern struct Library          *CyberGfxBase;
extern struct Library          *DiskfontBase;
extern struct Library          *PictureDTBase;

extern struct SignalSemaphore  lib_poolSem;
extern APTR                    lib_pool;
extern ULONG                   lib_alpha;
extern ULONG                   lib_flags;

enum
{
    BASEFLG_Init  = 1<<0,
    BASEFLG_MUI20 = 1<<1,
    BASEFLG_MUI4  = 1<<2,
};

/****************************************************************************/
/*
** Macros
*/

/* these systems are able to handle alpha channel information */
#if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
    #define WITH_ALPHA          1
#endif

#define _riflags(obj) (muiRenderInfo(obj)->mri_Flags)

#define RAWIDTH(w)                      ((((UWORD)(w))+15)>>3 & 0xFFFE)
#define BOOLSAME(a,b)                   (((a) ? TRUE : FALSE)==((b) ? TRUE : FALSE))

#define getconfigitem(cl,obj,item,ptr)  DoSuperMethod(cl,obj,MUIM_GetConfigItem,item,(IPTR)ptr)
#define superset(cl,obj,tag,val)        SetSuperAttrs(cl,obj,tag,(IPTR)(val),TAG_DONE)
#define superget(cl,obj,tag,storage)    DoSuperMethod(cl,obj,OM_GET,tag,(IPTR)(storage))
#define nnsuperset(cl,obj,tag,val)      SetSuperAttrs(cl,obj,tag,(IPTR)(val),MUIA_NoNotify,TRUE,TAG_DONE)
#undef set
#define set(obj,attr,value)             SetAttrs((Object *)(obj),(ULONG)(attr),(IPTR)(value),TAG_DONE)
#undef get
#define get(obj,attr,store)             GetAttr((ULONG)(attr),(APTR)obj,(IPTR *)((IPTR)(store)))

#define setFlag(mask, flag)             (mask) |= (flag)
#define clearFlag(mask, flag)           (mask) &= ~(flag)
#define isAnyFlagSet(mask, flag)        (((mask) & (flag)) != 0)
#define isFlagSet(mask, flag)           (((mask) & (flag)) == (flag))
#define isFlagClear(mask, flag)         (((mask) & (flag)) == 0)

#ifdef __MORPHOS__
#undef NewObject
#undef MUI_NewObject
#undef DoSuperNew
APTR NewObject(struct IClass *classPtr,CONST_STRPTR classID,ULONG tag1,...);
Object *MUI_NewObject(CONST_STRPTR classname,Tag tag1,...);
#endif

// xget()
// Gets an attribute value from a MUI object
IPTR xget(Object *obj, const IPTR attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({IPTR b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif

/***********************************************************************/

enum
{
    MINTERM_ZERO        = 0,
    MINTERM_ONE         = ABC | ABNC | ANBC | ANBNC | NABC | NABNC | NANBC | NANBNC,
    MINTERM_COPY        = ABC | ABNC | NABC | NABNC,
    MINTERM_NOT_C       = ABNC | ANBNC | NABNC | NANBNC,
    MINTERM_B_AND_C     = ABC | NABC,
    MINTERM_NOT_B_AND_C = ANBC | NANBC,
    MINTERM_B_OR_C      = ABC | ABNC | NABC | NABNC | ANBC | NANBC,
};

/****************************************************************************/
/*
** stripUnderscore() modes
*/

enum
{
    STRIP_First,
    STRIP_All,
};

/***********************************************************************/

struct scale
{
    UWORD w;
    UWORD tw;
    UWORD sl;
    UWORD st;
    UWORD sw;
    UWORD sh;
    UWORD dw;
    UWORD dh;
};

/***********************************************************************/
/*
** MUI undocs
*/

#ifndef MUIM_Backfill
#define MUIM_Backfill 0x80428d73UL
struct  MUIP_Backfill        { STACKED ULONG MethodID; STACKED LONG left; STACKED LONG top; STACKED LONG right; STACKED LONG bottom; STACKED LONG xoffset; STACKED LONG yoffset; STACKED LONG lum; };
#endif

#ifndef MUIA_CustomBackfill
#define MUIA_CustomBackfill  0x80420a63UL
#endif

#ifndef MUIM_CustomBackfill
#define MUIM_CustomBackfill  MUIM_Backfill
#endif

#ifndef MUIP_CustomBackfill
#define MUIP_CustomBackfill  MUIP_Backfill
#endif

#ifndef IDCMP_MOUSEOBJECT
#define IDCMP_MOUSEOBJECT 0x40000000UL
#endif

#ifndef MUIA_FrameDynamic
#define MUIA_FrameDynamic 0x804223c9UL
#endif

#ifndef MUIA_FrameVisible
#define MUIA_FrameVisible 0x80426498UL
#endif

/***********************************************************************/

#include "class_protos.h"

/***********************************************************************/

#endif /* _CLASS_H */
