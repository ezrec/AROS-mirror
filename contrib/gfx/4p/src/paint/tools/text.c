/*
 * tools
 * 
 *   callback mechanism and handlers for drawing tools
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
#include <string.h>
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "cursors.h"


////////////////////////////////////////////////////////////////////////////////
// text

// this one is temporary... it sucks... i know.
void
tools_text_handler (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    char text[512];
    int hh = current_font->h>>1;

    switch (mouse_event)
    {
	case (TOOL_EVENT_INIT):
//	    CURSOR_IBEAM();
	    printf("text\n");


	case (TOOL_EVENT_IDLE):
	    Page_Negative(page_rubberband);
	    primitive_rectangle_hollow(page_rubberband, page_active->fgpen,
			primitive_pixel, x, y-hh, 
			current_font->w+x, current_font->h+y-hh );
	    break;

	case (TOOL_EVENT_DEINIT):
	    Page_Negative(page_rubberband);
	    break;

	case (TOOL_EVENT_STARTDOWN):
	    Page_Undo_Checkpoint(page_active);

	    text[0] = '\0';
	    if( do_fontreq( actual_screen, text, 512 ) )
	    {
		font_render_text(page_active, page_active->fgpen, 
			page_active->bgpen,
			current_font, x, y-hh, text);
	    }
	default:
	    break;
    }
}
