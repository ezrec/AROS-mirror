/*
 * iff_save
 *
 *  write out Electronic Arts Interchange File Format ILBM images.
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

/*

    write out the basic header FORM----ILBM
    write out image header BMHD
    write out the palette CMAP
    write out the color ranges CRNG and CCRT 
    write out the handle GRAB

    malloc a scanline's worth of memory
	(w * 2)

    for each scanline
        for each plane
	    pull out the pixels to a buffer
	    
	check the buffer for repetitions
	    output the appropriate characters.

    go back and fill in the appropriate sizes in the FORM and BODY
 */


// save out the image 'page' to the filename
void
iff_file_save_page(
	IPAGE * page,
	char * filename
)
{
    if (!page || !filename) return;
    if (!page->pimage) return;

}
