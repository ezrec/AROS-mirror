/*
** palette.c
**
**  1999 Dec 29 - 2000 Jan ??
**
**  code skeleton borrowed from the SDL demo code
**  bits of this will be parted out as needed.
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


void cdcb(int msg, JSUI_DIALOG * d)
{
    if (msg == JSUI_MSG_LPRESS)
	jsui_dialog_broadcast(PAINT_MSG_EYEDROPPER, 0);
    else 
	jsui_dialog_broadcast(PAINT_MSG_PALETTEEDIT, 0);
}

void cpcb(int msg, JSUI_DIALOG * d, int color)
{
    if (!page_active) return;

    if (msg == JSUI_MSG_LPRESS)
	jsui_dialog_broadcast(PAINT_MSG_FGPALETTESET, color);
    else 
	jsui_dialog_broadcast(PAINT_MSG_BGPALETTESET, color);
}

extern JSUI_DIALOG colors_dialog[];

int paint_dlg_msg_handler(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    if (msg == PAINT_MSG_FGPALETTESET)
    {
	colors_dialog[1].d1 = c;
	jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, &colors_dialog[1], 0);
	colors_dialog[2].d2 = c;
	jsui_widget_colorpicker(JSUI_MSG_DRAW, vjdr, &colors_dialog[2], 0);
    }

    if (msg == PAINT_MSG_BGPALETTESET)
    {
	colors_dialog[1].d2 = c;
	jsui_widget_colordisplay(JSUI_MSG_DRAW, vjdr, &colors_dialog[1], 0);
    }
    return( JSUI_R_O_K );
}

JSUI_DIALOG colors_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */

    { jsui_widget_frame, 0, 0, 51, 95, 0, 0,
		    JSUI_FRAME_SOUTH | JSUI_FRAME_EAST,
		    0, "", NULL, NULL  },
    { jsui_widget_colordisplay, 2,  1, 48, 17, 0, 0, 1, 0, (void *)cdcb },
    { jsui_widget_colorpicker,  2, 19, 48, 74, 0, 0, 32, 1, (void *)cpcb, NULL},
    { paint_dlg_msg_handler },
    JSUI_DLG_END
};




int pe_cb( JSUI_DIALOG * d, int object, int msg )
{
    return( JSUI_R_CLOSE );
}

JSUI_DIALOG palette_edit_dialog[] =
{
    /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    { jsui_widget_frame, 0, 0, 300, 300, 0, 0, 
		    JSUI_FRAME_CENTER, 0,
		     "Color Palette", NULL, NULL  },

    { jsui_widget_text,  6, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "R"},
    { jsui_widget_text, 36, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "G"},
    { jsui_widget_text, 66, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "B"},

    { jsui_widget_text, 106, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "H"},
    { jsui_widget_text, 136, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "S"},
    { jsui_widget_text, 166, 10, 33, 0, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "V"},

    { jsui_widget_slider, 5, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },
    { jsui_widget_slider, 35, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },
    { jsui_widget_slider, 65, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },

    { jsui_widget_slider, 105, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },
    { jsui_widget_slider, 135, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },
    { jsui_widget_slider, 165, 15, 33, 160, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },

    { jsui_widget_colorpicker,  215, 25,  (4*17)+2, (8*17)+2, 0, 0, 
		32, 1, NULL, NULL},

    { jsui_widget_button, 40, 180, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Spread", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-150-10, 180, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Ex", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-85, 180, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Copy", NULL, (void *)pe_cb },

    { jsui_widget_button, 5, 210, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Range", NULL, (void *)pe_cb },

    { jsui_widget_text, 75, 210, 45, 18, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, ":"},

    { jsui_widget_button, 300-180, 210, 40, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "C1", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-135, 210, 40, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "C2", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-90, 210, 40, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "C3", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-45, 210, 40, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "C4", NULL, (void *)pe_cb },

/*
              Color Palette
 ---------------------------------------
 
    R  G  B   H  S  V   [   ]
    ^  ^  ^   ^  ^  ^   +--------+
    |  |  |   |  |  |
    *  *  *   |  *  |
    |  |  |   *  |  |
    |  |  |   |  |  *
    |  |  |   |  |  |
    v  v  v   v  v  v   +--------+
 
      [ SPREAD ]    [ EX ] [ COPY ]

  [ RANGE ] : [ C1 ][ C2 ][ C3 ][ C4 ]
    Speed  <------[]---------->  [ ^ ]

  [ CANCEL ]   [  UNDO  ]   [   OK   ]

*/

    { jsui_widget_text, 5, 235, 70, 18, 0, JSUI_F_NOFRAME | JSUI_F_NOFOCUS,
			JSUI_T_CENTER, 0, "Speed"},

    { jsui_widget_slider, 80, 235, 180, 18, 0, 
		JSUI_F_NOFRAME, 255, 127, NULL },

    { jsui_widget_button, 300-35, 235, 30, 18, 0, 0,
                JSUI_B_STICKY, 2,
                "^", "v", (void *)pe_cb },


    { jsui_widget_button, 5, 300-20-5, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Cancel", NULL, (void *)pe_cb },

    { jsui_widget_button, 300/2 - 70/2, 300-20-5, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Undo", NULL, (void *)pe_cb },

    { jsui_widget_button, 300-70-5, 300-20-5, 70, 18, 0, 0,
                JSUI_B_STICKY|JSUI_B_RADIO, 2,
                "Okay", NULL, (void *)pe_cb },

    JSUI_DLG_END
};
