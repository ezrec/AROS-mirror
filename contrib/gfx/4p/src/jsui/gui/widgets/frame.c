/*
 * frame
 * 
 */

/*
 * JSUI - Jerry's SDL User Interface
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

#include <string.h>
#include <ctype.h>
#include "jsui.h"

#undef USECLOSEBUTTON

int jsui_widget_frame
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    // down on the corner, out in the street
    // willy and the poor boys are playing
    // bring a nickel, tap your feet.

    switch(msg)
    {
	case (JSUI_MSG_START):
	    if (d->d1 & JSUI_FRAME_CENTER)
		jsui_dialog_center(d);

	    if (d->d1 & JSUI_FRAME_WEST)
		jdr->x = 0;
	    else if (d->d1 & JSUI_FRAME_EAST)
		jdr->x = jsui_screen->w - jdr->buffer->w - 1;

	    if (d->d1 & JSUI_FRAME_NORTH)
		jdr->y = 15; // in case there's a menu on screen
	    else if (d->d1 & JSUI_FRAME_SOUTH)
		jdr->y = jsui_screen->h - jdr->buffer->h - 1;

	    //jsui_dialog_position(d, jdr->x, jdr->y);
	    jdr->flags |= JSUI_F_DIRTY;
	    d->d1 = d->d2 = -1;
	    break;

	case (JSUI_MSG_LPRESS):
	    if (jdr->mx >=
#ifdef USECLOSEBUTTON
                12
#else
                0
#endif
                )
	    {
		d->d1 = jdr->mx;
		d->d2 = jdr->my;
	    }
	    break;

	case (JSUI_MSG_LDRAG):
	    //if (jdr->my <= 12)
	    if (d->d2 <= 12) // this helps.
	    {
		if (d->d1 != -1)
                {
                    if (jsui_active_background_page)
                    {
                        Page_DR_Dirtify(jsui_active_background_page,
                                        jdr->x,
                                        jdr->y, 
                                        jdr->x + jdr->buffer->w,
                                        jdr->y + jdr->buffer->h);
                    }
		    jsui_dialog_position(d,
			    jdr->x - (d->d1 - jdr->mx),
			    jdr->y - (d->d2 - jdr->my) );
                }
	    }
	    break;

	case (JSUI_MSG_LRELEASE):
#ifdef USECLOSEBUTTON
	    if ( (d->d1 == -1) && (jdr->mx < 12) && (jdr->my <= 12))
	    {
		printf ("close frame\n");
	    } else 
#endif
            if (!(d->flags & JSUI_F_ZLOCK)) {
		//jsui_dialog_raise(d);
		printf ("raise frame\n");
	    }
	    d->d1 = d->d2 = -1;

	    break;

	case (JSUI_MSG_GOTFOCUS):
	    //printf("Frame Got Focus\n");
	    jsui_dialog_send_message(d, JSUI_MSG_DRAW, 0);
	    break;

	case (JSUI_MSG_LOSTFOCUS):
	    //printf("Frame Lost Focus\n");
	    jsui_dialog_send_message(d, JSUI_MSG_DRAW, 0);
	    break;

	case (JSUI_MSG_DRAW):
	    newpen = Pen_Create();
	    newpen->Icolor = JSUI_C_LOLITE;

	    // first the drag bar
	    // textbar
	    primitive_rectangle_fourcolor(jdr->buffer, __primitive_pixel,
#ifdef USECLOSEBUTTON
		    12,
#else
                    0,
#endif
                    0, jdr->buffer->w-1, 11, //-13, 11,
		    (d->flags & JSUI_F_GOTFOCUS)?JSUI_C_DB:JSUI_C_DBIDLE,
		    JSUI_C_BRIGHT, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_BRIGHT);

	    font_render_text_center(jdr->buffer, newpen, newpen, 
			    jsui_current_font,
			    jdr->buffer->w/2, 2, d->dp);

#ifdef USECLOSEBUTTON
	    // close button
	    primitive_rectangle_fourcolor(jdr->buffer, primitive_pixel,
		    0,0, 11, 11,
		    (d->flags & JSUI_F_GOTFOCUS)?JSUI_C_CB:JSUI_C_CBIDLE,
		    JSUI_C_BRIGHT, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_BRIGHT);

	    primitive_line(jdr->buffer, newpen, __primitive_pixel, 1,1, 10,10);
	    primitive_line(jdr->buffer, newpen, __primitive_pixel, 1,10, 10,1);
#endif

#ifdef NEVER
	    // position button
	    primitive_rectangle_fourcolor(jdr->buffer, __primitive_pixel,
		    jdr->buffer->w-12, 0, jdr->buffer->w-1, 11, 
		    (d->flags & JSUI_F_GOTFOCUS)?JSUI_C_CB:JSUI_C_CBIDLE,
		    JSUI_C_BRIGHT, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_BRIGHT);

	    primitive_rectangle_hollow(jdr->buffer, newpen, __primitive_pixel,
		    jdr->buffer->w-12 +2, 2, 
		    jdr->buffer->w-1  -4, 7);

	    primitive_rectangle_filled(jdr->buffer, newpen, 
		    jdr->buffer->w-12 +4, 4, 
		    jdr->buffer->w-1  -2, 9);
#endif

	    Pen_Destroy(newpen);
	
	    // then the body

	    primitive_rectangle_fourcolor(jdr->buffer, __primitive_pixel,
		    0,12, jdr->buffer->w-1, jdr->buffer->h-1,
		    JSUI_C_BG,
		    JSUI_C_BRIGHT, JSUI_C_DARK, JSUI_C_DARK, JSUI_C_BRIGHT);

            Page_DR_Dirtify(jdr->buffer, 0, 0, jdr->buffer->w-1, jdr->buffer->h-1);

	    if(jdr)
		jdr->flags |= JSUI_F_DIRTY;
    }

    return JSUI_R_O_K;
}
