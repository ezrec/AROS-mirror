/*
    Copyright © 1995-2006, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Init of xprzmodem.library
    Lang: english
*/
#include <utility/utility.h>
#include <aros/symbolsets.h>

#include LC_LIBDEFS_FILE

extern int  __UserLibInit (LIBBASETYPE *);
extern void __UserLibCleanup (LIBBASETYPE *);

static int Init(LIBBASETYPEPTR lh)
{
	if (RETURN_OK == __UserLibInit(lh)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static int Expunge(LIBBASETYPEPTR lh)
{
	__UserLibCleanup(lh);
	return TRUE;
}
    
ADD2INITLIB(Init, 0);
ADD2EXPUNGELIB(Expunge, 0);
