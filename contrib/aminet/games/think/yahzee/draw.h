#ifndef DRAW_H
#define DRAW_H
/*
 * draw.h
 * ======
 * Interface to drawing functions.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include "game.h"
#include "rules.h"


#define BOARD_BACKGROUNDPEN   0
#define BOARD_PAPERPEN        1
#define BOARD_LINEPEN         2
#define BOARD_TEXTPEN         3
#define BOARD_HIGHLIGHTPEN    4
#define BOARD_FILLPEN         5


struct box {
   WORD    left, top;
   UWORD   width, height;
};


extern struct box   upper_board, lower_board;
extern struct box   board_grid[MAX_PLAYERS + 1][MAX_LABELS];


void
init_board_pens (
   UWORD  *gui_pens);

void
delete_gadgets (
   struct RastPort  *rp,
   struct Gadget    *gad);

void
draw_scoreboard (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             num_players);

void
delete_scoreboard (
   struct RastPort  *rp);

void
reset_scoreboard (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             num_players);

void
draw_player_numbers (
   struct RastPort  *rp,
   UBYTE             num_players,
   UBYTE             current_player);

void
draw_possible_score (
   struct RastPort  *rp,
   struct rules     *rules,
   UWORD             current_player,
   WORD              scoreboard[],
   UBYTE             possible_score[]);

void
draw_chosen_score (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             current_player,
   WORD              scoreboard[],
   BOOL              highlighted);

#endif
