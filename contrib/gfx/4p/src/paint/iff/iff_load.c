/*
 * iff_load
 *
 *  read in Electronic Arts Interchange File Format ILBM images.
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


////////////////////////////////////////////////////////////////////////////////

// decode a single plane from the file.

int iff_decoded[8];

void
iff_decode_byte(
	unsigned char c,
	int set
)
{
    iff_decoded[0] = (c & 0x80)?set:0;
    iff_decoded[1] = (c & 0x40)?set:0;
    iff_decoded[2] = (c & 0x20)?set:0;
    iff_decoded[3] = (c & 0x10)?set:0;
    iff_decoded[4] = (c & 0x08)?set:0;
    iff_decoded[5] = (c & 0x04)?set:0;
    iff_decoded[6] = (c & 0x02)?set:0;
    iff_decoded[7] = (c & 0x01)?set:0;
}

void
iff_load_in_plane(
    	FILE * fp,
	int  * imagebuffer,
	int    row,
	int    width,
	int    plane
)
{
}

void
iff_decode_plane(
    	FILE * fp,
	int  * imagebuffer,
	int    row,
	int    width,
	int    plane
)
{
    int x = 0;
    int xb;
    int rn;
    char n;
    int d;

    while (x < width)
    {
	rn = getc(fp);

	if (rn==EOF){
	    return;
	}
	n = (char)rn&0x00ff;

	if (n>=0) 
	{
	    // copy the next n+1 bytes literally
	    while (n>=0)
	    {
		d = iff_read_8(fp);
		iff_decode_byte(d, 1<<plane);

		for (xb=0 ; xb<8 && x < width ; x++, xb++)
		{
		    imagebuffer[(row*width)+x] |= iff_decoded[xb];
		}
		
		n--;
	    }
	}
	else if (n >= -127 && n <= -1)
	{
	    // repeat the next byte n+1 times
	    d = iff_read_8(fp);
	    iff_decode_byte(d, 1<<plane);

	    while (n<1)
	    {
		for (xb=0 ; xb<8 && x < width ; x++, xb++)
		{
		    imagebuffer[(row*width)+x] |= iff_decoded[xb];
		}

		n++;
	    }
	}
	else if (n == -128)
	{
	    // skip this byte
	}
    }
}


// load in an image from the filename, and store it in IPAGE. 
// if there were any errors, return a NULL
IPAGE *
iff_file_load_page(
	char * filename
)
{
    int x,y;
    int plane;
    int handle_x = 0;
    int handle_y = 1;
    BLOCKHEADER bh;
    IFF_BMHD bmhd;
    FILE * fp;
    COLEL * palette = NULL;
    IPAGE * iff_page = NULL;

    fp = fopen(filename, "r");

    // skip the container header...
    (void)iff_read_32(fp);
    (void)iff_read_32(fp);
    (void)iff_read_32(fp);

    bmhd.width=0;
    bmhd.height=0;
    // parse the file...
    iff_read_header(fp, &bh);
    while (bh.size)
    {
	switch (bh.name)
	{
	case(IFF_BLOCK_BMHD):
	    //printf("Bitmap header\n");

	    bmhd.width   = iff_read_16(fp) & 0x00ffff;
	    bmhd.height  = iff_read_16(fp) & 0x00ffff;
	    bmhd.x       = iff_read_16(fp) & 0x00ffff;
	    bmhd.y       = iff_read_16(fp) & 0x00ffff;
	    bmhd.planes  = iff_read_8(fp)  & 0x0000ff;
	    bmhd.masking = iff_read_8(fp)  & 0x0000ff;
	    bmhd.comp    = iff_read_8(fp)  & 0x0000ff;
	    (void)iff_read_8(fp);  // pad.  not used.  -- store 0
	    bmhd.xparent = iff_read_16(fp) & 0x00ffff;
	    bmhd.xaspect = iff_read_8(fp)  & 0x0000ff;
	    bmhd.yaspect = iff_read_8(fp)  & 0x0000ff;
	    bmhd.pwidth  = iff_read_16(fp) & 0x00ffff;
	    bmhd.pheight = iff_read_16(fp) & 0x00ffff;

	    /*
	    printf("image is %d by %d  (%d %d)  (%d %d)\n",
			bmhd.width, bmhd.height,
			bmhd.pwidth, bmhd.pheight,
			bmhd.xaspect, bmhd.yaspect);
	    printf("%d planes %d %s\n",
			bmhd.planes, bmhd.masking, 
			(bmhd.comp==1)?"RLE":"?");
	    */
	    break;

	case(IFF_BLOCK_GRAB):
	    handle_x = iff_read_16(fp);
	    handle_y = iff_read_16(fp);
	    break;

	case(IFF_BLOCK_CMAP):
	    palette = (COLEL *) malloc(bh.size*sizeof(COLEL));
	    for (x=0 ; x<bh.size/3 ; x++)
	    {
		palette[x].r = iff_read_8(fp)&0x00ff;
		palette[x].g = iff_read_8(fp)&0x00ff;
		palette[x].b = iff_read_8(fp)&0x00ff;
	    }
	    break;

	case(IFF_BLOCK_CRNG):
	    // Electronic Arts Deluxe Paint color range
	    (void)iff_read_16(fp); // pad -- store 0
	    (void)iff_read_16(fp); // rate
	    (void)iff_read_16(fp); // active -- 1 means cycle them
	    (void)iff_read_8(fp);  // start range
	    (void)iff_read_8(fp);  // end range
	    break;

	case(IFF_BLOCK_CCRT):
	    // Commodore Graphicraft color range
	    (void)iff_read_16(fp); // direction  0=stop 1=forw -1=back
	    (void)iff_read_8(fp);  // start range
	    (void)iff_read_8(fp);  // end range
	    (void)iff_read_32(fp); // seconds
	    (void)iff_read_32(fp); // microseconds
	    (void)iff_read_16(fp); // pad -- store 0
	    break;

	case(IFF_BLOCK_BODY):
	    //printf("Image Body %d (%d colors)\n", bh.size, 1<<bmhd.planes);

	    iff_page = Page_Create(bmhd.width, bmhd.height, 1<<bmhd.planes);

	    iff_page->handle_x = handle_x;
	    iff_page->handle_y = handle_y;

	    // at this point, we should have a palette, but just in case,
	    // we'll put an if around this anyway...
	    if (palette)
	    {
		free(iff_page->palette);
		iff_page->palette = palette;
	    }

	    // now load in the bitmap data...

	    for (y=0 ; y < iff_page->h ; y++)
	    {
		// decode image data

		for ( plane = 0 ; plane < bmhd.planes ; plane++)
		{
		    if (1) //bmhd.comp==1)
		    {
			iff_decode_plane(fp, iff_page->pimage, y, 
						iff_page->w, plane);
		    } else {
			iff_load_in_plane(fp, iff_page->pimage, y, 
						iff_page->w, plane);
		    }
		}

		// and the mask...
		if (bmhd.masking == 1)
		{
		    iff_decode_plane(fp, iff_page->alpha, y, iff_page->w, 1);
		}
	    }

	    return iff_page;
	    break;

	default:
	    iff_skip_block(fp, &bh);
	}


	iff_read_header(fp, &bh);
    }

    fclose(fp);
    return iff_page;
}
