/*
 * palette
 * 
 *   Palette manipulations tools
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "jsui.h"

// Deluxe Paint 1   32 color palette
int __pal32_1[96] = 
{
	// base colors
      0,   0,   0,  240, 240, 240,  224,   0,   0,  160,   0,   0,
    208, 128,   0,  240, 224,   0,  128, 240,   0,    0, 128,   0,

	// greens and blues
      0, 196,  96,    0, 208, 208,    0, 160, 240,    0, 112, 192,
      0,   0, 240,  112,   0, 240,  192,   0, 224,  198,   0, 128,

	// mouse colors
     96,  32,   0,  224,  80,  32,  160,  80,  32,  240, 192, 160,

	// grays
     48,  48,  48,   64,  64,  64,   80,  80,  80,   96,  96,  96,
    112, 112, 112,  128, 128, 128,  144, 144, 144,  160, 160, 160,
    176, 176, 176,  192, 192, 192,  208, 208, 208,  224, 224, 224,
};


// Deluxe Paint 2   32 color palette
int __pal32_2[96] = 
{
	// base colors
      0,   0,   0,  224, 192, 160,  224,   0,   0,  160,   0,   0,
    208, 128,   0,  240, 224,   0,  128, 240,   0,    0, 128,   0,

	// greens and blues
      0, 196,  96,    0, 208, 208,    0, 160, 240,    0, 112, 192,
      0,   0, 240,  112,   0, 240,  192,   0, 224,  198,   0, 128,

	// mouse colors
     96,  32,   0,  224,  80,  32,  160,  80,  32,  240, 192, 160,

	// grays
     48,  48,  48,   64,  64,  64,   80,  80,  80,   96,  96,  96,
    112, 112, 112,  128, 128, 128,  144, 144, 144,  160, 160, 160,
    192, 192, 192,  208, 208, 208,  224, 224, 224,  240, 240, 240,
};


COLEL * 
palette_generate_default(
	int ncolors
)
{
    int col;
    COLEL * dpal = NULL;

    if (ABS(ncolors) != 32)  return NULL;

    dpal = (COLEL *)malloc(ABS(ncolors) * 3 * sizeof(COLEL));

    if (!dpal) return NULL;

    if (ncolors > 0)
    {
	for (col=0 ; col<ncolors ; col++)
	{
	    dpal[col].r = __pal32_1[col*3+0];
	    dpal[col].g = __pal32_1[col*3+1];
	    dpal[col].b = __pal32_1[col*3+2];
	}
    } else {
	for (col=0 ; col<ncolors*3 ; col++)
	{
	    dpal[col].r = __pal32_2[col*3+0];
	    dpal[col].g = __pal32_2[col*3+1];
	    dpal[col].b = __pal32_2[col*3+2];
	}

    }

    return dpal;
}


void
palette_free(
    COLEL * palette
)
{
    if (palette)
	free(palette);
}
