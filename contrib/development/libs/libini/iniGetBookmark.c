/**********************************************************************************
 * iniGetBookmark
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * PAH = Peter Harvey		- pharvey@codebydesign.com
 * -----------------------------------------------
 *
 * PAH	18.MAR.99	Created.
 **************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************

    NAME */
	AROS_LH2(int, iniGetBookmark,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(HINIBOOKMARK, hIniBookmark, D1),

/*  LOCATION */
	struct Library *, inifileBase, 17, inifile)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

	if ( hIni == NULL || hIniBookmark == NULL )
		return INI_ERROR;
	
   	hIniBookmark->hIni 			= hIni;
	hIniBookmark->hCurObject	= hIni->hCurObject;
	hIniBookmark->hCurProperty	= hIni->hCurProperty;

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniGetBookmark */
