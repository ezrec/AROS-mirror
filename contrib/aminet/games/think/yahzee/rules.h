#ifndef RULES_H
#define RULES_H
/*
 * rules.h
 * =======
 * Definition of game rules.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>


#define MAX_NUM_DICE   6

/* labels */
#define ACES              0
#define TWOS              1
#define TREES             2
#define FOURS             3
#define FIVES             4
#define SIXES             5
#define UPPER_TOTAL       6
#define BONUS             7
#define ONE_PAIR          8
#define TWO_PAIRS         9
#define THREE_PAIRS       10
#define THREE_OF_A_KIND   11
#define FOUR_OF_A_KIND    12
#define FIVE_OF_A_KIND    13
#define SM_STRAIGHT       14
#define LG_STRAIGHT       15
#define FULL_STRAIGHT     16
#define FULL_HOUSE        17
#define HOUSE             18
#define TOWER             19
#define CHANCE            20
#define YAHZEE            21
#define TOTAL             22
#define SAVED_ROLLS       23
#define MAX_LABELS   (SAVED_ROLLS + 1)

/* scoring methods */
#define SCMTD_TRADITIONAL   0
#define SCMTD_AMERICAN      1
#define MAX_SCORINGMETHODS   (SCMTD_AMERICAN + 1)

/* game types */
#define GAMETYPE_ORIGINAL   0
#define GAMETYPE_MAXI       1
#define MAX_GAMETYPES   (GAMETYPE_MAXI + 1)


/* procedure for calculating score */
typedef void (*scoreproc) (UBYTE dice[], UBYTE score[]);

struct rules {
   BOOL        labels[MAX_LABELS];
   UBYTE       num_dice;
   scoreproc   calc_score;
   UBYTE       bonus_limit, bonus;
   ULONG       yahzee_label;
};
typedef struct rules game[MAX_SCORINGMETHODS];


extern const UBYTE   label_msg[];
/* definition of the different games */
extern const game   game_definition[MAX_GAMETYPES];


UBYTE
game_number (
   struct rules  *rules);

UBYTE
num_upper_labels (
   struct rules  *rules);

UBYTE
num_lower_labels (
   struct rules  *rules);

UBYTE
upper_label_len (
   struct rules  *rules);

UBYTE
lower_label_len (
   struct rules  *rules);

#endif
