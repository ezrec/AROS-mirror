/*
 * fill
 * 
 *   floodfill callback handlers
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
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "cursors.h"


////////////////////////////////////////////////////////////////////////////////
// floodfill


void
tools_floodfill_handler (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    int current_pixel;

    switch (mouse_event)
    {
	case (TOOL_EVENT_INIT):
	    printf("flood fill\n");
	    Page_Negative(page_rubberband);
	    CURSOR_PAINTCAN();
	    break;

	case (TOOL_EVENT_STARTDOWN):

	    current_pixel = primitive_pixel_get(page_active, x, y);

	    if (mouse_buttons == 1)
	    {
		if (current_pixel != page_active->fgpen->Icolor) 
		{
		    Page_Undo_Checkpoint(page_active);
		    primitive_floodfill( page_active, page_active->fgpen, x, y);
		}
	    }
	    else if (mouse_buttons == 3)
	    {
		if (current_pixel != page_active->bgpen->Icolor) 
		{
		    Page_Undo_Checkpoint(page_active);
		    primitive_floodfill( page_active, page_active->bgpen, x, y);
		}
	    }
	    break;

	case (TOOL_EVENT_IDLE):
            Page_Negative(page_rubberband);
	    primitive_pixel(page_rubberband, page_active->fgpen, x, y);

	    break;

	case (TOOL_EVENT_DEINIT):
	case (TOOL_EVENT_DRAGGING):
	case (TOOL_EVENT_DRAG_STAT):
	case (TOOL_EVENT_RELEASE):
	case (TOOL_EVENT_NONE):
	default:
	    break;

    }
}
