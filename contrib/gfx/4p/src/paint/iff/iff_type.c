/*
 * iff_type
 *
 *  read in and write out Electronic Arts Interchange File Format 
 *  ILBM images.
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

/*
 * rewrite to be like this:
 *	int file_is_iff( char * )
 *	int iff_seek( long block )  (if it returns nonzero, it found the block)
 *	void iff_read_row_plane(int * image, int offset, int bit)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsui.h"
#if defined (__APPLE__) && defined (__MACH__)
  #include "endian.h"
  #include "blockio.h"
  #include "iffheadr.h"
#else
  #include "iff/endian.h"
  #include "iff/blockio.h"
  #include "iff/iffheadr.h"
#endif

int
file_is_iff(
	char * filename
)
{
    long a, b, c;
    FILE * iff_file = fopen(filename, "r");
    if (iff_file == NULL) return 0;

    a = iff_read_32(iff_file);
    (void)iff_read_32(iff_file);
    b = iff_read_32(iff_file);
    c = iff_read_32(iff_file);

    fclose(iff_file);

    if (a==IFF_BLOCK_FORM)
    {
	if ( (b == IFF_BLOCK_ILBM) && (c == IFF_BLOCK_BMHD) )
	{
	    return 1;
	}
    }

    return 0;
}
