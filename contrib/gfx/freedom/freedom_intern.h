#ifndef FREEDOM_INTERN_H
#define FREEDOM_INTERN_H

/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Internal definitions for freedom.library
    Lang: English
*/

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef EXEC_MEMORY_H
#   include <exec/memory.h>
#endif
#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif
#ifndef DOS_DOS_H
#   include <dos/dos.h>
#endif
#ifndef DOS_DOSEXTENS_H
#   include <dos/dosextens.h>
#endif
#ifndef UTILITY_UTILITY_H
#   include <utility/utility.h>
#endif
#ifndef DISKFONT_DISKFONT_H
#   include <diskfont/diskfont.h>
#endif
#ifndef DISKFONT_GLYPH_H
#   include <diskfont/glyph.h>
#endif
#ifndef DISKFONT_DISKFONTTAG_H
#   include <diskfont/diskfonttag.h>
#endif
#ifndef DISKFONT_OTERRORS_H
#   include <diskfont/oterrors.h>
#endif

#ifndef MYGLYPHENGINE_H
#include "myglyphengine.h"
#endif

/****************************************************************************************/

struct FreedomBase_intern
{
    struct Library		library;
    struct ExecBase		*sysbase;
    BPTR			seglist;
    struct DosLibrary	    	*dosbase;
    struct UtilityBase		*utilitybase;
};

/****************************************************************************************/

void ClearGlyphRequest(struct MyGlyphEngine *glyphEngine);

/****************************************************************************************/

#define FIXED_TO_FIXED26_DIV (65536/64)
#define FIXED26_TO_FIXED(x) ((x) << (16 - 6))
#define FIXED_TO_FIXED26(x) ((x) >> (16 - 6))

#define FIXED_TO_APPROX(x) (((x) + 0x8000) >> 16)

#define FLOOR(x) ((x) & -64)

#define CEILING(x) (((x) + 63) & -64)

/****************************************************************************************/

#undef FB
#define FB(b)		((struct FreedomBase_intern *)b)

#undef SysBase
#define SysBase		(FB(FreedomBase)->sysbase)

#undef DOSBase
#define DOSBase     	(FB(FreedomBase)->dosbase)

#undef UtilityBase
#define UtilityBase	(FB(FreedomBase)->utilitybase)

/****************************************************************************************/

#endif /* FREEDOM_INTERN_H */
