/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Freedom initialization code.
    Lang: English
*/

#define AROS_ALMOST_COMPATIBLE
#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>

#include <proto/exec.h>

#include "freedom_intern.h"
#include "libdefs.h"

/****************************************************************************************/

#undef SysBase

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   (FB(lib)->sysbase)
#define LC_SEGLIST_FIELD(lib)   (FB(lib)->seglist)
#define LC_LIBBASESIZE		sizeof(struct FreedomBase_intern)
#define LC_LIBHEADERTYPEPTR	LIBBASETYPEPTR
#define LC_LIB_FIELD(lib)	(FB(lib)->library)

/* #define LC_NO_INITLIB    */
/* #define LC_NO_OPENLIB    */
/* #define LC_NO_CLOSELIB   */
/* #define LC_NO_EXPUNGELIB */

#include <libcore/libheader.c>

#undef DEBUG
#define DEBUG 1
#include <aros/debug.h>

struct ExecBase *SysBase;
struct ExecBase **SysBasePtr = &SysBase;

struct Library *FreedomBase;
struct Library **FreedomBasePtr = &FreedomBase;

#define SysBase			(LC_SYSBASE_FIELD(FreedomBase))

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR FreedomBase)
{
    D(bug("Inside Init func of freedom.library\n"));

    *SysBasePtr = SysBase;
    *FreedomBasePtr = FreedomBase;
    
    if (!UtilityBase)
        (struct Library *)UtilityBase = OpenLibrary("utility.library", 37);
    if (!UtilityBase)
        return FALSE;

    if (!DOSBase)
    	(struct Library *)DOSBase = OpenLibrary("dos.library", 37);
    if (!DOSBase)
    	return FALSE;
	
    return TRUE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR FreedomBase)
{
    D(bug("Inside Open func of freedom.library\n"));

    return TRUE;
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR FreedomBase)
{
    D(bug("Inside Close func of freedom.library\n"));
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR FreedomBase)
{
    D(bug("Inside Expunge func of freedom.library\n"));

    /* CloseLibrary() checks for NULL-pointers */

    CloseLibrary((struct Library *)DOSBase);
    DOSBase = NULL;
    
    CloseLibrary((struct Library *)UtilityBase);
    UtilityBase = NULL;
}

/****************************************************************************************/
