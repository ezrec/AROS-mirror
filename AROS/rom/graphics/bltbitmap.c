/*
    (C) 1995 AROS - The Amiga Replacement OS
    $Id$

    Desc: Copy a rectangle in a bitmap to another place or another bitmap.
    Lang: english
*/
#include <string.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <proto/exec.h>
#include "graphics_intern.h"

static void copyonepixel (PLANEPTR src, ULONG xsrc, PLANEPTR dest,
	ULONG xdest, ULONG minterm);

/*****************************************************************************

    NAME */
#include <graphics/gfx.h>
#include <proto/graphics.h>

	AROS_LH11(LONG, BltBitMap,

/*  SYNOPSIS */
	AROS_LHA(struct BitMap *, srcBitMap, A0),
	AROS_LHA(LONG           , xSrc, D0),
	AROS_LHA(LONG           , ySrc, D1),
	AROS_LHA(struct BitMap *, destBitMap, A1),
	AROS_LHA(LONG           , xDest, D2),
	AROS_LHA(LONG           , yDest, D3),
	AROS_LHA(LONG           , xSize, D4),
	AROS_LHA(LONG           , ySize, D5),
	AROS_LHA(ULONG          , minterm, D6),
	AROS_LHA(ULONG          , mask, D7),
	AROS_LHA(PLANEPTR       , tempA, A2),

/*  LOCATION */
	struct GfxBase *, GfxBase, 5, Graphics)

/*  FUNCTION
	Moves a part of a bitmap around or into another bitmaps.

    INPUTS
	srcBitMap - Copy from this bitmap.
	xSrc, ySrc - This is the upper left corner of the area to copy.
	destBitMap - Copy to this bitmap. May be the same as srcBitMap.
	xDest, yDest - Upper left corner where to place the copy
	xSize, ySize - The size of the area to copy
	minterm - How to copy. Most useful values are 0x00C0 for a vanilla
		copy, 0x0030 to invert the source and then copy or 0x0050
		to ignore the source and just invert the destination. If
		you want to calculate other values, then you must know that
		channel A is set, if you are inside the rectangle, channel
		B is the source and channels C and C are the destination of
		the rectangle.
	mask - Which planes should be copied. Typically, you should set
		this to ~0L.
	tempA - If the copy overlaps exactly to the left or right (i.e. the
		scan line addresses overlap), and tempA is non-zero, it
		points to enough chip accessible memory to hold a line of a
		source for the blit (ie. CHIP RAM). BltBitMap will allocate
		(and free) the needed TempA if none is provided and one is
		needed.  Blit overlap is determined from the relation of
		the first non-masked planes in the source and destination
		bit maps.

    RESULT
	The number of planes actually involved in the blit.

    NOTES
	If a special hardware is available, this function will use it.

    EXAMPLE

    BUGS

    SEE ALSO
	ClipBlit(), BltBitMapRastPort()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GfxBase *,GfxBase)
    LONG planecnt;

    if (srcBitMap->Pad != 0 || destBitMap->Pad != 0)
    {
	planecnt = driver_BltBitMap (srcBitMap, xSrc, ySrc
	    , destBitMap, xDest, yDest
	    , xSize, ySize
	    , minterm, mask, tempA
	    , GfxBase
	);
    }
    else
    {
	ULONG wSrc, wDest;
	ULONG x, y, plane;
	ULONG depth;
	PLANEPTR src, dest, temp;

	wSrc  =  srcBitMap->BytesPerRow * 8;
	wDest = destBitMap->BytesPerRow * 8;
	temp = NULL;

	/* Clip X and Y */
	if (xSrc < 0)
	{
	    xDest += -xSrc;
	    xSize -= -xSrc;
	    xSrc = 0;
	}

	if (ySrc < 0)
	{
	    yDest += -ySrc;
	    ySize -= -ySrc;
	    ySrc = 0;
	}

	/* Clip width and height for source and dest */
	if (ySrc + ySize >= srcBitMap->Rows)
	    ySize = srcBitMap->Rows - ySrc;

	if (yDest + ySize >= destBitMap->Rows)
	    ySize = destBitMap->Rows - yDest;

	if (xSrc + xSize >= wSrc)
	    xSize = wSrc - xSrc;

	if (xDest + xSize >= wDest)
	    xSize = wDest - xDest;

	/* If the size is illegal or we need not copy anything, return */
	if (ySize <= 0 || xSize <= 0 || !mask || !minterm)
	    return 0;

	planecnt = 0;

	/* For all planes */
	for (plane=0; plane<depth; plane ++)
	{
	    /* Copy this plane ? */
	    if (1L << plane & mask)
	    {
		planecnt ++; /* count it */

		for (y=0; y<ySize; y++)
		{
		    src  =  srcBitMap->Planes[plane] + y* srcBitMap->BytesPerRow;
		    dest = destBitMap->Planes[plane] + y*destBitMap->BytesPerRow;

		    if ((src <= dest && src+srcBitMap->BytesPerRow > dest)
			|| (dest <= src && dest+destBitMap->BytesPerRow > src)
		    )
		    {
			if (!temp)
			{
			    if (tempA)
				temp = tempA;
			    else
				temp = AllocMem (srcBitMap->BytesPerRow, MEMF_ANY);

			    if (!temp)
				return 0;
			}

			memmove (temp, src, srcBitMap->BytesPerRow);

			for (x=0; x<xSize; x++)
			    copyonepixel (temp, x+xSrc, dest, x+xDest, minterm);
		    }
		    else
		    {
			for (x=0; x<xSize; x++)
			    copyonepixel (src, x+xSrc, dest, x+xDest, minterm);
		    }
		}
	    }
	}

	if (temp && !tempA)
	    FreeMem (temp, srcBitMap->BytesPerRow);
    }

    return planecnt;
    AROS_LIBFUNC_EXIT
} /* BltBitMap */

static void copyonepixel (PLANEPTR src, ULONG xsrc, PLANEPTR dest, ULONG xdest,
	ULONG minterm)
{
    ULONG sByte, sBit, sSet;
    ULONG dByte, dBit, dSet;

    sByte = xsrc / 8;
    sBit = 1L << (xsrc & 7L);
    sSet = (src[sByte] & sBit) != 0;

    dByte = xdest / 8;
    dBit = 1L << (xsrc & 7L);
    dSet = (dest[dByte] & dBit) != 0;

    switch (minterm)
    {
    case 0x00C0:
	dest[dByte] = (dest[dByte] & ~dBit)
		| (sSet ? dBit : 0);
	break;

    case 0x0030:
	dest[dByte] = (dest[dByte] & ~dBit)
		| (!sSet ? dBit : 0);
	break;

    case 0x0050:
	dest[dByte] ^= dBit;
	break;

    }
}

