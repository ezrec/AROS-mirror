/**********************************************************************************
 * iniObjectInsert
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
	AROS_LH2(int, iniObjectInsert,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszObject, A0),

/*  LOCATION */
	struct Library *, inifileBase, 23, inifile)

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
    
	HINIOBJECT	hObject;
	char		szObjectName[INI_MAX_OBJECT_NAME+1];

    /* SANITY CHECK */
    if ( hIni == NULL )
        return INI_ERROR;
    if ( pszObject == NULL )
        return INI_ERROR;

	strncpy( szObjectName, pszObject, INI_MAX_OBJECT_NAME );
	iniAllTrim( szObjectName );

	/* CREATE OBJECT STRUCT */
	hObject = malloc( sizeof(INIOBJECT) );
	hIni->hCurProperty			= NULL;
	hObject->hFirstProperty		= NULL;
	hObject->hLastProperty		= NULL;
	hObject->nProperties		= 0;
	hObject->pNext				= NULL;
	hObject->pPrev				= NULL;
	strncpy( hObject->szName, szObjectName, INI_MAX_OBJECT_NAME );

	/* APPEND TO OBJECT LIST */
	if ( hIni->hFirstObject == NULL )
		hIni->hFirstObject = hObject;
	
	hObject->pPrev				= hIni->hLastObject;
	hIni->hLastObject			= hObject;

	if ( hObject->pPrev != NULL )
        hObject->pPrev->pNext	= hObject;
	
	hIni->hCurObject = hObject;
	hIni->nObjects++;

    return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniObjectInsert */

