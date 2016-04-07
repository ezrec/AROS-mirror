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
	AROS_LH1(int, iniToUpper,

/*  SYNOPSIS */
	AROS_LHA(char *, pszString, A0),
	
/*  LOCATION */
	struct Library *, inifileBase, 41, inifile)

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
    
	int n = 0;

	for ( n = 0; pszString[n] != '\0'; n++ )
		pszString[n] = toupper(pszString[n]);

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniToUpper */



