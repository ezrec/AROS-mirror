/*
** cursors
**
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
#include <string.h>
#include <ctype.h>

#include "tool_cb.h"
#include "tools.h"
#include "jsui.h"
#include "SDL.h"
#include "cursors.h"

/*
 * The cursor is created in black and white according to the following:
 * data  mask    resulting pixel on screen
 *  0     1       White
 *  1     1       Black
 *  0     0       Transparent
 *  1     0       Inverted color if possible, black if not.
 */

////////////////////////////////////////////////////////////////////////////////

Uint8 crosshair_cursor_data[] = {
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa8, 0x2a,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
};

Uint8 crosshair_cursor_mask[] = {
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0xfc, 0x7e,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
};

SDL_Cursor * crosshairs;


////////////////////////////////////////////////////////////////////////////////

/*  eyedropper
   					data	mask
 *    - # # -  - - - -  - - - -  - - -  00 00	60 00
 *    # # # #  - - - -  - - - -  - - -  00 00 	f0 00
 *    # # # #  # - - -  - - - -  - - -  00 00	f8 00
 *    - # # #  # # - -  - - - -  - - -  00 00   7c 00

 *    - - # #  # # # -  - - - -  - - - 	00 00	3e 00
 *    - - - #  # # # #  # - - -  - - -  00 00	1f 80
 *    - - - -  # # # -  - # - -  - - -  01 80	0f c0
 *    - - - -  - # - -  - - # -  - - -  03 c0	07 e0

 *    - - - -  - # - -  - - - #  - - -  03 e0	07 f0
 *    - - - -  - - # -  - - - -  # - -  01 f0	03 f8
 *    - - - -  - - - #  - - - -  - # -  00 f8	01 fc
 *    - - - -  - - - -  # - - -  - - #  00 7c	00 fe

 *    - - - -  - - - -  - # - -  - - #  00 3c   00 7e
 *    - - - -  - - - -  - - # -  - - #  00 1c   00 3e
 *    - - - -  - - - -  - - - #  # # -  00 00   00 1c
 */


Uint8 eyedropper_cursor_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xc0,
    0x03, 0xe0, 0x01, 0xf0, 0x00, 0xf8, 0x00, 0x7c,
    0x00, 0x3c, 0x00, 0x18, 0x00, 0x00
};                                     

Uint8 eyedropper_cursor_mask[] = {
    0x60, 0x00, 0xf0, 0x00, 0xf8, 0x00, 0x7c, 0x00,
    0x3e, 0x00, 0x1f, 0x80, 0x0f, 0xc0, 0x07, 0xe0,
    0x07, 0xf0, 0x03, 0xf8, 0x01, 0xfc, 0x00, 0xfe,
    0x00, 0x7e, 0x00, 0x3e, 0x00, 0x1c
};                                     

SDL_Cursor * eyedropper;


////////////////////////////////////////////////////////////////////////////////

/*  ibeam
   			data	mask
 *    # # # -  # # # -  00 	ee
 *    # - - #  - - # -  6c 	fe
 *    # - - -  - - # -  7c 	fe
 *    # # - -  - # # -  38 	fe

 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38

 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38
 *    - - # -  # - - -  10 	38

 *    # # - -  - # # -  38 	fe
 *    # - - -  - - # -  7c 	fe
 *    # - - #  - - # -  6c 	fe
 *    # # # -  # # # -  00 	ee

 */


Uint8 ibeam_cursor_data[] = {
    0x00, 0x6c, 0x7c, 0x38, 
    0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10,
    0x38, 0x7c, 0x6c, 0x00, 
};                                     

Uint8 ibeam_cursor_mask[] = {
    0xee, 0xfe, 0xfe, 0xfe,
    0x38, 0x38, 0x38, 0x38,
    0x38, 0x38, 0x38, 0x38,
    0xfe, 0xfe, 0xfe, 0xee,
};                                     

SDL_Cursor * ibeam;

////////////////////////////////////////////////////////////////////////////////

/*  arrow
   					data	mask
 *    - # # #  # # - -  - - - -  - - -  00 00	7c 00
 *    # - - -  - - # -  - - - -  - - -  7c 00 	fe 00
 *    # - - -  - - # -  - - - -  - - -  7c 00	fe 00
 *    # - - -  - # - -  - - - -  - - -  78 00   fc 00

 *    # - - -  - - # -  - - - -  - - - 	7c 00	fe 00
 *    # - - #  - - - #  - - - -  - - -  6e 00	ff 00
 *    - # # -  # - - -  # - - -  - - -  07 00	6f 80
 *    - - - -  - # - -  - # - -  - - -  03 80	07 c0

 *    - - - -  - - # -  - - # -  - - -  01 c0	03 e0
 *    - - - -  - - - #  - # - -  - - -  00 80	01 c0
 *    - - - -  - - - -  # - - -  - - -  00 00	00 80
 *    - - - -  - - - -  - - - -  - - -  00 00	00 00

 */


