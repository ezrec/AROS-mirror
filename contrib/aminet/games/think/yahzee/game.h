#ifndef GAME_H
#define GAME_H
/*
 * game.h
 * ======
 * Interface to game functions.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#define MAX_PLAYERS   6


BOOL
new_game (void);

void
roll_dice (void);

UBYTE
choose_score (
   WORD   x,
   WORD   y);

void
undo_last_choice (void);

#endif
