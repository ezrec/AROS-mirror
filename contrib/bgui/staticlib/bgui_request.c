/*
    Copyright © 1995-2000, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/

#include <utility/tagitem.h>
#include <proto/alib.h>
#include <stdarg.h>

struct bguiRequest;
struct Window;

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/bgui.h>
extern struct Library * BGUIBase;

	ULONG BGUI_Request (

/*  SYNOPSIS */
	struct Window * win,
	struct bguiRequest * estruct,
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
    va_list args;
    ULONG    rc;

    va_start (args, estruct);

    rc = BGUI_RequestA (win, estruct, (APTR)args);

    va_end (args);

    return rc;

} /* BGUI_Request */
