/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#define AROS_TAGRETURNTYPE void
#define AROS_TAGRETURNTYPEVOID

#include <render/render.h>

/*****************************************************************************

    NAME */
#define NO_INLINE_STDARG /* turn off inline def */
#include <proto/render.h>

extern struct Library * RenderBase;

	void CreatePenTable (

/*  SYNOPSIS */
	UBYTE *src,
	APTR srcpal,
    	UWORD width,
	UWORD height,
	APTR newpal,
	UBYTE *pentab,
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

    CreatePenTableA(src, srcpal, width, height, newpal, pentab, AROS_SLOWSTACKTAGS_ARG(tag1));

    AROS_SLOWSTACKTAGS_POST
    
} /* CreatePenTable */
