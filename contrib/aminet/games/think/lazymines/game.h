/*
 * game.h
 * ======
 * Interface to game module.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef GAME_H
#define GAME_H

#include <exec/types.h>
#include "counter.h"
#include "timer.h"


#define MIN_ROWS      8
#define MAX_ROWS      50
#define MIN_COLUMNS   8
#define MAX_COLUMNS   50
#define MIN_MINES     1

extern UBYTE   max_rows, max_columns;


struct level {
   UWORD   columns;
   UWORD   rows;
   UWORD   bombs;
};

#define NOVICE_LEVEL     1
#define AMATEUR_LEVEL    2
#define EXPERT_LEVEL     3
#define OPTIONAL_LEVEL   0

extern struct level   levels[];
extern UBYTE          current_level;

#define SWEEP_ALL    0
#define SWEEP_PATH   1

extern UBYTE   chosen_task;

#define MIN_OPENING   0
#define MAX_OPENING   10

extern UBYTE   auto_opening;
extern BOOL    place_warnings;

extern counter_ptr   time_counter;
extern counter_ptr   flag_counter;

extern timer_ptr   timer_obj;
extern BOOL        time_on;

#endif
