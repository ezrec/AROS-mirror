/**********************************************************************************
 * iniAppend
 *
 * - Appends Sections which do not exist in hIni. Ignores all else.
 * - Does not try to append 'missing' Entries  to existing Sections (does not try to merge).
 * - hIni will become ReadOnly
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
	AROS_LH2(int, iniAppend,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszFileName, A0),

/*  LOCATION */
	struct Library *, inifileBase, 9, inifile)

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
	char    szLine[INI_MAX_LINE+1];
	char	szObjectName[INI_MAX_OBJECT_NAME+1];
	char	szPropertyName[INI_MAX_PROPERTY_NAME+1];
	char	szPropertyValue[INI_MAX_PROPERTY_VALUE+1];

    /* SANITY CHECK */
    if ( strlen( pszFileName ) > ODBC_FILENAME_MAX )
        return INI_ERROR;

    /* OPEN FILE */
    hFile = fopen( pszFileName, "r" );
	if ( !hFile )
		return INI_ERROR;

	iniObjectLast( hIni );
	iniPropertyLast( hIni );

	/* SCAN UNTIL WE GET TO AN OBJECT NAME OR EOF */
	szLine[0] = '\0';
	if ( _iniScanUntilObject( hIni, hFile, szLine ) == INI_SUCCESS )
	{
		do
		{
			if ( szLine[0] == hIni->cLeftBracket )
			{
				_iniObjectRead( hIni, szLine, szObjectName );
				if ( iniObjectSeek( hIni, szObjectName ) == INI_SUCCESS )
				{
					iniObjectLast( hIni );
					iniPropertyLast( hIni );
					if ( _iniScanUntilNextObject( hIni, hFile, szLine ) != INI_SUCCESS)
						break;
				}
				else
				{
					iniObjectInsert( hIni, szObjectName );
					if ( fgets( szLine, INI_MAX_LINE, hFile ) == NULL )
						break;
				}
			}
			else if ( (szLine[0] != hIni->cComment) && isalnum(szLine[0]) )
			{
				_iniPropertyRead( hIni, szLine, szPropertyName, szPropertyValue );
				iniPropertyInsert( hIni, szPropertyName, szPropertyValue );
				if ( fgets( szLine, INI_MAX_LINE, hFile ) == NULL )
					break;
			}
			else
			{
				if ( fgets( szLine, INI_MAX_LINE, hFile ) == NULL )
					break;
			}
		} while( 1 );
	}

	/* WE ARE NOT GOING TO TRY TO BE SMART ENOUGH TO SAVE THIS STUFF */
	hIni->bReadOnly			= 1;

    /* CLEANUP */
    if ( hFile != NULL )
		fclose( hFile );

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniAppend */


