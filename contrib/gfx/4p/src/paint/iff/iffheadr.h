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

#ifndef __IFFHEADR_H__
#define __IFFHEADR_H__


#define IFF_BLOCK_FORM	0x464F524D
#define IFF_BLOCK_ILBM  0x494C424D
#define IFF_BLOCK_BMHD	0x424D4844
#define IFF_BLOCK_CCRT	0x43435254
#define IFF_BLOCK_CMAP	0x434D4150
#define IFF_BLOCK_CRNG	0x43524E47
#define IFF_BLOCK_GRAB	0x47524142
#define IFF_BLOCK_BODY	0x424F4459


////////////////////////////////////////////////////////////////////////////////
//  headers and such.

// the header for an ILBM.  
typedef struct __iff_BMHD {
    int width;
    int height;
    int x;
    int y;
    int planes;
    int masking;
    int comp;
    int xparent;
    int xaspect;
    int yaspect;
    int pwidth;
    int pheight;
} IFF_BMHD;

#endif
