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
	AROS_LH1(int, iniPropertyDelete,

/*  SYNOPSIS */
	AROS_LHA(HINI, hIni, D0),
	
/*  LOCATION */
	struct Library *, inifileBase, 31, inifile)

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
    
	HINIPROPERTY	hProperty;
	HINIOBJECT		hObject;

    /* SANITY CHECKS */
    if ( hIni == NULL )
        return INI_ERROR;
	if ( hIni->hCurObject == NULL )
		return INI_ERROR;
	if ( hIni->hCurProperty == NULL )
		return INI_NO_DATA;
	
	hObject		= hIni->hCurObject;
	hProperty	= hIni->hCurProperty;

	/* REMOVE FROM LIST */
	if ( hObject->hFirstProperty == hProperty )
		hObject->hFirstProperty = hProperty->pNext;
	if ( hObject->hLastProperty == hProperty )
		hObject->hLastProperty = hProperty->pPrev;

	hIni->hCurProperty		= NULL;
	if ( hProperty->pNext )
	{
		hProperty->pNext->pPrev = hProperty->pPrev;
		hIni->hCurProperty		= hProperty->pNext;
	}
	if ( hProperty->pPrev )
	{
		hProperty->pPrev->pNext = hProperty->pNext;
		hIni->hCurProperty		= hProperty->pPrev;
	}
	hObject->nProperties--;

	/* FREE MEMORY */
	free( hProperty );

	return INI_SUCCESS;

    AROS_LIBFUNC_EXIT
} /* iniPropertyDelete */
