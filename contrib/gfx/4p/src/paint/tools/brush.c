/*
 * brush
 * 
 *   brush tools callback handlers
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
// snag brush


void
tools_snag_brush_handler (
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
	    printf("brush\n");
	    startx = x;
	    starty = y;
	    break;

	case (TOOL_EVENT_DEINIT):
	    Page_Negative(page_rubberband);
	    break;

	case (TOOL_EVENT_STARTDOWN):
	    startx = x;
	    starty = y;
	    break;

	case (TOOL_EVENT_DRAGGING):
	case (TOOL_EVENT_DRAG_STAT):
	    Page_Negative(page_rubberband);
	    if (mouse_buttons == 1)
	    {
		primitive_rectangle_hollow(page_rubberband, pen_cycle,
			primitive_pixel, startx, starty, x, y);

	    }
	    else if (mouse_buttons == 3)
	    {
		primitive_rectangle_hollow(page_rubberband, pen_cycle,
			primitive_pixel, startx, starty, x, y);
	    }
	    break;

	case (TOOL_EVENT_RELEASE):
	    Page_Negative(page_rubberband);
	    printf ("Snag brush:  (%d,%d) - (%d,%d)  %d %d\n", 
			x, y, startx, starty, 
			ABS(x-startx), ABS(y-starty));

            if (brush_custom)  Page_Destroy(brush_custom);

	    brush_custom = Page_Cutout_Brush(
				page_active, startx, starty, x, y,
				excl_brush, 
				(mouse_buttons == 3)?page_active->bgpen:NULL);

	    brush_active = brush_custom;
				
	    jsui_dialog_broadcast(PAINT_MSG_DOTTEDDRAW, 0);
	    jsui_dialog_broadcast(PAINT_MSG_MODEMATTE, 0);
	    drawing_mode = DRAW_STYLE_MATTE;
	    break;


	case (TOOL_EVENT_IDLE):
	    Page_Negative(page_rubberband);
	    primitive_crosshairs(page_rubberband, x, y);
	    break;

	case (TOOL_EVENT_NONE):
	default:
	    break;

    }
}



void
tools_brush_circlesize (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("brush size %d\n", keyhit);

	if (brush_circle) Page_Destroy(brush_circle);
	brush_circle = NULL;
	
	brush_size   = keyhit;
	if (brush_size == 0)
	{
	    brush_active = NULL;
	} else {
	    brush_circle = brush_create_circle(brush_size);
	    brush_active = brush_circle;
	}
	drawing_mode = DRAW_STYLE_COLOR;
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}

void
tools_brush_squaresize (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("square brush size %d\n", keyhit);

	if (brush_square) Page_Destroy(brush_square);
	brush_square = NULL;

	brush_size = keyhit;
	if (brush_size == 0)
	{
	    brush_active = NULL;
	} else {
	    brush_square = brush_create_square(brush_size);
	    brush_active = brush_square;
	}
	drawing_mode = DRAW_STYLE_COLOR;
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}

void
tools_brush_increase (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	if (brush_active == NULL)
	{
	    brush_size = 0;
	    brush_active = brush_circle;
	}

	if (brush_active == brush_circle)
	{
	    brush_size++;
	    printf("increase circle brush to %d\n", brush_size);

	    if (brush_circle) Page_Destroy(brush_circle);
	    brush_circle = brush_create_circle(brush_size);
	    brush_active = brush_circle;

	    jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, brush_size);
	    drawing_mode = DRAW_STYLE_COLOR;
	    jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
	}
	else if (brush_active == brush_square)
	{
	    brush_size++;
	    printf("increase square brush to %d\n", brush_size);

	    if (brush_square) Page_Destroy(brush_square);
	    brush_square = brush_create_square(brush_size);
	    brush_active = brush_square;

	    jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, brush_size);
	    drawing_mode = DRAW_STYLE_COLOR;
	    jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
	}
    }
}

void
tools_brush_decrease (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("decrease brush size to %d\n", brush_size-1);

	if ( (brush_active == brush_circle) ||
	     (brush_active == brush_square) )
	{
	    if (brush_size == 1)
	    {
		brush_active = NULL;
		jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0);
	    }

	    if (brush_size > 1)
	    {
		brush_size--;

		if (brush_active == brush_circle)
		{
		    printf("decrease circle brush to %d\n", brush_size);

		    if (brush_circle) Page_Destroy(brush_circle);
		    brush_circle = brush_create_circle(brush_size);
		    brush_active = brush_circle;
		    jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, brush_size);
		}
		else if (brush_active == brush_square)
		{
		    printf("decrease square brush to %d\n", brush_size);

		    if (brush_square) Page_Destroy(brush_square);
		    brush_square = brush_create_square(brush_size);
		    brush_active = brush_square;
		    if ( brush_size == 1 )
		    {
			brush_active = NULL;
			jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0);
		    } else 
			jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, brush_size);
		}
		drawing_mode = DRAW_STYLE_COLOR;
		jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
	    }
	}
    }
}

void
tools_dot (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("dot\n");

	brush_size = 0;
	brush_active = NULL;
	drawing_mode = DRAW_STYLE_COLOR;

	jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0);
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}


void
tools_retrieve_old_brush (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("old brush swap\n");
	
	if (brush_custom)
	{
	    printf("switching to custom\n");
	    brush_size   = 0;
	    brush_active = brush_custom;
	    drawing_mode = DRAW_STYLE_MATTE;
	    jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, 0);
	    jsui_dialog_broadcast(PAINT_MSG_MODEMATTE, 0);
	}
    }
}



void
tools_brush_circle (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("switch to circle brush\n");

	if (brush_circle) Page_Destroy(brush_circle);
	brush_circle = brush_create_circle(brush_size);
	brush_active = brush_circle;
	drawing_mode = DRAW_STYLE_COLOR;
	jsui_dialog_broadcast(PAINT_MSG_CIRCLEBRUSHSIZE, brush_size);
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}

void
tools_brush_square (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("switch to square brush\n");

	if (brush_square) Page_Destroy(brush_square);
	brush_square = brush_create_square(brush_size);
	brush_active = brush_square;
	drawing_mode = DRAW_STYLE_COLOR;

	jsui_dialog_broadcast(PAINT_MSG_SQUAREBRUSHSIZE, brush_size);
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}


////////////////////////////////////////////////////////////////////////////////

void
tools_brush_3 (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("internal scatterbrush 3\n");

	brush_active = brush_3;
	drawing_mode = DRAW_STYLE_COLOR;
	//jsui_dialog_broadcast(PAINT_MSG_BRUSH3, 0);
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}


void
tools_brush_5 (
	int mouse_event,
	int mouse_buttons,
	int keyhit,
	int x,
	int y
)
{
    if (mouse_event == TOOL_EVENT_INIT)
    {
	printf("internal scatterbrush 5\n");

	brush_active = brush_5;
	drawing_mode = DRAW_STYLE_COLOR;
	//jsui_dialog_broadcast(PAINT_MSG_BRUSH5, 0);
	jsui_dialog_broadcast(PAINT_MSG_MODECOLOR, 0);
    }
}


