/**********************************************************************************
 * iniPropertyInsert
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
	AROS_LH3(int, iniPropertyInsert,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	AROS_LHA(char *, pszProperty, A0),
    AROS_LHA(char *, pszValue, A1),

/*  LOCATION */
	struct Library *, inifileBase, 34, inifile)

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
    
	HINIOBJECT		hObject;
	HINIPROPERTY	hProperty;

    /* SANITY CHECKS */
	if ( hIni == NULL )
		return INI_ERROR;
    if ( hIni->hCurObject == NULL )
        return INI_ERROR;
	if ( pszProperty == NULL )
        return INI_ERROR;

	hObject	= hIni->hCurObject;

	/* CREATE PROPERTY STRUCT */
	hProperty = (HINIPROPERTY)malloc( sizeof(INIPROPERTY) );
	strncpy( hProperty->szName, pszProperty, INI_MAX_PROPERTY_NAME );
	strncpy( hProperty->szValue, pszValue, INI_MAX_PROPERTY_VALUE );
	hProperty->pNext = NULL;
    iniAllTrim( hProperty->szName );
    iniAllTrim( hProperty->szValue );

	/* APPEND TO LIST */
	if ( hObject->hFirstProperty == NULL )
		hObject->hFirstProperty = hProperty;

    hProperty->pPrev		= hObject->hLastProperty;
    hObject->hLastProperty	= hProperty;

	if ( hProperty->pPrev != NULL )
        hProperty->pPrev->pNext	= hProperty;

	hIni->hCurProperty = hProperty;
	hObject->nProperties++;

	return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniPropertyInsert */
