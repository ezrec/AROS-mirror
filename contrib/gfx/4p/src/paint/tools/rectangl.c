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
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "brushes.h"
#include "cursors.h"


////////////////////////////////////////////////////////////////////////////////
// rectangles

void
tools_rectangle_handler (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    static int startx;
    static int starty;
    int real_mode = drawing_mode;

    switch (mouse_event)
    {
	case (TOOL_EVENT_INIT):
	    CURSOR_CROSSHAIRS();
	    Page_Negative(page_rubberband);
	    printf("rectangle\n");
            startx = x;
            starty = y;
	    break;

	case (TOOL_EVENT_DEINIT):
	    Page_Negative(page_rubberband);
	    break;

	case (TOOL_EVENT_STARTDOWN):
        //printf("Start %d %d\n", x, y);
	    Page_Undo_Checkpoint(page_active);
	    startx = x;
	    starty = y;
	    break;

	case (TOOL_EVENT_DRAGGING):
	case (TOOL_EVENT_DRAG_STAT):
            //printf("Drag %d %d\n", x, y);

	    Page_Negative(page_rubberband);
	    if (mouse_buttons == 1)
	    {
		primitive_rectangle_hollow(page_rubberband, pen_cycle,
				primitive_pixel,
				//(brush_active)?prim_brush:primitive_pixel,
				startx, starty, x, y);

	    }
	    else if (mouse_buttons == 3)
	    {
		drawing_mode = DRAW_STYLE_BGCOLOR;
		primitive_rectangle_hollow(page_rubberband, pen_cycle,
				primitive_pixel,
				//(brush_active)?prim_brush:primitive_pixel,
				startx, starty, x, y);
	    }
	    break;

	case (TOOL_EVENT_RELEASE):
        //printf("Release %d %d\n", x, y);
	    Page_Negative(page_rubberband);
	    if (mouse_buttons == 1)
	    {
		primitive_rectangle_hollow(page_active, page_active->fgpen,
			(brush_active)?prim_brush:primitive_pixel, 
			startx, starty, x, y);

	    }
	    else if (mouse_buttons == 3)
	    {
		drawing_mode = DRAW_STYLE_BGCOLOR;
		primitive_rectangle_hollow(page_active, page_active->bgpen,
			(brush_active)?prim_brush:primitive_pixel, 
			startx, starty, x, y);
	    }

	    // there is intentionally no 'break' here.


	case (TOOL_EVENT_IDLE):
            //printf("IDLE %d %d\n", x, y);

	    Page_Negative(page_rubberband);

	    if (brush_active)
		prim_brush(page_rubberband, page_active->fgpen, x, y);

	    primitive_crosshairs(page_rubberband, x, y);

	    break;

	case (TOOL_EVENT_NONE):
	default:
	    break;

    }
    drawing_mode = real_mode;
}


void
tools_filledrectangle_handler (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    static int startx;
    static int starty;

    switch (mouse_event)
    {
	case (TOOL_EVENT_INIT):
	    CURSOR_CROSSHAIRS();
	    Page_Negative(page_rubberband);
	    printf("filled rectangle\n");
            startx = x;
            starty = y;
	    break;

	case (TOOL_EVENT_DEINIT):
	    Page_Negative(page_rubberband);
	    break;

	case (TOOL_EVENT_STARTDOWN):
	    Page_Undo_Checkpoint(page_active);
	    startx = x;
	    starty = y;
	    break;

	case (TOOL_EVENT_DRAGGING):
	case (TOOL_EVENT_DRAG_STAT):
	    Page_Negative(page_rubberband);
	    if (mouse_buttons == 1)
	    {
		primitive_rectangle_filled(page_rubberband, page_active->fgpen,
			startx, starty, x, y);

	    }
	    else if (mouse_buttons == 3)
	    {
		primitive_rectangle_filled(page_rubberband, page_active->bgpen,
			startx, starty, x, y);
	    }
	    break;

	case (TOOL_EVENT_RELEASE):
	    Page_Negative(page_rubberband);
	    if (mouse_buttons == 1)
	    {
		primitive_rectangle_filled(page_active, page_active->fgpen,
			startx, starty, x, y);

	    }
	    else if (mouse_buttons == 3)
	    {
		primitive_rectangle_filled(page_active, page_active->bgpen,
			startx, starty, x, y);

	    }
	    // there is intentionally no 'break' here.

	case (TOOL_EVENT_IDLE):
	    Page_Negative(page_rubberband);
	    primitive_crosshairs(page_rubberband, x, y);
	    break;

	case (TOOL_EVENT_NONE):
	default:
	    break;

    }
}
