/*
 * draw.h
 * ======
 * Interface to drawing functions.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */
#ifndef DRAW_H
#define DRAW_H

#include <exec/types.h>
#include <intuition/intuition.h>


void
place_large_brick (
   struct Window  *win,
   UBYTE           row,
   UBYTE           hole,
   BYTE            color);

void
place_small_brick (
   struct Window  *win,
   UBYTE           row,
   UBYTE           hole,
   BYTE            color);

void
draw_solution (
   struct Window  *win,
   APTR            vis_info,
   BYTE            solution[],
   BOOL            show_it);

void
draw_playfield (
   struct Window  *win,
   APTR            vis_info,
   BYTE            guess[][],
   BYTE            answer[][],
   BOOL            show_solution);

void
draw_panel (
   struct Window  *win,
   UBYTE           num_colors,
   UBYTE           chosen_num_colors,
   BYTE            selection);

void
select_panel (
   struct Window  *win,
   BYTE           selection);

void
delete_gadgets (
   struct RastPort  *rp,
   struct Gadget    *gad);

#endif /* DRAW_H */
