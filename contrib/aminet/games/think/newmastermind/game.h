/*
 * game.h
 * ======
 * Interface to game functions.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */
#ifndef GAME_H
#define GAME_H

#include <exec/types.h>
#include <libraries/gadtools.h>
#include <intuition/intuition.h>


#define HUMAN      0
#define COMPUTER   1

#define CHILDREN   0
#define ADULTS     1

extern struct Window  *main_win;
extern APTR   vis_info;
extern struct Gadget  *ok_gad, *gt_gads;
extern struct NewGadget   new_ok_gad;

extern UBYTE   num_colors, chosen_num_colors;
extern UBYTE   opponent, chosen_opponent;
extern UBYTE   correction, chosen_correction;

extern UBYTE   current_row;
extern BYTE   active_color;
extern BYTE   guess[][];
extern BYTE   answer[][];


BOOL
new_game (void);

void
handle_mouseclick (
   UWORD   x,
   UWORD   y);

void
handle_mousemove (
   UWORD   x,
   UWORD   y);

void
end_of_turn (void);

#endif /* GAME_H */
