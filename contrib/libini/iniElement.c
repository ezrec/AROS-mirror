/**********************************************************************************
 * iniElement
 *
 * Use when;
 * 1. strtok is scary (also does not handle empty elements well)
 * 2. strstr is not portable
 * 3. performance is less important than simplicity and the above (feel free to improve on this)
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
	AROS_LH6(int, iniElement,

/*  SYNOPSIS */
	AROS_LHA(char *, pszData, A0),
	AROS_LHA(char, cSeperator, D0),
	AROS_LHA(char, cTerminator, D1),
    AROS_LHA(int, nElement, D2),
	AROS_LHA(char *, pszElement, A1),
	AROS_LHA(int, nMaxElement, D3),

/*  LOCATION */
	struct Library *, inifileBase, 14, inifile)

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

	int nCurElement		= 0;
	int nChar			= 0;
	int nCharInElement	= 0;

	memset( pszElement, '\0', nMaxElement );
	for ( ; nCurElement <= nElement && (nCharInElement+1) < nMaxElement; nChar++ )
	{
		/* check for end of data */
        if ( cSeperator != cTerminator && pszData[nChar] == cTerminator )
        {
			break;
        }

		if ( cSeperator == cTerminator && pszData[nChar] == cSeperator && pszData[nChar+1] == cTerminator )
        {
			break;
        }

		/* check for end of element */
		if ( pszData[nChar] == cSeperator )
        {
			nCurElement++;
        }
		else if ( nCurElement == nElement )
		{
			pszElement[nCharInElement] = pszData[nChar];
            nCharInElement++;
		}
	}

	if ( pszElement[0] == '\0' )
    {
		return INI_NO_DATA;
    }

	return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniElement */

/*****************************************************************************

    NAME */
	AROS_LH6(int, iniElementToEnd,

/*  SYNOPSIS */
	AROS_LHA(char *, pszData, A0),
	AROS_LHA(char, cSeperator, D0),
	AROS_LHA(char, cTerminator, D1),
    AROS_LHA(int, nElement, D2),
	AROS_LHA(char *, pszElement, A1),
	AROS_LHA(int, nMaxElement, D3),

/*  LOCATION */
	struct Library *, inifileBase, 15, inifile)

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


    int nCurElement     = 0;
    int nChar           = 0;
    int nCharInElement  = 0;

    memset( pszElement, '\0', nMaxElement );
    for ( ; nCurElement <= nElement && (nCharInElement+1) < nMaxElement; nChar++ )
    {
        /* check for end of data */
        if ( cSeperator != cTerminator && pszData[nChar] == cTerminator )
            break;

        if ( cSeperator == cTerminator && pszData[nChar] == cSeperator && pszData[nChar+1] == cTerminator )
            break;

        /* check for end of element */
        if ( pszData[nChar] == cSeperator && ( nCurElement < nElement ))
            nCurElement++;
        else if ( nCurElement == nElement )
        {
            pszElement[nCharInElement] = pszData[nChar];
            nCharInElement++;
        }
    }

    if ( pszElement[0] == '\0' )
        return INI_NO_DATA;

    return INI_SUCCESS;

        AROS_LIBFUNC_EXIT
} /* iniElementToEnd */

