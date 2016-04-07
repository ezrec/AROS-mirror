/*
    Copyright © 1995-2006, The AROS Development Team. All rights reserved.
    $Id$

    Desc: bsdsocket initialization code.
    Lang: English.
*/

/****************************************************************************************/

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>
#include <aros/macros.h>
#include <utility/utility.h>

#include "inifile_intern.h"

#define DEBUG 1
#   include <aros/debug.h>

#include LC_LIBDEFS_FILE

/****************************************************************************************/

static int Init(LIBBASETYPEPTR LIBBASE)
{
    D(bug(NAME_STRING ": Inside libinit func \n"));

	return TRUE;
}

/****************************************************************************************/

static int Open(LIBBASETYPEPTR LIBBASE)
{
    /*
      This function is single-threaded by exec by calling Forbid.
      If you break the Forbid() another task may enter this function
      at the same time. Take care.
    */
    D(bug(NAME_STRING ": Inside libopen func \n"));

	return TRUE;
}

/****************************************************************************************/

static int Expunge(LIBBASETYPEPTR LIBBASE)
{
    /*
	This function is single-threaded by exec by calling Forbid.
	Never break the Forbid() or strange things might happen.
    */

    /* Test for openers. */
    D(bug(NAME_STRING ": Inside libexpunge func \n"));

	return TRUE;
}

/****************************************************************************************/

static void Close(LIBBASETYPEPTR LIBBASE)
{
    /*
	This function is single-threaded by exec by calling Forbid.
	If you break the Forbid() another task may enter this function
	at the same time. Take care.
    */

    D(bug(NAME_STRING ": Inside libclose func\n"));

	return TRUE;
}

/****************************************************************************************/

ADD2INITLIB(Init, 0);
ADD2OPENLIB(Open, 0);
ADD2CLOSELIB(Close, 0);
ADD2EXPUNGELIB(Expunge, 0);
