/*
 * layout.h
 * ========
 * Handles layout of display.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <exec/types.h>
#include <graphics/view.h>

#define NORMAL_SPACE   0
#define NO_SPACE       1

extern UBYTE   cell_space;
extern UBYTE   cell_w, cell_h;
extern BOOL    digital_display;
extern BOOL    display_colors;
extern UWORD   zoom_bounds[];

#define NUM_GAMEPENS   8

extern LONG    game_pens[];


void
window_extent (
   struct Screen  *scr,
   UBYTE           level,
   UWORD           font_w,
   UWORD           font_h,
   UWORD          *used_w,
   UWORD          *used_h);

void
init_pens (
   struct Screen  *scr);

void
free_pens (
   struct Screen  *scr);

BOOL
layout_display (
   struct Screen  *scr,
   BOOL           *rom_font);

#endif /* LAYOUT_H */
