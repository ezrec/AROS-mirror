/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function DummyFunction()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

#ifdef RX_STRONGTYPING
#define RexxSubcomHandlerPtr RexxRegisterSubcomHandler *
#else
#define RexxSubcomHandlerPtr PFN
#endif
	AROS_LH3(APIRET, RexxRegisterSubcomExe,

/*  SYNOPSIS */
	AROS_LHA(PCSZ, EnvName, A0),
	AROS_LHA(RexxSubcomHandlerPtr, EntryPoint, A1),
	AROS_LHA(PUCHAR, UserArea, A2),

/*  LOCATION */
	struct Library *, ReginaBase, 6, Regina)

/*  FUNCTION
        RexxStart function from Rexx SAA library

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

    return RexxRegisterSubcomExe(EnvName, EntryPoint, UserArea);

    AROS_LIBFUNC_EXIT

} /* DummyFunction */
