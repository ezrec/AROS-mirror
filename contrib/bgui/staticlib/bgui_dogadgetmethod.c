/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
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
	ULONG MethodID, 
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
    AROS_SLOWSTACKTAGS_PRE(tag1)

    retval = BGUI_DoGadgetMethodA(obj, win, req, AROS_SLOWSTACKMETHODS_ARG(MethodID));

    AROS_SLOWSTACKTAGS_POST
} /* BGUI_DoGadgetMethod */
