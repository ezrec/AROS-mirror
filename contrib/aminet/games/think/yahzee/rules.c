/*
 * rules.c
 * =======
 * Definition of game rules.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <string.h>
#include <exec/types.h>
#include "localize.h"
#include "game.h"
#include "rules.h"


const UBYTE   label_msg[] = {
   MSG_ACES_LABEL, MSG_TWOS_LABEL, MSG_THREES_LABEL, MSG_FOURS_LABEL,
   MSG_FIVES_LABEL, MSG_SIXES_LABEL, MSG_UPPERTOTAL_LABEL, MSG_BONUS_LABEL,
   MSG_ONEPAIR_LABEL, MSG_TWOPAIRS_LABEL, MSG_THREEPAIRS_LABEL,
   MSG_THREEKIND_LABEL, MSG_FOURKIND_LABEL, MSG_FIVEKIND_LABEL,
   MSG_SMSTRAIGHT_LABEL, MSG_LGSTRAIGHT_LABEL, MSG_FULLSTRAIGHT_LABEL,
   MSG_FULLHOUSE_LABEL, MSG_HOUSE_LABEL, MSG_TOWER_LABEL, MSG_CHANCE_LABEL,
   MSG_YAHZEE_LABEL, MSG_TOTAL_LABEL, MSG_SAVEDROLLS_LABEL
};


static void calc_trad_original (UBYTE dice[], UBYTE score[]);
static void calc_amer_original (UBYTE dice[], UBYTE score[]);
static void calc_trad_maxi (UBYTE dice[], UBYTE score[]);
static void calc_amer_maxi (UBYTE dice[], UBYTE score[]);

const game   game_definition[MAX_GAMETYPES] = {
   {  {  { 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
         5, calc_trad_original, 63, 50, MSG_YAHZEE_LABEL },
      {  { 1, 1, 1, 1, 1, 1, 1, 1,
           0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
         5, calc_amer_original, 63, 35, MSG_YAHZEE_LABEL } },
   {  {  { 1, 1, 1, 1, 1, 1, 1, 1,
           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
         6, calc_trad_maxi, 84, 100, MSG_MAXIYAHZEE_LABEL },
      {  { 1, 1, 1, 1, 1, 1, 1, 1,
           0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
         6, calc_amer_maxi, 84, 70, MSG_MAXIYAHZEE_LABEL } }
};


static void
calc_trad_original (
   UBYTE   dice[],
   UBYTE   score[])
{
   UBYTE   kind_count[6];
   register UBYTE   n;
   register UBYTE   pair1 = 0, pair2 = 0, tripple = 0;
   
   /* initialize kind_count and calculate chance */
   score[CHANCE] = 0;
   for (n = 0; n < 6; ++n)
      kind_count[n] = 0;
   for (n = 0; n < 5; ++n)
   {
      score[CHANCE] += dice[n];
      ++kind_count[dice[n] - 1];
   }
   
   score[FOUR_OF_A_KIND] = 0;
   score[YAHZEE] = 0;
   /* aces, twos, threes, fours, fives, sixes, 4 of a kind and yahzee */
   for (n = 0; n < 6; ++n)
   {
      score[n] = kind_count[n] * (n + 1);
      if (kind_count[n] >= 4)
         score[FOUR_OF_A_KIND] = 4 * (n + 1);
      if (kind_count[n] == 5)
         score[YAHZEE] = 50;
   }
   
   /* small and large straight */
   for (n = 1; n < 6 && kind_count[n] == 1; ++n)
      ;
   score[SM_STRAIGHT] = (n == 5) ? 15 : 0;
   score[LG_STRAIGHT] = (n == 6) ? 20 : 0;
   
   /* 1 pair, 2 pairs, 3 of a kind and full house */
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] >= 2)
      {
         if (pair1)
            pair2 = n + 1;
         else
            pair1 = n + 1;
      }
      if (kind_count[n] >= 3)
      {
         tripple = n + 1;
         if (pair1 == tripple)
         {
            pair1 = pair2;
            pair2 = tripple;
         }
      }
   }
   score[ONE_PAIR] = (pair1 > pair2) ? 2 * pair1 : 2 * pair2;
   score[TWO_PAIRS] = (pair1 && pair2) ? 2 * (pair1 + pair2) : 0;
   score[THREE_OF_A_KIND] = 3 * tripple;
   score[FULL_HOUSE] = (pair1 && tripple) ? 2 * pair1 + 3 * tripple : 0;
}

static void
calc_amer_original (
   UBYTE   dice[],
   UBYTE   score[])
{
   UBYTE   kind_count[6];
   register UBYTE   n, straight = 0;
   register UBYTE   pair1 = 0, pair2 = 0, tripple = 0;
   
   /* initialize kind_count and calculate chance */
   score[CHANCE] = 0;
   for (n = 0; n < 6; ++n)
      kind_count[n] = 0;
   for (n = 0; n < 5; ++n)
   {
      score[CHANCE] += dice[n];
      ++kind_count[dice[n] - 1];
   }
   
   score[FOUR_OF_A_KIND] = 0;
   score[YAHZEE] = 0;
   /* aces, twos, threes, fours, fives, sixes, 4 of a kind and yahzee */
   for (n = 0; n < 6; ++n)
   {
      score[n] = kind_count[n] * (n + 1);
      if (kind_count[n] >= 4)
         score[FOUR_OF_A_KIND] = score[CHANCE];
      if (kind_count[n] == 5)
         score[YAHZEE] = 50;
   }
   
   /* small and large straight */
   score[SM_STRAIGHT] = 0;
   score[LG_STRAIGHT] = 0;
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] == 0)
         straight = 0;
      else
         ++straight;
      if (straight == 4)
         score[SM_STRAIGHT] = 30;
      if (straight == 5)
         score[LG_STRAIGHT] = 40;
   }
   
   /* 3 of a kind and full house */
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] >= 2)
      {
         if (pair1)
            pair2 = n + 1;
         else
            pair1 = n + 1;
      }
      if (kind_count[n] >= 3)
      {
         tripple = n + 1;
         if (pair1 == tripple)
         {
            pair1 = pair2;
            pair2 = tripple;
         }
      }
   }
   score[THREE_OF_A_KIND] = (tripple) ? score[CHANCE] : 0;
   score[FULL_HOUSE] = (pair1 && tripple) ? 25 : 0;
}

static void
calc_trad_maxi (
   UBYTE   dice[],
   UBYTE   score[])
{
   UBYTE   kind_count[6];
   register UBYTE   n;
   UBYTE   pair[3] = { 0, 0, 0 };
   register UBYTE   tripple1 = 0, tripple2 = 0, quad = 0;
   
   /* initialize kind_count and calculate chance */
   score[CHANCE] = 0;
   for (n = 0; n < 6; ++n)
      kind_count[n] = 0;
   for (n = 0; n < 6; ++n)
   {
      score[CHANCE] += dice[n];
      ++kind_count[dice[n] - 1];
   }
   
   score[FIVE_OF_A_KIND] = 0;
   score[YAHZEE] = 0;
   /* aces, twos, threes, fours, fives, sixes, 5 of a kind and yahzee */
   for (n = 0; n < 6; ++n)
   {
      score[n] = kind_count[n] * (n + 1);
      if (kind_count[n] >= 5)
         score[FIVE_OF_A_KIND] = 5 * (n + 1);
      if (kind_count[n] == 6)
         score[YAHZEE] = 100;
   }
   
   /* small, large and full straight */
   for (n = 1; n < 6 && kind_count[n] >= 1; ++n)
      ;
   score[SM_STRAIGHT] = (n >= 5 && kind_count[0] >= 1) ? 15 : 0;
   score[LG_STRAIGHT] = (n == 6) ? 20 : 0;
   score[FULL_STRAIGHT] = (n == 6 && kind_count[0] >= 1) ? 21 : 0;
   
   /* 1 pair, 2 pairs, 3 pairs, 3 of a kind, 4 of a kind, */
   /* full house, house and tower                         */
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] >= 2)
      {
         if (pair[1])
            pair[2] = n + 1;
         else if (pair[0])
            pair[1] = n + 1;
         else
            pair[0] = n + 1;
      }
      if (kind_count[n] >= 3)
      {
         if (tripple1)
            tripple2 = n + 1;
         else
            tripple1 = n + 1;
      }
      if (kind_count[n] >= 4)
         quad = n + 1;
   }
   score[ONE_PAIR] = 2 * pair[0];
   for (n = 1; n <= 2; ++n)
      if (pair[n])
         score[ONE_PAIR] = 2 * pair[n];
   score[TWO_PAIRS] = (pair[1]) ? ((pair[2]) ? 2 * (pair[2] + pair[1]) :
                                               2 * (pair[0] + pair[1])) : 0;
   score[THREE_PAIRS] = (pair[2]) ? 2 * (pair[0] + pair[1] + pair[2]) : 0;
   score[THREE_OF_A_KIND] = (tripple2) ? 3 * tripple2 : 3 * tripple1;
   score[FOUR_OF_A_KIND] = 4 * quad;
   score[FULL_HOUSE] = 0;
   if (score[THREE_OF_A_KIND])
      for (n = 0; n <= 2; ++n)
         if (pair[n] && pair[n] != score[THREE_OF_A_KIND] / 3)
               score[FULL_HOUSE] = 2 * pair[n] + score[THREE_OF_A_KIND];
   score[HOUSE] = (tripple2) ? 3 * (tripple1 + tripple2) : 0;
   score[TOWER] = 0;
   if (quad)
      for (n = 0; n <= 2; ++n)
         if (pair[n] && pair[n] != quad)
            score[TOWER] = 2 * pair[n] + 4 * quad;
}

