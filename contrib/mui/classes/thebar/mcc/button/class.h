/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id: class.h 205 2008-02-19 08:38:17Z thboeckel $

***************************************************************************/

#ifndef _CLASS_H
#define _CLASS_H

#include <diskfont/glyph.h>

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

#include <string.h>

#include <mui/TheBar_mcc.h>

#include "private.h"

#include "SDI_compiler.h"

// these systems are able to handle alpha channel information
#if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
	#define WITH_ALPHA			1
#endif

/****************************************************************************/

#define _riflags(obj) (muiRenderInfo(obj)->mri_Flags)

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

// xget()
// Gets an attribute value from a MUI object
#ifdef __AROS__
#define xget XGET
#else
ULONG xget(Object *obj, const ULONG attr);
#if defined(__GNUC__)
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({ULONG b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif
#endif /* __AROS__ */

/***********************************************************************/

/* utils.c */
#ifdef __MORPHOS__
#elif defined(__AROS__)
Object *DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...);
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#endif
APTR allocVecPooled(APTR pool, ULONG size);
void freeVecPooled(APTR pool, APTR mem);
void stripUnderscore(STRPTR dest , STRPTR from, ULONG mode);
struct TextFont *openFont(STRPTR name);
APTR gmalloc(ULONG size);
void gfree(APTR mem);
ULONG peekQualifier(void);

/* brc1.c */
int BRCUnpack(APTR pSource, APTR pDest, LONG srcBytes0, LONG dstBytes0);

/* scale.c */
void scale(struct scale *sce , UBYTE *src , UBYTE *dst);
void scaleRGB(struct scale *sce , ULONG *src , ULONG *dst);

/* build.c */
void scaleStripBitMaps(struct InstData *data);
void freeBitMaps(struct InstData *data);
void build(struct InstData *data);

/***********************************************************************/

#endif /* _CLASS_H */
