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
	AROS_LH2(int, iniObject,

/*  SYNOPSIS */
    AROS_LHA(HINI, hIni, D0),
    AROS_LHA(char *, pszObject, A0),
	
/*  LOCATION */
	struct Library *, inifileBase, 19, inifile)

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
	
    /* Ok */
	strncpy( pszObject, hIni->hCurObject->szName, INI_MAX_OBJECT_NAME );

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniObject */


