/*
** toolbar.c
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
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

#include "jsui.h"
#include "paint.h"
#include "tool_cb.h"

/*
#define JSUI_B_ON            0x0001
*/

extern JSUI_DIALOG brushes_dialog[];

int tool_dlg_msg_handler(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{ 
    int index = 0;

    switch( msg )
    {
    case( PAINT_MSG_CIRCLEBRUSHSIZE ):
	index = c + 2;
	if (index > 5) index = 12;
	jsui_widget_imagebutton(JSUI_MSG_TRIGGER_L, vjdr, 
				&brushes_dialog[index], 0);
	break;

    case( PAINT_MSG_SQUAREBRUSHSIZE ):
	index = c;
	if (c > 5) index = 12;
	else 
	    index = (7 + (4 - c));
	jsui_widget_imagebutton(JSUI_MSG_TRIGGER_L, vjdr, 
				&brushes_dialog[index], 0);
	break;

    case( PAINT_MSG_BRUSH3 ):
	if (!c)
	jsui_widget_imagebutton(JSUI_MSG_TRIGGER_L, vjdr, 
				&brushes_dialog[10], 1);
printf(" >>> scatter 3 %d\n", c);
	break;

    case( PAINT_MSG_BRUSH5 ):
	if (!c)
	jsui_widget_imagebutton(JSUI_MSG_TRIGGER_L, vjdr, 
				&brushes_dialog[11], 1);
printf(" >>> scatter 5\n");
	break;
    }
    return( JSUI_R_O_K );
}


int
bbcb( JSUI_DIALOG * d, int id, int msg )
{
    if (!d) return( JSUI_R_O_K );

    if (!(d->d1 & JSUI_B_ON)) return( JSUI_R_O_K );
    switch(id)
    {
    case(2): jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0); break;
    case(3): jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 1); break;
    case(4): jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 2); break;
    case(5): jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 3); break;

    case(6): jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, 5); break;
    case(7): jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, 4); break;
    case(8): jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, 3); break;
    case(9): jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, 2); break;

    case(10): jsui_dialog_broadcast(PAINT_MSG_BRUSH3, 1); break;
    case(11): jsui_dialog_broadcast(PAINT_MSG_BRUSH5, 1); break;
    }
    return( JSUI_R_O_K );
}

extern IPAGE brushes_bitmap;

////////////////////////////////////////
// first row, 1,3,5,7 pixel circles
IPAGE_XREF br_1pc[3] = {
    { &brushes_bitmap,   1, 2, 3, 7, 1 },
    { &brushes_bitmap,  26, 2, 3, 7, 1 },
    { &brushes_bitmap,  26, 2, 3, 7, 1 },
};

IPAGE_XREF br_3pc[3] = {
    { &brushes_bitmap,  4, 2, 5, 7, 1 },
    { &brushes_bitmap, 29, 2, 5, 7, 1 },
    { &brushes_bitmap, 29, 2, 5, 7, 1 },
};

IPAGE_XREF br_5pc[3] = {
    { &brushes_bitmap,  9, 2, 7, 7, 1 },
    { &brushes_bitmap, 34, 2, 7, 7, 1 },
    { &brushes_bitmap, 34, 2, 7, 7, 1 },
};

IPAGE_XREF br_7pc[3] = {
    { &brushes_bitmap, 16, 2, 7, 7, 1 },
    { &brushes_bitmap, 41, 2, 7, 7, 1 },
    { &brushes_bitmap, 41, 2, 7, 7, 1 },
};

////////////////////////////////////////
// second row, 5,4,3,2 pixel squares

IPAGE_XREF br_5ps[3] = {
    { &brushes_bitmap,  1, 9, 7, 7, 1 },
    { &brushes_bitmap, 26, 9, 7, 7, 1 },
    { &brushes_bitmap, 26, 9, 7, 7, 1 },
};

IPAGE_XREF br_4ps[3] = {
    { &brushes_bitmap,  8, 9, 6, 6, 1 },
    { &brushes_bitmap, 33, 9, 6, 6, 1 },
    { &brushes_bitmap, 33, 9, 6, 6, 1 },
};

IPAGE_XREF br_3ps[3] = {
    { &brushes_bitmap, 14, 9, 5, 5, 1 },
    { &brushes_bitmap, 39, 9, 5, 5, 1 },
    { &brushes_bitmap, 39, 9, 5, 5, 1 },
};

IPAGE_XREF br_2ps[3] = {
    { &brushes_bitmap, 19, 9, 4, 4, 1 },
    { &brushes_bitmap, 44, 9, 4, 4, 1 },
    { &brushes_bitmap, 44, 9, 4, 4, 1 },
};

////////////////////////////////////////
// third row, 3 and 5 point scatterbrushes

IPAGE_XREF br_3s[3] = {
    { &brushes_bitmap,  4, 16, 5, 5, 1 },
    { &brushes_bitmap, 29, 16, 5, 5, 1 },
    { &brushes_bitmap, 29, 16, 5, 5, 1 },
};

IPAGE_XREF br_5s[3] = {
    { &brushes_bitmap, 14, 14, 9, 7, 1 },
    { &brushes_bitmap, 39, 14, 9, 7, 1 },
    { &brushes_bitmap, 39, 14, 9, 7, 1 },
};


JSUI_DIALOG brushes_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)
                (key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    { jsui_widget_frame, 0, 0, 51, 46, 0, 0, 
		JSUI_FRAME_NORTH | JSUI_FRAME_EAST, 0, "", NULL, NULL  },

    // needs to be 'nofocus' so that it doesn't snag focus... oops
    {jsui_widget_3Dbox, 1, 1, 49, 44, 0, JSUI_F_NOFOCUS, 0, JSUI_3_RAISED },

    {jsui_widget_imagebutton, 2, 3, 6, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME,
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_1pc, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 8, 3, 10, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_3pc, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 18, 3, 14, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_5pc, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 32, 3, 14, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_7pc, NULL, (void *) bbcb},

    ////////////
    {jsui_widget_imagebutton, 2, 17, 14, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_5ps, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 16, 17, 12, 12, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_4ps, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 28, 17, 10, 10, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_3ps, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 38, 17, 8, 8, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_2ps, NULL, (void *) bbcb},

    /////////
    {jsui_widget_imagebutton, 8, 31, 10, 10, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_3s, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 28, 27, 18, 14, 0,
			JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME, 
			JSUI_B_RADIO | JSUI_B_STICKY,
			2, (void *) br_5s, NULL, (void *) bbcb},

    {jsui_widget_imagebutton, 0,0,0,0, 0,
		JSUI_F_NOFOCUSBOX | JSUI_F_NOFRAME,
		JSUI_B_RADIO, 2},

    {tool_dlg_msg_handler},
    JSUI_DLG_END
};
