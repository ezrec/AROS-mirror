/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.

    $Id$
*/

#include <exec/types.h>
#include <exec/libraries.h>

#include <aros/libcall.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include LC_LIBDEFS_FILE

#define DBEUG 1
#include <aros/debug.h>

/****************************************************************************************/

#undef SysBase

#define LC_RESIDENTNAME 	Tapinterface_resident
#define LC_RESIDENTFLAGS	RTF_AUTOINIT|RTF_COLDSTART
#define LC_RESIDENTPRI		0
#define LC_STATIC_INITLIB

/* Customize libheader.c */
/* #define LC_NO_INITLIB    */
/* #define LC_NO_OPENLIB    */
/* #define LC_NO_CLOSELIB   */
/* #define LC_NO_EXPUNGELIB */

#include <libcore/libheader.c>

#define SysBase			(LC_SYSBASE_FIELD(TapInterfaceBase))

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LC_LIBHEADERTYPEPTR TapInterfaceBase)
{
    D(bug("Inside Init func of tapinterface.library\n"));
    return TRUE;
}

/****************************************************************************************/

ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBHEADERTYPEPTR TapInterfaceBase)
{
    D(bug("Inside Open func of tapinterface.library\n"));

    return TRUE;
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBHEADERTYPEPTR TapInterfaceBase)
{
    D(bug("Inside Close func of tapinterface.library\n"));
}

/****************************************************************************************/

void  SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LC_LIBHEADERTYPEPTR TapInterfaceBase)
{
    D(bug("Inside Expunge func of tapinterface.library\n"));
}

/****************************************************************************************/
