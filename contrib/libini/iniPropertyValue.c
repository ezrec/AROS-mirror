/**********************************************************************************
 * .
 *  							totally untested
 *
 * see iniElement instead	
 **********************************************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH5(int, iniPropertyValue,

/*  SYNOPSIS */
	AROS_LHA(char *, pszString, A0),
	AROS_LHA(char *, pszProperty, A1),
    AROS_LHA(char *, pszValue, A2),
    AROS_LHA(char, cEqual, D0),
    AROS_LHA(char, cPropertySep, D1),

/*  LOCATION */
	struct Library *, inifileBase, 40, inifile)

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

    char    szBuffer[INI_MAX_LINE+1];
	char	szEqual[2];
	char 	szPropertySep[2];
	char 	*pProperty;
	char	*pValue;
	char	*pValueLastChar;

	szEqual[0]			= cEqual;
	szEqual[1]			= '\0';
    szPropertySep[0]	= cPropertySep;
	szPropertySep[1]	= '\0';

    strcpy( pszValue, "" );
	strncpy( szBuffer, pszString, INI_MAX_LINE );

	/* find pszProperty		*/
	while ( 1 )
	{
		pProperty = (char *)strtok( szBuffer, (const char *)szPropertySep );
		if ( pProperty == NULL )
			break;
		else
		{
			/* extract pszValue 		*/
			if ( strncmp( pProperty, pszProperty, strlen(pszProperty) ) == 0 )
			{
				pValue = (char *)strtok( szBuffer, (const char *)szEqual );
				if ( pValue )
				{
					/* truncate any other data */
					pValueLastChar = (char *)strchr( pValue, (int)szPropertySep );
					if ( pValueLastChar )
						pValueLastChar[0] = '\0';

					strncpy( pszValue, pValue, INI_MAX_PROPERTY_VALUE );
					iniAllTrim( pszValue );
				}
				break;
			}
		}
	}

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniPropertyValue */


