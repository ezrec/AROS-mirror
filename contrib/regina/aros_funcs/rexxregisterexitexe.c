/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function RexxRegisterExitExe()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */
#ifdef RX_STRONGTYPING
#define RexxExitHanlderPtr RexxExitHandler *
#else
#define RexxExitHandlerPtr PFN
#endif
	AROS_LH3(APIRET, RexxRegisterExitExe,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(RexxExitHandlerPtr, EntryPoint, A1),
	AROS_LHA(PUCHAR, UserArea, A2),

/*  LOCATION */
	struct Library *, ReginaBase, 11, Regina)

/*  FUNCTION
        RexxRegisterExitExe function from Rexx SAA library

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

    return RexxRegisterExitExe(EnvName, EntryPoint, UserArea);

    AROS_LIBFUNC_EXIT
}
