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
	AROS_LH2(int, iniProperty,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszProperty, A0),

/*  LOCATION */
	struct Library *, inifileBase, 30, inifile)

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

    /* Ok */
    strncpy( pszProperty, hIni->hCurProperty->szName, INI_MAX_PROPERTY_NAME );

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniProperty */


