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
	AROS_LH1(int, iniPropertyFirst,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	
/*  LOCATION */
	struct Library *, inifileBase, 33, inifile)

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
	
    hIni->hCurProperty = hIni->hCurObject->hFirstProperty;

	if ( hIni->hCurProperty == NULL )
		return INI_NO_DATA;

	return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniPropertyFirst */
