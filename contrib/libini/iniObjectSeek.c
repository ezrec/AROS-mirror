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
 * PAH = Peter Harvey		- pharvey@codebydesign.com
 * -----------------------------------------------
 *
 * PAH	19.MAR.99	Now sets hCurProperty to hFirstProperty when found
 **************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH2(int, iniObjectSeek,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszObject, A0),

/*  LOCATION */
	struct Library *, inifileBase, 26, inifile)

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

	iniObjectFirst( hIni );
	while ( iniObjectEOL( hIni ) == FALSE )
	{
        if ( strcasecmp( pszObject, hIni->hCurObject->szName ) == 0 )
            return INI_SUCCESS;
		iniObjectNext( hIni );
	}

	return INI_NO_DATA;

    AROS_LIBFUNC_EXIT
} /* iniObjectSeek */

