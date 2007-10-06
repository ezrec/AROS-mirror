/*
    Copyright © 1995-2000, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#define AROS_TAGRETURNTYPE ULONG
#include <utility/tagitem.h>
#include <proto/alib.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/bgui.h>
extern struct Library * BGUIBase;

	ULONG BGUI_DoGadgetMethod (

/*  SYNOPSIS */
	Object * obj,
	struct Window * win,
	struct Requester * req,
	IPTR MethodID, 
	...)

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
    AROS_SLOWSTACKMETHODS_PRE(MethodID)

    retval = BGUI_DoGadgetMethodA(obj, win, req, AROS_SLOWSTACKMETHODS_ARG(MethodID));

    AROS_SLOWSTACKMETHODS_POST
} /* BGUI_DoGadgetMethod */
