/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxRegisterFunctionExe()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

#ifdef RX_STRONGTYPING
#define RexxFunctionHandlerPtr RexxFunctionHandler *
#else
#define RexxFunctionHandlerPtr PFN
#endif
	AROS_LH2(APIRET, RexxRegisterFunctionExe,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, name, A0),
	AROS_LHA(RexxFunctionHandlerPtr, EntryPoint, A1),

/*  LOCATION */
	struct Library *, ReginaBase, 15, Regina)

/*  FUNCTION
        RexxRegisterFunctionExe function from Rexx SAA library

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,ReginaBase)

    return RexxRegisterFunctionExe(name, EntryPoint);

    AROS_LIBFUNC_EXIT
}
