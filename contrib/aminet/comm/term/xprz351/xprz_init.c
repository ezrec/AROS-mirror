/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Init of xprzmodem.library
    Lang: english
*/
#include <utility/utility.h>

#include "libdefs.h"

extern struct ExecBase * SysBase;

#define LC_NO_OPENLIB
#define LC_NO_CLOSELIB
#define LC_RESIDENTPRI	    -120

#define AROS_LC_SETFUNCS

#include <libcore/libheader.c>


extern int  __UserLibInit (LC_LIBHEADERTYPEPTR *);
extern void __UserLibCleanup (LC_LIBHEADERTYPEPTR *);

ULONG SAVEDS L_InitLib (LC_LIBHEADERTYPEPTR lh)
{
	SysBase = lh->lh_SysBase;
	if (RETURN_OK == __UserLibInit(lh)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void SAVEDS L_ExpungeLib (LC_LIBHEADERTYPEPTR lh)
{
	__UserLibCleanup(lh);
}
