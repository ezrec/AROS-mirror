/**********************************************************************************
 * iniCommit
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
	AROS_LH1(int, iniCommit,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	
/*  LOCATION */
	struct Library *, inifileBase, 11, inifile)

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

    FILE    *hFile;

    /* SANITY CHECK */
    if ( hIni == NULL )
        return INI_ERROR;

	if ( hIni->bReadOnly )
        return INI_ERROR;

    /* OPEN FILE */
    hFile = fopen( hIni->szFileName, "w" );
    if ( !hFile )
        return INI_ERROR;

	_iniDump( hIni, hFile );

    /* CLEANUP */
    if ( hFile != NULL )
		fclose( hFile );

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* inifile_iniAllTrim */
