/**********************************************************************************
 * iniOpen
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * PAH = Peter Harvey		- pharvey@codebydesign.com
 * -----------------------------------------------
 *
 * PAH	06.MAR.99	Can now create file-less INI. Pass NULL for
 *					pszFileName. Then copy a file name into hIni->szFileName
 *					before calling iniCommit.
 **************************************************/

#include "inifile_intern.h"
#include "ini.h"
#include <aros/libcall.h>

/*****************************************************************************
    NAME */
	AROS_LH7(int, iniOpen,

/*  SYNOPSIS */
	AROS_LHA(HINI *, hIni, A0),
	AROS_LHA(char, *pszFileName, A1),
    AROS_LHA(char, cComment, D0),
	AROS_LHA(char, cLeftBracket, D1),
	AROS_LHA(char, cRightBracket, D2),
	AROS_LHA(char, cEqual, D3),
	AROS_LHA(int, bCreate, D4),
	
/*  LOCATION */
	struct Library *, inifileBase, 29, inifile)

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
    char    szObjectName[INI_MAX_OBJECT_NAME+1];
    char    szPropertyName[INI_MAX_PROPERTY_NAME+1];
    char    szPropertyValue[INI_MAX_PROPERTY_VALUE+1];
    int     nValidFile;

    /* INIT STATEMENT */
    *hIni = malloc( sizeof(INI) );
    if ( pszFileName )
        strncpy( (*hIni)->szFileName, pszFileName, ODBC_FILENAME_MAX );
    else
        strncpy( (*hIni)->szFileName, "", ODBC_FILENAME_MAX );
    (*hIni)->cComment           = cComment;
    (*hIni)->cLeftBracket       = cLeftBracket;
    (*hIni)->cRightBracket      = cRightBracket;
    (*hIni)->cEqual             = cEqual;
    (*hIni)->bChanged           = FALSE;
    (*hIni)->hCurObject         = NULL;
    (*hIni)->hFirstObject       = NULL;
    (*hIni)->hLastObject        = NULL;
    (*hIni)->nObjects           = 0;
    (*hIni)->bReadOnly          = 0;

    /* OPEN FILE */
    if ( pszFileName )
    {
        hFile = fopen( pszFileName, "r" );
        if ( !hFile )
        {
            if ( bCreate == TRUE )
            {
                hFile = fopen( pszFileName, "w" );
            }
        }

        if ( !hFile )
        {
            free( *hIni );
            *hIni = NULL;
            return INI_ERROR;
        }

        nValidFile = _iniScanUntilObject( *hIni, hFile, szLine );
        if ( nValidFile == INI_SUCCESS )
        {
            char *ptr;
            do
            {
                if ( szLine[0] == cLeftBracket )
                {
                    _iniObjectRead( (*hIni), szLine, szObjectName );
                    iniObjectInsert( (*hIni), szObjectName );
                }
                else if ( (szLine[0] != cComment) && !isspace(szLine[0]) )
                {
                    _iniPropertyRead( (*hIni), szLine, szPropertyName, szPropertyValue );
                    iniPropertyInsert( (*hIni), szPropertyName, szPropertyValue );
                }

            } while ( (ptr = fgets( szLine, INI_MAX_LINE, hFile )) != NULL );
        }
        else if ( nValidFile == INI_ERROR )
        {
            /* INVALID FILE */
            if ( hFile != NULL )
                fclose( hFile );
            free( *hIni );
            *hIni = NULL;
            return INI_ERROR;
        }

        /* CLEANUP */
        if ( hFile != NULL )
            fclose( hFile );

        iniObjectFirst( *hIni );

    } /* if file given */

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniOpen */

