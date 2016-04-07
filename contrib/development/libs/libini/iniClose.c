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
	AROS_LH1(int, iniClose,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	
/*  LOCATION */
	struct Library *, inifileBase, 10, inifile)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

        iniClose

        1. free memory previously allocated for HINI
        2. DO NOT save any changes (see iniCommit)

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

	hIni->hCurObject = hIni->hFirstObject;
	while ( iniObjectDelete( hIni ) == INI_SUCCESS )
	{
	}
	
	free( hIni );

	return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniClose */



