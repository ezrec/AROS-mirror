/**********************************************************************************
 * iniPropertySeek
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * -----------------------------------------------
 *
 * PAH	06.MAR.99	Added this func
 **************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH4(int, iniPropertySeekSure,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszObject, A0),
    AROS_LHA(char *, pszProperty, A1),
    AROS_LHA(char *, pszValue, A2),

/*  LOCATION */
	struct Library *, inifileBase, 38, inifile)

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
    
	int nReturn;

    /* SANITY CHECKS */
    if ( hIni == NULL )
        return INI_ERROR;
	if ( !pszObject )
		return INI_ERROR;
	if ( !pszProperty )
		return INI_ERROR;
	if ( !pszValue )
		return INI_ERROR;

	/* OK */
	if ( (nReturn = iniPropertySeek( hIni, pszObject, pszProperty, "" )) == INI_NO_DATA )
	{
		iniObjectSeekSure( hIni, pszObject );
		return iniPropertyInsert( hIni, pszProperty, pszValue );
	}
	else if ( nReturn == INI_SUCCESS )
		return iniValue( hIni, pszValue );

	return nReturn;

    AROS_LIBFUNC_EXIT
} /* iniPropertySeekSure */

