/**********************************************************************************
 * .
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/


#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH2(int, iniValue,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszValue, A0),

/*  LOCATION */
	struct Library *, inifileBase, 42, inifile)

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

    /* SANITY CHECKS */
    if ( hIni == NULL )
        return INI_ERROR;

	if ( hIni->hCurObject == NULL )
		return INI_NO_DATA;
	
	if ( hIni->hCurProperty == NULL )
		return INI_NO_DATA;
	
    strncpy( pszValue, hIni->hCurProperty->szValue, INI_MAX_PROPERTY_VALUE );

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniValue */

