/*
 * game.c
 * ======
 * Game functions.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <math.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include "rules.h"
#include "layout.h"
#include "draw.h"
#include "game.h"
#include "dice.h"
#include "highscores.h"
#include <libraries/gadtools.h>

#include <proto/gadtools.h>
#include <proto/intuition.h>


extern struct Window  *main_win;
extern struct NewGadget   new_roll_gad;
extern struct Gadget  die_gads[], *roll_gad, *disp_gad, *gt_gads;
extern struct rules  *rules;
extern UBYTE   num_players, chosen_num_players;
extern UBYTE   chosen_gametype, chosen_scoring;

static UBYTE   rolls_left;
static UBYTE   rounds_left;
static UBYTE   last_choice, prev_rolls_left;
static UBYTE   current_player;
static UBYTE   die_values[MAX_NUM_DICE];
static WORD    scoreboard[MAX_PLAYERS][MAX_LABELS];
static UBYTE   possible_score[MAX_LABELS];


static void
end_of_game (void)
{
   register UBYTE   n;
   register BYTE    m;
   UBYTE   order[MAX_PLAYERS];
   BOOL    new_high = FALSE;
   
   order[0] = 0;
   
   GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                      GA_Disabled, TRUE, TAG_DONE);
   
   for (n = 1; n < num_players; ++n)
   {
      order[n] = n;
      for (m = n - 1; m >= 0; --m)
      {
         if (scoreboard[n][TOTAL] > scoreboard[order[m]][TOTAL] ||
             (scoreboard[n][TOTAL] == scoreboard[order[m]][TOTAL] &&
              !rules->labels[SAVED_ROLLS]) ||
             (scoreboard[n][TOTAL] == scoreboard[order[m]][TOTAL] &&
              rules->labels[SAVED_ROLLS] &&
              scoreboard[n][SAVED_ROLLS] >= scoreboard[order[m]][SAVED_ROLLS]))
         {
            order[m + 1] = order[m];
            order[m] = n;
         }
      }
   }
   draw_player_numbers (main_win->RPort, num_players, order[0]);
   draw_chosen_score (main_win->RPort, rules,
                      order[0], scoreboard[order[0]], TRUE);
   
   for (n = 0; n < num_players; ++n)
   {
      if (update_high_score (game_number (rules),
                             order[n], scoreboard[order[n]][TOTAL],
                             scoreboard[order[n]][SAVED_ROLLS]))
      {
         new_high = TRUE;
      }
   }
   if (new_high)
      display_high_scores (game_number (rules));
}


BOOL
new_game (void)
{
   register UBYTE   n, m;
   
   rolls_left = 3;
   current_player = 0;

   if (num_players != chosen_num_players ||
       rules != &game_definition[chosen_gametype][chosen_scoring])
   {
      rules = &game_definition[chosen_gametype][chosen_scoring];
      num_players = chosen_num_players;
      if (!layout_display (main_win->WScreen, &main_win, rules, num_players))
         return FALSE;
   }
   else
   {
      for (n = 0; n < rules->num_dice; ++n)
         OffGadget (&die_gads[n], main_win, NULL);
      GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                         GA_Disabled, FALSE, TAG_DONE);
      GT_SetGadgetAttrs (disp_gad, main_win, NULL,
                         GTNM_Number, rolls_left, TAG_DONE);
   }
   
   rounds_left = 0;
   for (n = 0; n < MAX_LABELS; ++n)
   {
      if (rules->labels[n] && n != UPPER_TOTAL && n != BONUS && n != TOTAL &&
          n != SAVED_ROLLS)
      {
         ++rounds_left;
      }
   }
   
   for (n = 0; n < num_players; ++n)
   {
      for (m = 0; m < MAX_LABELS; ++m)
      {
         if (m == UPPER_TOTAL || m == BONUS || m == TOTAL || m == SAVED_ROLLS)
            scoreboard[n][m] = 0;
         else
            scoreboard[n][m] = -1;
      }
   }
   
   reset_scoreboard (main_win->RPort, rules, num_players);
   draw_player_numbers (main_win->RPort, num_players, current_player);
   
   return TRUE;
}

void
roll_dice (void)
{
   register UBYTE   n;
   UWORD   pos;
   BOOL   die_rolled = FALSE;
   
   for (n = 0; n < rules->num_dice; ++n)
   {
      if (die_gads[n].Flags & (GFLG_DISABLED | GFLG_SELECTED))
      {
         die_values[n] = drand48() * 6 + 1;
         pos = RemoveGadget (main_win, &die_gads[n]);
         die_gads[n].GadgetRender = &die_images[die_values[n]];
         die_gads[n].Flags &= ~(GFLG_SELECTED | GFLG_DISABLED);
         AddGadget (main_win, &die_gads[n], pos);
         RefreshGList (&die_gads[n], main_win, NULL, 1);
         die_rolled = TRUE;
      }
   }
   
   if (die_rolled)
   {
      if (rolls_left == 0)
      {
         if (--scoreboard[current_player][SAVED_ROLLS] == 0)
         {
            GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                               GA_Disabled, TRUE, TAG_DONE);
         }
      }
      else if (--rolls_left == 0)
      {
         if (!(rules->labels[SAVED_ROLLS] &&
             scoreboard[current_player][SAVED_ROLLS] > 0))
         {
            GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                               GA_Disabled, TRUE, TAG_DONE);
         }
      }
      GT_SetGadgetAttrs (disp_gad, main_win, NULL,
                         GTNM_Number, rolls_left, TAG_DONE);
      (rules->calc_score) (die_values, possible_score);
      draw_possible_score (main_win->RPort, rules, current_player,
                           scoreboard[current_player], possible_score);
   }
}

UBYTE
choose_score (
   WORD   x,
   WORD   y)
{
   register UBYTE   n;
   BYTE   score_no = -1;
   
   if (die_gads[0].Flags & GFLG_DISABLED)
      return 0;
   
   for (n = 0; score_no < 0 && n < MAX_LABELS; ++n)
   {
      if (rules->labels[n] &&
          x >= board_grid[current_player + 1][n].left &&
          x < board_grid[current_player + 1][n].left +
              board_grid[current_player + 1][n].width &&
          y >= board_grid[current_player + 1][n].top &&
          y < board_grid[current_player + 1][n].top +
              board_grid[current_player + 1][n].height)
      {
         score_no = n;
      }
   }
   
   if (scoreboard[current_player][score_no] == -1)
   {
      prev_rolls_left = rolls_left;
      last_choice = score_no;
      scoreboard[current_player][score_no] = possible_score[score_no];
      scoreboard[current_player][TOTAL] += possible_score[score_no];
      if (score_no < UPPER_TOTAL)
      {
         scoreboard[current_player][UPPER_TOTAL] +=
                                                   possible_score[score_no];
         if (scoreboard[current_player][UPPER_TOTAL] >= rules->bonus_limit &&
             scoreboard[current_player][BONUS] == 0)
         {
            scoreboard[current_player][BONUS] = rules->bonus;
            scoreboard[current_player][TOTAL] += rules->bonus;
         }
      }
      if (rules->labels[SAVED_ROLLS])
         scoreboard[current_player][SAVED_ROLLS] += rolls_left;
      
      draw_chosen_score (main_win->RPort, rules,
                         current_player, scoreboard[current_player], FALSE);
      for (n = 0; n < rules->num_dice; ++n)
         OffGadget (&die_gads[n], main_win, NULL);
      
      rolls_left = 3;
      ++current_player;
      if (current_player == num_players)
      {
         current_player = 0;
         --rounds_left;
      }
      if (current_player == 0 && rounds_left == 0)
         end_of_game ();
      else
      {
         draw_player_numbers (main_win->RPort, num_players, current_player);
         GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                            GA_Disabled, FALSE, TAG_DONE);
         GT_SetGadgetAttrs (disp_gad, main_win, NULL,
                            GTNM_Number, rolls_left);
      }
   }
   
   return rounds_left;
}

void
undo_last_choice (void)
{
   register UBYTE   n;
   
   if (current_player == 0)
   {
      ++rounds_left;
      current_player = num_players - 1;
   }
   else
      --current_player;
   
   rolls_left = prev_rolls_left;
   if (rules->labels[SAVED_ROLLS])
      scoreboard[current_player][SAVED_ROLLS] -= rolls_left;
   
   scoreboard[current_player][last_choice] = -1;
   scoreboard[current_player][TOTAL] -= possible_score[last_choice];
   if (last_choice < UPPER_TOTAL)
   {
      scoreboard[current_player][UPPER_TOTAL] -= possible_score[last_choice];
      if (scoreboard[current_player][UPPER_TOTAL] < rules->bonus_limit &&
          scoreboard[current_player][BONUS] > 0)
      {
         scoreboard[current_player][TOTAL] -= rules->bonus;
         scoreboard[current_player][BONUS] = 0;
      }
   }
   
   draw_player_numbers (main_win->RPort, num_players, current_player);
   draw_chosen_score (main_win->RPort, rules,
                      current_player, scoreboard[current_player], FALSE);
   draw_possible_score (main_win->RPort, rules, current_player,
                        scoreboard[current_player], possible_score);
   
   for (n = 0; n < rules->num_dice; ++n)
      OnGadget (&die_gads[n], main_win, NULL);
   if (!(rolls_left > 0 ||
       (rules->labels[SAVED_ROLLS] &&
        scoreboard[current_player][SAVED_ROLLS] > 0)))
   {
      GT_SetGadgetAttrs (roll_gad, main_win, NULL,
                         GA_Disabled, TRUE, TAG_DONE);
   }
   GT_SetGadgetAttrs (disp_gad, main_win, NULL,
                     GTNM_Number, rolls_left, TAG_DONE);
}
