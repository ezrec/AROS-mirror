/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/

#define AROS_TAGRETURNTYPE Object *
#include <utility/tagitem.h>
#include <proto/alib.h>
#include <exec/libraries.h>

/*****************************************************************************

    NAME */
#include <intuition/classusr.h>
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/bgui.h>
extern struct Library * BGUIBase;

	Object * BGUI_NewObject (

/*  SYNOPSIS */
	ULONG num,
	Tag tag1, 
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

    retval = BGUI_NewObjectA(num, AROS_SLOWSTACKTAGS_ARG(tag1));

    AROS_SLOWSTACKTAGS_POST
} /* BGUI_NewObjectA */
