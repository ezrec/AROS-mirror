#ifndef HIGHSCORES_H
#define HIGHSCORES_H

/* highscores.h
 * ============
 * Interface to highscore handling.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>


void
load_high_scores (
   char  *default_name);

void
save_high_scores (void);

BOOL
update_high_score (
   UBYTE   game,
   UBYTE   player,
   UWORD   score,
   UBYTE   rolls);

void
display_high_scores (
   UBYTE   game);

#endif
