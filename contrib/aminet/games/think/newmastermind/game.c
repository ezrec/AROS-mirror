/*
 * game.c
 * ======
 * Implementation of game procedures.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <math.h>
#include <stdlib.h>
#include <exec/types.h>
#include <intuition/gadgetclass.h>
#include "requesters.h"
#include "layout_const.h"
#include "layout.h"
#include "draw.h"
#include "game.h"

#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/alib.h>

extern struct Library * GadToolsBase;

UBYTE   current_row;
BYTE   active_color;
BYTE   guess[11][4];
BYTE   answer[10][4];

static BYTE    active_hole;
static UBYTE   filled_holes;


static BOOL
correct_row (
   UBYTE   row)
{
   register UBYTE   n, m;
   UBYTE   hole_mask1 = 0, hole_mask2, next_hole = 0;
   BOOL ret_val;
   
   for (n = 0; n < 4; ++n)
   {
      if (guess[row][n] == guess[10][n])
      {
         answer[row][(correction == ADULTS) ? next_hole : n] = BLACKPEN;
         ++next_hole;
         hole_mask1 |= 1 << n;
      }
   }
   if (!(ret_val = hole_mask1 == 15))
   {
      hole_mask2 = hole_mask1;
      for (n = 0; n < 4; ++n)
      {
         if (!(hole_mask1 & 1 << n))
         {
            for (m = 0; m < 4; ++m)
            {
               if (m != n && !(hole_mask2 & 1 << m) &&
                   guess[row][n] == guess[10][m])
               {
                  answer[row][(correction == ADULTS) ? next_hole : n] =
                                                                     WHITEPEN;
                  ++next_hole;
                  hole_mask1 |= 1 << n;
                  hole_mask2 |= 1 << m;
                  m = 3;
               }
            }
         }
      }
   }
   for (n = 0; n < 4; ++n)
      place_small_brick (main_win, row + 1, n, answer[row][n]);
   
   return ret_val;
}
BOOL
new_game (void)
{
   register UBYTE   n, m;
   
   GT_SetGadgetAttrs (ok_gad, main_win, NULL,
                      GA_Disabled, TRUE, TAG_DONE);

   if (num_colors != chosen_num_colors)
   {
      delete_gadgets (main_win->RPort, gt_gads);
      draw_panel (main_win, num_colors, chosen_num_colors, NOPEN);
      RemoveGList (main_win, gt_gads, -1);
      FreeGadgets (gt_gads);
      new_ok_gad.ng_LeftEdge = playfield_grid[0][1].left +
                               chosen_num_colors * (BRICK1_W + INTERWIDTH);
      
      ok_gad = CreateContext (&gt_gads);
      ok_gad = CreateGadget (BUTTON_KIND, ok_gad, &new_ok_gad,
                             GA_Disabled, TRUE, TAG_DONE);
      if (ok_gad)
      {
         AddGList (main_win, gt_gads, -1, -1, NULL);
         RefreshGadgets (gt_gads, main_win, NULL);
         GT_RefreshWindow (main_win, NULL);
      }
      else
      {
         msg_requester (NULL, "Init Error", "OK", "Couldn't create gadgets!");
         
         return FALSE;
      }
      num_colors = chosen_num_colors;
   }
   opponent = chosen_opponent;
   correction = chosen_correction;
   
   current_row = (opponent == HUMAN) ? 11 : 1;
   for (n = 0; n < 11; ++n)
   {
      for (m = 0; m < 4; ++m)
      {
         guess[n][m] = NOPEN;
         if (n < 10)
            answer[n][m] = NOPEN;
      }
   }
   
   if (opponent == COMPUTER)
      for (n = 0; n < 4; ++n)
         guess[10][n] = num_colors * drand48 ();
   
   filled_holes = 0;
   active_hole = -1;
   active_color = NOPEN;
   select_panel (main_win, active_color);
   draw_playfield (main_win, vis_info, guess, answer, opponent == HUMAN);
   
   return TRUE;
}

void
handle_mouseclick (
   UWORD   x,
   UWORD   y)
{
   BYTE   new_color;
   
   if (active_hole != -1 && active_color != NOPEN)
   {
      if (guess[current_row - 1][active_hole] == NOPEN)
      {
         ++filled_holes;
         if (filled_holes == 4)
         {
            GT_SetGadgetAttrs (ok_gad, main_win, NULL,
                               GA_Disabled, FALSE, TAG_DONE);
         }
      }
      guess[current_row - 1][active_hole] = active_color;
   }   
   else if (x >= playfield_grid[0][1].left && y >= playfield_grid[0][1].top &&
            x < playfield_grid[0][1].left +
                num_colors * (BRICK1_W + INTERWIDTH) &&
            y < playfield_grid[0][1].top + playfield_grid[0][1].height)
   {
      new_color = (x - playfield_grid[0][1].left) / (BRICK1_W + INTERWIDTH);
      if (x - new_color * (BRICK1_W + INTERWIDTH) <
          BRICK1_W + playfield_grid[0][1].left)
      {
         active_color = new_color;
      }
      select_panel (main_win, active_color);
   }
}

void
handle_mousemove (
   UWORD   x,
   UWORD   y)
{
   BYTE   new_hole;
   
   if (x >= playfield_grid[current_row][0].left + LINEWIDTH &&
       y >= playfield_grid[current_row][0].top + LINEHEIGHT &&
       x < playfield_grid[current_row][0].left +
           playfield_grid[current_row][0].width - LINEWIDTH &&
       y < playfield_grid[current_row][0].top +
           playfield_grid[current_row][0].height -
           LINEHEIGHT - INTERHEIGHT / 2)
   {
      new_hole = (y - playfield_grid[current_row][0].top - LINEHEIGHT) /
                 (BRICK1_H + INTERHEIGHT / 2);
   }
   else
      new_hole = -1;
   
   if (new_hole != active_hole)
   {
      place_large_brick (main_win, current_row, active_hole,
                         guess[current_row - 1][active_hole]);
      place_large_brick (main_win, current_row, new_hole, active_color);
      active_hole = new_hole;
   }
}

void
end_of_turn (void)
{
   GT_SetGadgetAttrs (ok_gad, main_win, NULL,
                      GA_Disabled, TRUE, TAG_DONE);
   active_color = NOPEN;
   select_panel (main_win, active_color);
   active_hole = NOPEN;
   filled_holes = 0;
   if (current_row == 11)
   {
      current_row = 1;
      draw_solution (main_win, vis_info, guess[10], FALSE);
   }
   else if (correct_row (current_row - 1) || ++current_row > 10)
   {
      current_row = 0;
      draw_solution (main_win, vis_info, guess[10], TRUE);
   }
}
