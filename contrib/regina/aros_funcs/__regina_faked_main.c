/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Regina function __regina_faked_main()
    Lang: English
*/

#include "regina_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH2(int, __regina_faked_main,

/*  SYNOPSIS */
	AROS_LHA(int, argc, D0),
	AROS_LHA(char **, argv, A0),

/*  LOCATION */
	struct Library *, ReginaBase, 35, Regina)

/*  FUNCTION
        __regina_faked_main for regina.library

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

    return __regina_faked_main(argc, argv);

    AROS_LIBFUNC_EXIT
}
