/*
 * layout.h
 * ========
 * Interface to layout module.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */
#ifndef LAYOUT_H
#define LAYOUT_H

#include <exec/types.h>


#define NOPEN       -1
#define WHITEPEN    0
#define YELLOWPEN   1
#define ORANGEPEN   2
#define REDPEN      3
#define GREENPEN    4
#define BLUEPEN     5
#define VIOLETPEN   6
#define BLACKPEN    7
#define NUM_GAMEPENS   (BLACKPEN + 1)

extern WORD    game_pens[];
extern UWORD  *gui_pens;
extern BOOL    color_display;


#define BRICK1_W   17
#define BRICK1_H   15

#define BRICK2_W   7
#define BRICK2_H   7


struct box {
   WORD    left, top;
   UWORD   width, height;
};


extern struct box playfield_grid[12][2];


void
init_pens (
   struct Screen  *scr);

void
free_pens (
   struct Screen  *scr);

void
gadget_extent (
   struct Screen  *scr,
   UWORD          *width,
   UWORD          *height);

BOOL
window_extent (
   struct Screen  *scr,
   UBYTE           num_colors,
   UWORD          *width,
   UWORD          *height);

void
init_playfield_grid (
   struct Window  *win);

#endif /* LAYOUT_H */
