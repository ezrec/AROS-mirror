/**********************************************************************************
 * iniGotoBookmark
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
	AROS_LH1(int, iniGotoBookmark,

/*  SYNOPSIS */
	AROS_LHA(INIBOOKMARK, IniBookmark, D0),
	
/*  LOCATION */
	struct Library *, inifileBase, 18, inifile)

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
    
    if ( IniBookmark.hIni == NULL )
		return INI_ERROR;
	
	(IniBookmark.hIni)->hCurObject 		= IniBookmark.hCurObject;
	(IniBookmark.hIni)->hCurProperty	= IniBookmark.hCurProperty;

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniGotoBookmark */
