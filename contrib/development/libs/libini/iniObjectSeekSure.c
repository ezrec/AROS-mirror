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
 * -----------------------------------------------
 *
 * PAH	06.MAR.99	Added this func
 **************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH2(int, iniObjectSeekSure,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszObject, A0),
	
/*  LOCATION */
	struct Library *, inifileBase, 27, inifile)

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

	if ( (nReturn = iniObjectSeek( hIni, pszObject )) == INI_NO_DATA )
        return iniObjectInsert( hIni, pszObject );

	return nReturn;

    AROS_LIBFUNC_EXIT
} /* iniObjectSeekSure */

