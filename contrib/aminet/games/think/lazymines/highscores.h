/* highscores.h
 * ============
 * Interface to highscore handling.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <exec/types.h>


void
load_high_scores (
   char  *default_name);

void
save_high_scores (void);

UBYTE
update_high_score (
   UWORD   score);

void
display_high_scores (
   UBYTE   highlight_no);

#endif /* HIGHSCORES_H */
