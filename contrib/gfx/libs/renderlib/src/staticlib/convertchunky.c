/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#define AROS_TAGRETURNTYPE ULONG

#include <render/render.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/render.h>

extern struct Library * RenderBase;

	ULONG ConvertChunky (

/*  SYNOPSIS */
	UBYTE *src,
	APTR srcpal,
	UWORD width,
	UWORD height,
	UBYTE *dst,
	APTR dstpal,
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

    retval = ConvertChunkyA(src, srcpal, width, height, dst, dstpal, AROS_SLOWSTACKTAGS_ARG(tag1));

    AROS_SLOWSTACKTAGS_POST
    
} /* ConvertChunky */
