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

//#include <stdio.h>

#include "SDL.h"
#include "jsui.h"
#include "tool_cb.h"
#include "brushes.h"
#include "tools.h"

// these should eventually be moved elsewhere perhaps

int drawing_mode = DRAW_STYLE_MATTE;
int excl_brush = 0;
int color_cycle = 0;


void 
primitive_crosshairs (
	IPAGE * page,
	int x,
	int y
)
{
#ifdef FULLSCREEN_CROSSHAIRS
    primitive_line_horizontal(page_rubberband, page_active->fgpen, 
		primitive_pixel, 0, page_active->w-1, y);
		//primitive_pixel, x-20, x+20, y); // page_active->w-1, y);

    primitive_line_vertical(page_rubberband, page_active->fgpen, 
		primitive_pixel, x, 0, page_active->h-1);
		//primitive_pixel, x, y-20, y+20); //0, page_active->h-1);
#else
    int pos;

#define XH_SIZE  15
#define XH_SPACE 2

    for (pos = x-XH_SIZE ; pos<= x+XH_SIZE ; pos +=XH_SPACE)
	if (pos != x)
	primitive_pixel(page_rubberband, page_active->fgpen, pos, y);

    for (pos = y-XH_SIZE ; pos<= y+XH_SIZE ; pos +=XH_SPACE)
	if (pos != y)
	primitive_pixel(page_rubberband, page_active->fgpen, x, pos);

#undef XH_SIZE
#undef XH_SPACE

#endif
}


void
prim_brush(
	IPAGE * page,
	PEN * pen,
	int x,
	int y
)
{
    PAGE_DRAW_BRUSH(page, x, y, brush_active, drawing_mode);
}


////////////////////////////////////////////////////////////////////////////////
/*
callback_register will register PAINT_MSG_<toolname> items to their
controlling functions

some of those functions send out more messages
ie:
    fg color inc
	gets the current color
	increments it (with bounds check)
	sends out a [set fg color] message appropriately

 only one base message needed:  set fg color message

*/

