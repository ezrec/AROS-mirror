/*
 * tools
 * 
 *   callback mechanism for drawing tools
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

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "jsui.h"
#include "paint.h"

extern int drawing_mode;
extern int excl_brush;
extern int color_cycle;

void tools_init(void);

void primitive_crosshairs(IPAGE * page, int x, int y);
void prim_brush(IPAGE * page, PEN * pen, int x, int y);      



// tools/draw.c
void tools_dotted_freehand_handler(int me, int mb, int k, int x, int y);
void tools_continuous_freehand_handler(int me, int mb, int k, int x, int y);

// tools/vector.c
void tools_vector_handler(int me, int mb, int k, int x, int y);

// tools/fill.c
void tools_floodfill_handler(int me, int mb, int k, int x, int y);

// tools/airbrsh.c
void tools_airbrush_handler(int me, int mb, int k, int x, int y);

// tools/text.c
void tools_text_handler(int me, int mb, int k, int x, int y);

// tools/rectangl.c
void tools_rectangle_handler(int me, int mb, int k, int x, int y);
void tools_filledrectangle_handler(int me, int mb, int k, int x, int y);

// tools/circle.c
void tools_circle_handler(int me, int mb, int k, int x, int y);
void tools_filledcircle_handler(int me, int mb, int k, int x, int y);

// tools/brush.c
void tools_snag_brush_handler(int me, int mb, int k, int x, int y);
void tools_retrieve_old_brush(int me, int mb, int k, int x, int y);
void tools_dot(int me, int mb, int k, int x, int y);
void tools_brush_increase(int me, int mb, int k, int x, int y);
void tools_brush_decrease(int me, int mb, int k, int x, int y);
void tools_brush_circlesize(int me, int mb, int k, int x, int y);
void tools_brush_squaresize(int me, int mb, int k, int x, int y);

void tools_brush_circle(int me, int mb, int k, int x, int y);
void tools_brush_square(int me, int mb, int k, int x, int y);

void tools_brush_3(int me, int mb, int k, int x, int y);
void tools_brush_5(int me, int mb, int k, int x, int y);

// tools/brushgeo.c
void tools_brush_flip_horizontal(int me, int mb, int k, int x, int y);
void tools_brush_flip_vertical(int me, int mb, int k, int x, int y);
void tools_brush_rotate_90(int me, int mb, int k, int x, int y);
void tools_brush_halve_size(int me, int mb, int k, int x, int y);
void tools_brush_double_size(int me, int mb, int k, int x, int y);
void tools_brush_double_horizontal(int me, int mb, int k, int x, int y);
void tools_brush_double_vertical(int me, int mb, int k, int x, int y);

// tools/screen.c
void tools_undo(int me, int mb, int k, int x, int y);
void tools_clear_screen(int me, int mb, int k, int x, int y);
void tools_switch_to_spare(int me, int mb, int k, int x, int y);
void tools_copy_to_spare(int me, int mb, int k, int x, int y);
void tools_save_page(int me, int mb, int k, int x, int y);
void tools_load_page(int me, int mb, int k, int x, int y);

// tools/palette.c
void tools_eyedropper(int me, int mb, int k, int x, int y);
void tools_palette_fg_set(int me, int mb, int k, int x, int y);
void tools_palette_fg_decrement(int me, int mb, int k, int x, int y);
void tools_palette_fg_increment(int me, int mb, int k, int x, int y);
void tools_palette_bg_set(int me, int mb, int k, int x, int y);
void tools_palette_bg_decrement(int me, int mb, int k, int x, int y);
void tools_palette_bg_increment(int me, int mb, int k, int x, int y);
void tools_toggle_cycle(int me, int mb, int k, int x, int y);

// tools/drawmode.c
void tools_mode_change(int me, int mb, int k, int x, int y);

// tools/ui.c
void tools_ui_toggles(int me, int mb, int k, int x, int y);

#endif
