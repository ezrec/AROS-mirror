/*
** testdlgs.c
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

#include "jsui.h"



char temp[512] = "Hello";

JSUI_DIALOG test_text_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
   { jsui_widget_frame, 0, 0, 175, 255, 0, JSUI_FRAME_WEST, 0, 0, "Text Window", NULL, NULL  },

    {jsui_widget_text, 5, 5, 165, 20, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, 0, "Left", NULL, NULL},
    {jsui_widget_text, 5, 30, 165, 20, 0, JSUI_F_NOFRAME, JSUI_T_CENTER, 0, "Center", NULL, NULL},
    {jsui_widget_text, 5, 55, 165, 20, 0, JSUI_F_NOFRAME, JSUI_T_RIGHT, 0, "Right", NULL, NULL},

    {jsui_widget_text, 5, 80, 165, 20, 0, 0, JSUI_T_LEFT, 0, "Left", NULL, NULL},
    {jsui_widget_text, 5, 105, 165, 20, 0, 0, JSUI_T_CENTER, 0, "Center", NULL, NULL},
    {jsui_widget_text, 5, 130, 165, 20, 0, 0, JSUI_T_RIGHT, 0, "Right", NULL, NULL},

    {jsui_widget_edit, 5, 155, 165, 20, 0, 0, 100, 0, temp, NULL, NULL},
    {jsui_widget_edit, 5, 180, 165, 20, 0, 0, 10, 2, temp, NULL, NULL},
    {jsui_widget_edit, 5, 205, 165, 20, 0, 0, 5, 4, temp, NULL, NULL},

    JSUI_DLG_END
};

/*  Use this for the text tool */
JSUI_DIALOG text_tool_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    { jsui_widget_frame,  0, 0, 300, 55, 0, 0, 0, 0, "Text", NULL, NULL  },
    { jsui_widget_edit,   5, 5, 290, 20, 0, 0, 100, 0, temp, NULL, NULL },
    { jsui_widget_button, 215, 30, 80, 20, 's', 0, 0, 1, "Accept", "Accept", NULL},
    JSUI_DLG_END
};

/*  Use this for the file tool */
JSUI_DIALOG text_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    { jsui_widget_frame,  0, 0, 300, 80, 0, JSUI_FRAME_SOUTH, 0, 0, "File...", NULL, NULL  },
    { jsui_widget_text,   5, 5, 40, 20, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, 0, "Path:", NULL, NULL},
    { jsui_widget_edit,   50, 5, 240, 20, 0, 0, 100, 0, temp, NULL, NULL },
    { jsui_widget_text,   5, 30, 40, 20, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, 0, "File:", NULL, NULL},
    { jsui_widget_edit,   50, 30, 240, 20, 0, 0, 100, 0, temp, NULL, NULL },
    { jsui_widget_button, 215, 55, 80, 20, 's', 0, 0, 1, "Accept", "Accept", NULL},
    JSUI_DLG_END
};

int slid_cb(void * dp3, int value);



JSUI_DIALOG test_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
   { jsui_widget_frame, 0, 0, 175, 255, 0, 0, 0, 0, "Test Window", NULL, NULL  },

    {jsui_widget_button,  5, 5, 80, 20, 's', 0, JSUI_B_RADIO, 1, "1 off", "1 ON", NULL},
    {jsui_widget_button,  90, 5, 80, 20, 's', 0, JSUI_B_ON, 0, "2 off", "2 ON", NULL},
    {jsui_widget_button,  5, 30, 80, 20, 's', 0, JSUI_B_RADIO, 1, "3 off", "3 ON", NULL},
    {jsui_widget_button,  90, 30, 80, 20, 's', 0, JSUI_B_STICKY|JSUI_B_ON, 0, "4 off", "4 ON", NULL},

    {jsui_widget_slider, 5, 55, 165, 20, 0, JSUI_F_NOFRAME, 100, 50, (void *)slid_cb, NULL, NULL},


    {jsui_widget_progress, 5, 80, 40, 170, 's', JSUI_F_FLIP, 100, 25, 
		//&test_dialog[5].d2, NULL, NULL},
		NULL, NULL, NULL},

    {jsui_widget_slider,  50, 80, 40, 170, 's', 0, 100, 5, NULL, NULL, NULL},

    {jsui_widget_slider,  95, 80, 40, 170, 's', 0, 100, 3, NULL, NULL, NULL},


    {jsui_widget_button,  140, 80, 30, 30, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "9", NULL, NULL},

    {jsui_widget_button,  140, 115, 30, 30, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "10", NULL, NULL},
    {jsui_widget_button,  140, 150, 30, 30, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "11", NULL, NULL},
    {jsui_widget_button,  140, 185, 30, 30, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "12", NULL, NULL},
    {jsui_widget_button,  140, 220, 30, 30, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "13", NULL, NULL},

    JSUI_DLG_END
};


int 
slid_cb(void * dp3, int value)
{
    test_dialog[6].d2 = value;
    jsui_dialog_tell_message(test_dialog, 6, JSUI_MSG_DRAW);

    return JSUI_R_O_K;
}