void
tools_init(
	void
)
{
    /* these need some tweaking to get them to work right */
    tool_callback_register('=', CB_ONESHOT, tools_brush_increase);
    tool_callback_register('+', CB_ONESHOT, tools_brush_increase);
    tool_callback_register('-', CB_ONESHOT, tools_brush_decrease);
    tool_callback_register('_', CB_ONESHOT, tools_brush_decrease);

    tool_callback_register(PAINT_MSG_CIRCLEBRUSHSIZE, CB_ONESHOT, tools_brush_circlesize);
    tool_callback_register(PAINT_MSG_SQUAREBRUSHSIZE, CB_ONESHOT, tools_brush_squaresize);

    tool_callback_register('[', CB_ONESHOT, tools_palette_fg_decrement);
    tool_callback_register(']', CB_ONESHOT, tools_palette_fg_increment);
    tool_callback_register('{', CB_ONESHOT, tools_palette_bg_decrement);
    tool_callback_register('}', CB_ONESHOT, tools_palette_bg_increment);

    tool_callback_register(PAINT_MSG_FGPALETTESET, CB_ONESHOT, tools_palette_fg_set);
    tool_callback_register(PAINT_MSG_BGPALETTESET, CB_ONESHOT, tools_palette_bg_set);

    /* these should be fine */
    tool_callback_register('s', CB_TOOL, tools_dotted_freehand_handler);
    tool_callback_register('d', CB_TOOL, tools_continuous_freehand_handler);
    tool_callback_register('v', CB_TOOL, tools_vector_handler);
    tool_callback_register('f', CB_TOOL, tools_floodfill_handler);

    tool_callback_register(PAINT_MSG_DOTTEDDRAW, CB_TOOL, tools_dotted_freehand_handler);
    tool_callback_register(PAINT_MSG_CONTINUOUSDRAW, CB_TOOL, tools_continuous_freehand_handler);
    tool_callback_register(PAINT_MSG_VECTORDRAW, CB_TOOL, tools_vector_handler);
    tool_callback_register(PAINT_MSG_FLOODFILL, CB_TOOL, tools_floodfill_handler);

    tool_callback_register('a', CB_TOOL, tools_airbrush_handler);
    tool_callback_register('t', CB_TOOL, tools_text_handler);

    tool_callback_register(PAINT_MSG_AIRBRUSH, CB_TOOL, tools_airbrush_handler);
    tool_callback_register(PAINT_MSG_TEXT, CB_TOOL, tools_text_handler);

    tool_callback_register('r', CB_TOOL, tools_rectangle_handler);
    tool_callback_register('R', CB_TOOL, tools_filledrectangle_handler);
    tool_callback_register(PAINT_MSG_RECTANGLE, CB_TOOL, tools_rectangle_handler);
    tool_callback_register(PAINT_MSG_FILLEDRECTANGLE, CB_TOOL, tools_filledrectangle_handler);

    tool_callback_register('c', CB_TOOL, tools_circle_handler);
    tool_callback_register('C', CB_TOOL, tools_filledcircle_handler);
    tool_callback_register(PAINT_MSG_CIRCLE, CB_TOOL, tools_circle_handler);
    tool_callback_register(PAINT_MSG_FILLEDCIRCLE, CB_TOOL, tools_filledcircle_handler);

    tool_callback_register('b', CB_TOOL, tools_snag_brush_handler);
    tool_callback_register('B', CB_ONESHOT, tools_retrieve_old_brush);

    tool_callback_register(PAINT_MSG_SNAGBRUSH, CB_TOOL, tools_snag_brush_handler);
    tool_callback_register(PAINT_MSG_OLDBRUSH, CB_ONESHOT, tools_retrieve_old_brush);
    tool_callback_register('.', CB_ONESHOT, tools_dot);
    tool_callback_register(PAINT_MSG_DOTDRAW, CB_ONESHOT, tools_dot);

/*
    tool_callback_register('1', CB_ONESHOT, tools_brush_circle);
    tool_callback_register('2', CB_ONESHOT, tools_brush_square);
    tool_callback_register('3', CB_ONESHOT, tools_brush_3);
    tool_callback_register('5', CB_ONESHOT, tools_brush_5);
*/
    tool_callback_register(PAINT_MSG_BRUSH3, CB_ONESHOT, tools_brush_3);
    tool_callback_register(PAINT_MSG_BRUSH5, CB_ONESHOT, tools_brush_5);

    tool_callback_register('x', CB_ONESHOT, tools_brush_flip_horizontal);
    tool_callback_register('y', CB_ONESHOT, tools_brush_flip_vertical);
    tool_callback_register('z', CB_ONESHOT, tools_brush_rotate_90);

    tool_callback_register('h', CB_ONESHOT, tools_brush_halve_size);
    tool_callback_register('H', CB_ONESHOT, tools_brush_double_size);
    tool_callback_register('X', CB_ONESHOT, tools_brush_double_horizontal);
    tool_callback_register('Y', CB_ONESHOT, tools_brush_double_vertical);

    tool_callback_register('u', CB_ONESHOT, tools_undo);
    tool_callback_register('K', CB_ONESHOT, tools_clear_screen);
    tool_callback_register('j', CB_ONESHOT, tools_switch_to_spare);
    tool_callback_register('J', CB_ONESHOT, tools_copy_to_spare);

    tool_callback_register(PAINT_MSG_UNDO, CB_ONESHOT, tools_undo);
    tool_callback_register(PAINT_MSG_CLEARSCREEN, CB_ONESHOT, tools_clear_screen);

    tool_callback_register('S', CB_ONESHOT, tools_save_page);
    tool_callback_register('L', CB_ONESHOT, tools_load_page);

    tool_callback_register(',', CB_TOOL, tools_eyedropper);
    tool_callback_register(PAINT_MSG_EYEDROPPER, CB_TOOL, tools_eyedropper);

    tool_callback_register(SDLK_TAB, CB_ONESHOT, tools_toggle_cycle);

    tool_callback_register(SDLK_F1, CB_ONESHOT, tools_mode_change);
    tool_callback_register(SDLK_F2, CB_ONESHOT, tools_mode_change);
    tool_callback_register(SDLK_F3, CB_ONESHOT, tools_mode_change);

    tool_callback_register(SDLK_F8, CB_ONESHOT, tools_ui_toggles);
}
