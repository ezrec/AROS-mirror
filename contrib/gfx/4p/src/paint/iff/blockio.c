/*
 * iffheadr
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

#if defined (__APPLE__) && defined (__MACH__)
  #include "endian.h"
  #include "blockio.h"
#else
  #include "iff/endian.h"
  #include "iff/blockio.h"
#endif

////////////////////////////////////////////////////////////////////////////////

// read in the IFF header starting at the current position in the file.
void
iff_read_header(
	FILE * fp,
	BLOCKHEADER * head
)
{
/*
    int c = fgetc(fp);
    if (c == EOF)
    {
	printf ("EOF\n");
	head->name[0] = head->name[1] = 
	head->name[2] = head->name[3] = 
	head->name[4] = '\0';
	head->size = 0;
	return;
    }
    ungetc (c, fp);

    head->name[0] = iff_read_8(fp);
    head->name[1] = iff_read_8(fp);
    head->name[2] = iff_read_8(fp);
    head->name[3] = iff_read_8(fp);
    head->name[4] = '\0';
*/

    head->name = iff_read_32(fp);
    head->size = iff_read_32(fp);
}

void
iff_skip_block(
	FILE * fp,
	BLOCKHEADER * bh
)
{
    fseek(fp, bh->size, SEEK_CUR);
}