static void
calc_amer_maxi (
   UBYTE   dice[],
   UBYTE   score[])
{
   UBYTE   kind_count[6];
   register UBYTE   n, straight = 0;
   UBYTE   pair[3] = { 0, 0, 0 };
   register UBYTE   tripple1 = 0, tripple2 = 0, quad = 0;
   
   /* initialize kind_count and calculate chance */
   score[CHANCE] = 0;
   for (n = 0; n < 6; ++n)
      kind_count[n] = 0;
   for (n = 0; n < 6; ++n)
   {
      score[CHANCE] += dice[n];
      ++kind_count[dice[n] - 1];
   }
   
   score[FIVE_OF_A_KIND] = 0;
   score[YAHZEE] = 0;
   /* aces, twos, threes, fours, fives, sixes, 5 of a kind and yahzee */
   for (n = 0; n < 6; ++n)
   {
      score[n] = kind_count[n] * (n + 1);
      if (kind_count[n] >= 5)
         score[FIVE_OF_A_KIND] = score[CHANCE];
      if (kind_count[n] == 6)
         score[YAHZEE] = 100;
   }
   
   /* small, large and full straight */
   score[SM_STRAIGHT] = 0;
   score[LG_STRAIGHT] = 0;
   score[FULL_STRAIGHT] = 0;
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] == 0)
         straight = 0;
      else
         ++straight;
      if (straight == 4)
         score[SM_STRAIGHT] = 30;
      if (straight == 5)
         score[LG_STRAIGHT] = 40;
      if (straight == 6)
         score[FULL_STRAIGHT] = 50;
   }
   
   /* 3 of a kind, 4 of a kind, full house, house, tower */
   for (n = 0; n < 6; ++n)
   {
      if (kind_count[n] >= 2)
      {
         if (pair[1])
            pair[2] = n + 1;
         else if (pair[0])
            pair[1] = n + 1;
         else
            pair[0] = n + 1;
      }
      if (kind_count[n] >= 3)
      {
         if (tripple1)
            tripple2 = n + 1;
         else
            tripple1 = n + 1;
      }
      if (kind_count[n] >= 4)
         quad = n + 1;
   }
   score[THREE_OF_A_KIND] = (tripple2 || tripple1) ? score[CHANCE] : 0;
   score[FOUR_OF_A_KIND] = (quad) ? score[CHANCE] : 0;
   score[FULL_HOUSE] = 0;
   if (tripple1)
      for (n = 0; n <= 2; ++n)
         if (pair[n] && pair[n] != ((tripple2) ? tripple2 : tripple1))
               score[FULL_HOUSE] = 25;
   score[HOUSE] = (tripple2) ? 30 : 0;
   score[TOWER] = 0;
   if (quad)
      for (n = 0; n <= 2; ++n)
         if (pair[n] && pair[n] != quad)
            score[TOWER] = 35;
}

static UBYTE
num_labels (
   struct rules  *rules)
{
   register UBYTE   n, count = 0;
   
   for (n = 0; n < MAX_LABELS; ++n)
      if (rules->labels[n])
         ++count;
   
   return count;
}


UBYTE
game_number (
   struct rules  *rules)
{
   if (rules == &game_definition[1][1])
      return 3;
   else if (rules == &game_definition[1][0])
      return 2;
   else if (rules == &game_definition[0][1])
      return 1;
   else
      return 0;
}

UBYTE
num_upper_labels (
   struct rules  *rules)
{
   return (UBYTE)(num_labels (rules) / 2);
}

UBYTE
num_lower_labels (
   struct rules  *rules)
{
   return (UBYTE)(num_labels (rules) - num_upper_labels (rules));
}

UBYTE
upper_label_len (
   struct rules  *rules)
{
   UBYTE   label_len = 0, temp, limit;
   register UBYTE   n;
   
   limit = num_upper_labels (rules);
   for (n = 0; n < limit; ++n)
   {
      if (!rules->labels[n])
         ++limit;
      else
      {
         temp = strlen (localized_string (label_msg[n]));
         if (temp > label_len)
            label_len = temp;
      }
   }
   
   return label_len;
}

UBYTE
lower_label_len (
   struct rules  *rules)
{
   UBYTE   label_len = 0, temp, limit;
   register UBYTE   n;
   
   limit = num_upper_labels (rules);
   for (n = 0; n < limit; ++n)
      if (!rules->labels[n])
         ++limit;
   for (n = limit; n < MAX_LABELS; ++n)
   {
      if (rules->labels[n])
      {
         temp = strlen (localized_string ((n == YAHZEE) ? rules->yahzee_label :
                                          label_msg[n]));
         if (temp > label_len)
            label_len = temp;
      }
   }
   
   return label_len;
}
