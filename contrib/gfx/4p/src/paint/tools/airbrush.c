/*
 * airbrush
 * 
 *   airbrush tool callback handlers
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


#include <stdlib.h>  // for random
#include <time.h>    // for random
#include <stdio.h>
#include "jsui.h"
#include "tool_cb.h"
#include "tools.h"
#include "brushes.h"
#include "cursors.h"


////////////////////////////////////////////////////////////////////////////////
void
tools_airbrush_handler (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    int xoffs;
    int yoffs;
    int pplot;

    switch (mouse_event)
    {
	case (TOOL_EVENT_INIT):
	    CURSOR_CROSSHAIRS();
	    Page_Negative(page_rubberband);
	    printf("airbrush\n");
#if defined(WIN32)
	    srand( (unsigned) time(NULL));
#else
	    srandom( (unsigned) time(NULL));
#endif
	    break;

	case (TOOL_EVENT_DEINIT):
	    Page_Negative(page_rubberband);
	    break;

	case (TOOL_EVENT_STARTDOWN):
	    Page_Negative(page_rubberband);
	    Page_Undo_Checkpoint(page_active);
	    // and let it fall through...

	case (TOOL_EVENT_DRAGGING):
	case (TOOL_EVENT_DRAG_STAT):
	case (TOOL_EVENT_RELEASE):

	    if (mouse_buttons == 1)
	    {
		for (pplot = 0 ; pplot < 3 ; pplot++)
		{
		    // hardcoding the ranges for now.
		    // once we have a UI, it'll be a resizable circular region
#if defined(WIN32)
		    xoffs = (rand()&15)-7;
		    yoffs = (rand()&15)-7;
#else
		    xoffs = (random()&15)-7;
		    yoffs = (random()&15)-7;
#endif

		    if (brush_active)
			PAGE_DRAW_BRUSH(page_active, x+xoffs, y+yoffs,
					brush_active, drawing_mode);
		    else
			primitive_pixel(page_active, page_active->fgpen, 
				    x+xoffs, y+yoffs);
		}
	    }
	    else if (mouse_buttons == 3)
	    {
		for (pplot = 0 ; pplot < 3 ; pplot++)
		{
		    xoffs = (rand()&15)-7;    yoffs = (rand()&15)-7;

		    if (brush_active)
			PAGE_DRAW_BRUSH(page_active, x+xoffs, y+yoffs,
					brush_active, DRAW_STYLE_BGCOLOR);
		    else
			primitive_pixel(page_active, page_active->bgpen, 
				    x+xoffs, y+yoffs);
		}
	    }
	    break;

	case (TOOL_EVENT_IDLE):
	    Page_Negative(page_rubberband);
	    if (brush_active)
		prim_brush(page_rubberband, page_active->fgpen, x, y);
	    else
		primitive_pixel(page_rubberband, page_active->fgpen, x, y);
	    break;

	case (TOOL_EVENT_NONE):
	default:
	    break;
    }
}
