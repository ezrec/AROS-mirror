/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef INCLUDE_CSOLITAIRE_H
#define INCLUDE_CSOLITAIRE_H

#include "MUITools.h"

extern struct MUI_CustomClass *CL_CSolitaire;

#define MUIM_CSolitaire_NewGame     (TAGBASE_KAI | 0x2001)
#define MUIM_CSolitaire_Undo        (TAGBASE_KAI | 0x2002)
#define MUIA_CSolitaire_Timer       (TAGBASE_KAI | 0x2003)  /*  [I..] */

/* Klondike flags */
#define MUIM_CSolitaire_Sweep       (TAGBASE_KAI | 0x2010)
#define MUIM_CSolitaire_Move        (TAGBASE_KAI | 0x2011)
#define MUIA_CSolitaire_MoveButton  (TAGBASE_KAI | 0x2012)  /*  [I..] */
#define MUIA_CSolitaire_Score       (TAGBASE_KAI | 0x2013)  /*  [I..] */

#define MUIM_CSolitaire_GameMode    (TAGBASE_KAI | 0x2021)  /*  [IS.] */

struct Stats
{
  int  game_score;    /* current score */
  int  game_turns;    /* no stack turns */
  long all_score;     /* score of all games */
  int  all_won;       /* no games won */
  int  all_lost;      /* no games lost */
  long game_seconds;  /* time since game start */
  long all_seconds;   /* time of all games */
  BOOL finished;      /* game finished */
};

BOOL CSolitaire_Init(void);
void CSolitaire_Exit(void);

#endif