Uint8 arrow_cursor_data[] = {
    0x00, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x78, 0x00,
    0x7c, 0x00, 0x6e, 0x00, 0x07, 0x00, 0x03, 0x80,
    0x01, 0xc0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
};                                     

Uint8 arrow_cursor_mask[] = {
    0x7c, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfc, 0x00,
    0xfe, 0x00, 0xff, 0x00, 0x6f, 0x80, 0x07, 0xc0,
    0x03, 0xe0, 0x01, 0xc0, 0x00, 0x80, 0x00, 0x00,
};                                     

SDL_Cursor * arrow;

////////////////////////////////////////////////////////////////////////////////
/*  paintcan
   					data	mask
 *    - - - -  - - # -  - - - -  - - -  00 00	02 00
 *    - - - -  - # x #  - - - -  - - -  02 00	07 00
 *    - - - -  # x x x  # - - -  - - -  07 00	0f 80
 *    - - - #  x x x x  x # - -  - - -  0f 80	1f c0

 *    - - # x  x x x x  x x # -  - - -  1f c0	3f e0
 *    - # x x  x x x x  x x x #  - - -  3f e0	7f f0
 *    # x x x  x x x x  x x x #  # - -  7f e0	ff f8
 *    - # x x  x x x x  x x # #  - - -  3f c0	7f f0

 *    - - # x  x x x x  x # # -  - - -  1f 80	3f e0
 *    - - - #  x x x x  # # - -  - - -  0f 00	1f c0
 *    - - - -  # x x #  # - - -  - - -  06 00	0f 80
 *    - - - -  - # # #  - - - -  - - -  00 00	07 00

 *    - - - -  - # x #  - - - -  - - -  02 00	07 00
 *    - - - #  - # x #  - # - -  - - -  02 00	17 40
 *    - - - -  # x x x  # - - -  - - -  07 00	0f 80
 *    - - - -  - # x #  - - - -  - - -  02 00	07 00

 *    - - - x  x x - x  x x - -  - - -  1d c0	1d c0
 */


Uint8 paintcan_cursor_data[] = {
    0x00, 0x00, 0x02, 0x00, 0x07, 0x00, 0x0f, 0x80,
    0x1f, 0xc0, 0x3f, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0,
    0x1f, 0x80, 0x0f, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x07, 0x00, 0x02, 0x00,
    0x1d, 0xc0,
};                                     

Uint8 paintcan_cursor_mask[] = {
    0x02, 0x00, 0x07, 0x00, 0x0f, 0x80, 0x1f, 0xc0,
    0x3f, 0xe0, 0x7f, 0xf0, 0xff, 0xf8, 0x7f, 0xf0,
    0x3f, 0xe0, 0x1f, 0xc0, 0x0f, 0x80, 0x07, 0x00,
    0x07, 0x00, 0x17, 0x40, 0x0f, 0x80, 0x07, 0x00,
    0x1d, 0xc0,
};                                     

SDL_Cursor * paintcan;


////////////////////////////////////////////////////////////////////////////////

void 
cursor_init(
    void
)
{
    crosshairs = SDL_CreateCursor(
			crosshair_cursor_data, crosshair_cursor_mask,
			16, 15, 7, 7);

    eyedropper = SDL_CreateCursor(
			eyedropper_cursor_data, eyedropper_cursor_mask,
			16, 15, 0, 0);

    arrow = SDL_CreateCursor(
			arrow_cursor_data, arrow_cursor_mask,
			16, 12, 0, 0);

    ibeam = SDL_CreateCursor( 
			ibeam_cursor_data, ibeam_cursor_mask,
			8, 16, 4, 7);

    paintcan = SDL_CreateCursor( 
			paintcan_cursor_data, paintcan_cursor_mask,
			13, 17, 6, 17);

    CURSOR_CROSSHAIRS();
}


void 
cursor_deinit(
    void
)
{
    SDL_FreeCursor(crosshairs);
    SDL_FreeCursor(eyedropper);
    SDL_FreeCursor(arrow);
    SDL_FreeCursor(ibeam);
    SDL_FreeCursor(paintcan);
}
