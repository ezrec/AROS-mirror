/*
 * endian
 *
 *  read and write routines for amiga files
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


#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//    tools for reading the file.  
// since the amiga files are stored as little endian, and we're not sure
// what the endinanness of the local machine is, these will read in the values
// from the file, and convert it to local endian.


// read an 8 bit char/uchar
char iff_read_8 (FILE * fp);

// read a 16 bit word/uword
int iff_read_16(FILE * fp);

// read a 32bit long/ulong
long iff_read_32(FILE * fp);

#endif
