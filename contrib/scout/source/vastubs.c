/***************************************************************************

 YAM - Yet Another Mailer
 Copyright (C) 1995-2000 by Marcel Beck <mbeck@yam.ch>
 Copyright (C) 2000-2010 by YAM Open Source Team

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 YAM Official Support Site :  http://www.yam.ch/
 YAM OpenSource project    :  http://sourceforge.net/projects/yamos/

 $Id$

***************************************************************************/

/*
   Stubs for the variable argument functions of the shared libraries used by
   YAM. Please note that these stubs should only be used if the compiler
   suite/SDK doesn't come with own stubs/inline functions.

   Also note that these stubs are only safe on m68k machines as it
   requires a linear stack layout!
*/

#if !defined(__AROS__) && (defined(__VBCC__) || defined(NO_INLINE_STDARG))
#if defined(_M68000) || defined(__M68000) || defined(__mc68000)

#include <exec/types.h>

/* FIX V45 breakage... */
#if INCLUDE_VERSION < 45
#define MY_CONST_STRPTR CONST_STRPTR
#else
#define MY_CONST_STRPTR CONST STRPTR
#endif

#include <proto/intuition.h>
APTR NewObject( struct IClass *classPtr, CONST_STRPTR classID, Tag tag1, ... )
{ return NewObjectA(classPtr, classID, (struct TagItem *)&tag1); }
ULONG SetAttrs( APTR object, ULONG tag1, ... )
{ return SetAttrsA(object, (struct TagItem *)&tag1); }
LONG EasyRequest(struct Window *window, CONST struct EasyStruct *es, ULONG *idcmpPtr, ... )
{ return EasyRequestArgs(window, es, idcmpPtr, (CONST APTR)(&idcmpPtr+1)); }

#include <proto/dos.h>
LONG Printf( CONST_STRPTR format, ... )
{ return VPrintf(format, (CONST APTR)(&format+1)); }
LONG FPrintf( BPTR fh, CONST_STRPTR format, ... )
{ return VFPrintf(fh, format, (CONST APTR)(&format+1)); }

#include <proto/locale.h>
struct Catalog *OpenCatalog( struct Locale *locale, STRPTR name, Tag tag1, ... )
{ return OpenCatalogA(locale, name, (struct TagItem *)&tag1); }

#include <proto/mmu.h>
ULONG GetProperties(struct MMUContext *ctx, ULONG lower, Tag tag1, ... )
{ return GetPropertiesA(ctx, lower, (struct TagItem *)&tag1); }

#include <proto/identify.h>
LONG IdAlertTags(ULONG alert, Tag tag1, ... )
{ return IdAlert(alert, (struct TagItem *)&tag1); }
LONG IdExpansionTags(Tag tag1, ... )
{ return IdExpansion((struct TagItem *)&tag1); }
LONG IdFunctionTags(STRPTR libName, LONG offset, Tag tag1, ... )
{ return IdFunction(libName, offset, (struct TagItem *)&tag1); }
STRPTR IdHardwareTags(ULONG type, Tag tag1, ... )
{ return IdHardware(type, (struct TagItem *)&tag1); }
ULONG IdHardwareNumTags(ULONG type, Tag tag1, ... )
{ return IdHardwareNum(type, (struct TagItem *)&tag1); }

#include <proto/ahi.h>
BOOL AHI_GetAudioAttrs(ULONG ID, struct AHIAudioCtrl *ctrl, Tag tag1, ... )
{ return AHI_GetAudioAttrsA(ID, ctrl, (struct TagItem *)&tag1); }

#else
  #error "VARGS stubs are only save on m68k systems!"
#endif // !defined(__PPC__)

#endif
