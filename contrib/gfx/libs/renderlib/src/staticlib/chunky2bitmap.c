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

	void Chunky2BitMap (

/*  SYNOPSIS */
	UBYTE *src,
	UWORD sx,
	UWORD sy,
	UWORD width,
	UWORD height,
	struct BitMap *bm,
	UWORD dx,
	UWORD dy,
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

    Chunky2BitMapA(src, sx, sy, width, height, bm, dx, dy, AROS_SLOWSTACKTAGS_ARG(tag1));

    AROS_SLOWSTACKTAGS_POST
    
} /* Chunky2BitMap */
