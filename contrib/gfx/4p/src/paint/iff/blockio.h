/*
 * blockio.h
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

#ifndef __BLOCKIO_H__
#define __BLOCKIO_H__

#if defined (__APPLE__) && defined (__MACH__)
  #include "endian.h"
#else
  #include "iff/endian.h"
#endif

// the header for a block of IFF data
typedef struct __block_header {
    //char name[5];
    int name;
    int size;
} BLOCKHEADER;


////////////////////////////////////////////////////////////////////////////////
// read in the IFF header starting at the current position in the file.

void iff_read_header(FILE * fp, BLOCKHEADER * head);
void iff_skip_block(FILE * fp, BLOCKHEADER * bh);

#endif
