/**********************************************************************************
 * iniElementCount
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 05.APR.99
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
	AROS_LH3(int, iniElementCount,

/*  SYNOPSIS */
	AROS_LHA(char *, pszData, A0),
	AROS_LHA(char , cSeperator, D0),
	AROS_LHA(char , cTerminator, D1),

/*  LOCATION */
	struct Library *, inifileBase, 16, inifile)

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

	int	nToManyElements = 30000;
	int nCurElement		= 0;
	int nChar			= 0;

	for ( ; nCurElement <= nToManyElements; nChar++ )
	{
		/* check for end of data */
        if ( cSeperator != cTerminator && pszData[nChar] == cTerminator )
			break;

		if ( cSeperator == cTerminator && pszData[nChar] == cSeperator && pszData[nChar+1] == cTerminator )
			break;

		/* check for end of element */
		if ( pszData[nChar] == cSeperator )
			nCurElement++;
	}

	return nCurElement;

    AROS_LIBFUNC_EXIT
} /* iniElementCount */


