/*
** font/text requestor
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

#include "jsui.h"

int fontreq_value = -1;
char * fontreq_buffer = NULL;

int fontreq_cb(JSUI_DIALOG * d, int object, int msg)
{
    if (!d) return( JSUI_R_O_K );

    if(msg != JSUI_MSG_LRELEASE) return( JSUI_R_O_K );

    switch(object)
    {
    case(2):
	fontreq_value = 0;
	return( JSUI_R_CLOSE );
    case(3):
	fontreq_value = 1;
	return( JSUI_R_CLOSE );
    }
    return JSUI_R_O_K;
}

JSUI_DIALOG fontreq_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */

    {jsui_widget_frame, 0, 0, 300, 75, 0, 
            JSUI_FRAME_DRAGGABLE | JSUI_FRAME_TITLEBAR | JSUI_FRAME_CLOSE, 
	    JSUI_FRAME_CENTER,
            0, "Text...", NULL, NULL  },

    {jsui_widget_edit, 10, 15, 280, 20, 0, 0, 0, 0, 
		NULL, NULL, NULL},
                
    /* 2 */
    {jsui_widget_button,  5, 50, 70, 18, 0, 0, 0, 2,
		"Cancel", NULL, (void *)fontreq_cb},

    {jsui_widget_button,  225, 50, 70, 18, 0, 0, 0, 2,
		"Okay", NULL, (void *)fontreq_cb},

    JSUI_DLG_END
};

int
do_fontreq(
        SDL_Surface * screen,
	char * buffer,
	int buffersize
)
{
    fontreq_dialog[0].x = 0;
    fontreq_dialog[0].y = 0;
    fontreq_dialog[0].d1 = JSUI_FRAME_CENTER;
    fontreq_dialog[1].dp = buffer;
    fontreq_dialog[1].d1 = buffersize;
    fontreq_dialog[1].d2 = 0;
    fontreq_buffer = buffer;
    fontreq_value = -1;
    
    jsui_dialog_run_modal( fontreq_dialog );

    while ( jsui_is_running( fontreq_dialog ) ) 
    {
	SDL_Delay(10);
	jsui_poll(screen);
    }
    
    return( fontreq_value );
}
