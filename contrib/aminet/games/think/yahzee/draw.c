/*
 * draw.c
 * ======
 * Functions for drawing the scoreboard.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <graphics/rastport.h>
#include <intuition/screens.h>
#include "localize.h"
#include "rules.h"
#include "layout_const.h"
#include "draw.h"
#include <libraries/gadtools.h>

#include <proto/graphics.h>

extern struct GfxBase * GfxBase;

#define NUM_BOARDPENS   (BOARD_FILLPEN + 1)

UWORD   board_pens[NUM_BOARDPENS] = { 0, 0, 1, 1, 2, 3 };
BOOL    locked_pens[NUM_BOARDPENS] = { 0, 0, 0, 0, 0, 0 };
static const UBYTE   default_pens[NUM_BOARDPENS] = {
   BACKGROUNDPEN, BACKGROUNDPEN, SHADOWPEN, TEXTPEN, HIGHLIGHTTEXTPEN, FILLPEN
};

struct box   upper_board, lower_board;
struct box   board_grid[MAX_PLAYERS + 1][MAX_LABELS];


static void
draw_line (
   struct RastPort  *rp,
   WORD              x_pos,
   WORD              y_pos,
   UWORD             x_ext,
   UWORD             y_ext)
{
   Move (rp, x_pos, y_pos);
   Draw (rp, x_pos + x_ext - 1, y_pos + y_ext - 1);
}


void
init_board_pens (
   UWORD  *gui_pens)
{
   register UBYTE   n;
   
   if (gui_pens != NULL)
      for (n = 0; n < NUM_BOARDPENS; ++n)
         if (!locked_pens[n])
            board_pens[n] = gui_pens[default_pens[n]];
}

void
delete_gadgets (
   struct RastPort  *rp,
   struct Gadget    *gad)
{
   SetAPen (rp, board_pens[BOARD_BACKGROUNDPEN]);
   
   while (gad != NULL)
   {
      if (gad->Width != 0 && gad->Height != 0)
      {
         RectFill (rp, gad->LeftEdge, gad->TopEdge,
                   gad->LeftEdge + gad->Width - 1,
                   gad->TopEdge + gad->Height - 1);
      }
      gad = gad->NextGadget;
   }
}

void
draw_scoreboard (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             num_players)
{
   register UBYTE   n;
   register WORD    coord;
   struct box  *board_box;
   char        *label_str;
   UBYTE   limit;
   
   limit = num_upper_labels (rules);
   board_box = &upper_board;
   while (board_box != NULL)
   {
      /* draw background */
      SetAPen (rp, board_pens[BOARD_PAPERPEN]);
      RectFill (rp,
                board_box->left, board_box->top,
                board_box->left + board_box->width - 1,
                board_box->top + board_box->height - 1);
      
      /* draw horizontal lines and labels */
      SetAPen (rp, board_pens[BOARD_LINEPEN]);
      coord = board_box->top;
      if (!(board_box == &lower_board && lower_board.left == upper_board.left))
      {
         while (coord < board_box->top + LINEHEIGHT)
         {
            draw_line (rp, board_box->left, coord, board_box->width, 1);
            ++coord;
         }
         coord += board_grid[0][0].height;
      }
      for (n = ((board_box == &upper_board) ? 0 : limit);
           n < ((board_box == &upper_board) ? limit : MAX_LABELS);
           ++n)
      {
         if (!rules->labels[n])
            ++limit;
         else
         {
            if (n == UPPER_TOTAL || n == BONUS ||
                n == TOTAL || n == SAVED_ROLLS)
            {
               SetAPen (rp, board_pens[BOARD_HIGHLIGHTPEN]);
            }
            else
               SetAPen (rp, board_pens[BOARD_TEXTPEN]);
            Move (rp,
                  board_grid[0][n].left + INTERWIDTH / 4,
                  board_grid[0][n].top + rp->TxBaseline +
                  (board_grid[0][n].height - rp->TxHeight) / 2);
            label_str = localized_string ((n == YAHZEE) ?
                                          rules->yahzee_label :
                                          label_msg[n]);
            Text (rp, label_str, strlen (label_str));
   
            SetAPen (rp, board_pens[BOARD_LINEPEN]);
            while (coord < board_grid[0][n].top)
            {
               draw_line (rp, board_box->left, coord, board_box->width, 1);
               ++coord;
            }
            coord += board_grid[0][n].height;
         }
      }
      while (coord < board_box->top + board_box->height)
      {
         draw_line (rp, board_box->left, coord, board_box->width, 1);
         ++coord;
      }
      
      /* draw vertical lines */
      coord = board_box->left;
      for (n = 0; n <= num_players; ++n)
      {
         while (coord <
                board_grid[n][(board_box == &upper_board) ? 0 : TOTAL].left)
         {
            draw_line (rp, coord, board_box->top, 1, board_box->height);
            ++coord;
         }
         coord += board_grid[n][(board_box == &upper_board) ? 0 : TOTAL].width;
      }
      while (coord < board_box->left + board_box->width)
      {
         draw_line (rp, coord, board_box->top, 1, board_box->height);
         ++coord;
      }
      
      board_box = (board_box == &upper_board) ? &lower_board : NULL;
   }
}

void
delete_scoreboard (
   struct RastPort  *rp)
{
   SetAPen (rp, board_pens[BOARD_BACKGROUNDPEN]);
   RectFill (rp,
             upper_board.left, upper_board.top,
             upper_board.left + upper_board.width - 1,
             upper_board.top + upper_board.height - 1);
   RectFill (rp,
             lower_board.left, lower_board.top,
             lower_board.left + lower_board.width - 1,
             lower_board.top + lower_board.height - 1);
}

void
reset_scoreboard (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             num_players)
{
   register UBYTE   i, j;
   UBYTE   limit;
   
   limit = num_upper_labels (rules);
   /*delete old scores */
   SetAPen (rp, board_pens[BOARD_PAPERPEN]);
   for (i = 1; i <= num_players; ++i)
   {
      for (j = 0; j < MAX_LABELS; ++j)
      {
         if (rules->labels[j])
         {
            RectFill (rp,
                      board_grid[i][j].left, board_grid[i][j].top,
                      board_grid[i][j].left + board_grid[i][j].width - 1,
                      board_grid[i][j].top + board_grid[i][j].height - 1);
         }
      }
   }
   
   /* draw reset scores */
   SetAPen (rp, board_pens[BOARD_HIGHLIGHTPEN]);
   for (i = 1; i <= num_players; ++i)
   {
      for (j = UPPER_TOTAL; j < MAX_LABELS; ++j)
      {
         if (j == BONUS + 1)
            j = TOTAL;
         
         if (rules->labels[j])
         {
            Move (rp,
                  board_grid[i][j].left + INTERWIDTH / 4,
                  board_grid[i][j].top + rp->TxBaseline +
                  (board_grid[i][j].height - rp->TxHeight) / 2);
            Text (rp, "  0", 3);
         }
      }
   }
}

void
draw_player_numbers (
   struct RastPort  *rp,
   UBYTE             num_players,
   UBYTE             current_player)
{
   register UBYTE   n;
   char   buf[2];
   
   for (n = 1; n <= num_players; ++n)
   {
      sprintf (buf, "%d", n);
      
      if (n == current_player + 1)
         SetAPen (rp, board_pens[BOARD_HIGHLIGHTPEN]);
      else
         SetAPen (rp, board_pens[BOARD_TEXTPEN]);
      Move (rp,
            board_grid[n][0].left + (board_grid[n][0].width - rp->TxWidth) / 2,
            upper_board.top + LINEHEIGHT + rp->TxBaseline +
            (board_grid[n][0].height - rp->TxHeight) / 2);
      Text (rp, buf, strlen (buf));
      
      if (upper_board.left != lower_board.left)
      {
         Move (rp,
               board_grid[n][TOTAL].left +
               (board_grid[n][TOTAL].width - rp->TxWidth) / 2,
               lower_board.top + LINEHEIGHT + rp->TxBaseline +
               (board_grid[n][TOTAL].height - rp->TxHeight) / 2);
         Text (rp, buf, strlen (buf));
      }
   }
}

void
draw_possible_score (
   struct RastPort  *rp,
   struct rules     *rules,
   UWORD             current_player,
   WORD              scoreboard[],
   UBYTE             possible_score[])
{
   register UBYTE   n;
   char   buf[4];
   
   for (n = 0; n < MAX_LABELS; ++n)
   {
      if (rules->labels[n] && (scoreboard[n] == -1 || n == SAVED_ROLLS))
      {
         if (n == SAVED_ROLLS)
            sprintf (buf, "%d", scoreboard[n]);
         else
            sprintf (buf, "%d", possible_score[n]);
         
         SetAPen (rp, board_pens[(n == SAVED_ROLLS) ? BOARD_HIGHLIGHTPEN :
                                                      BOARD_FILLPEN]);
         Move (rp,
               board_grid[current_player + 1][n].left + INTERWIDTH / 4,
               board_grid[current_player + 1][n].top + rp->TxBaseline +
               (board_grid[current_player + 1][n].height - rp->TxHeight) / 2);
         Text (rp, "   ", 3 - strlen (buf));
         Text (rp, buf, strlen (buf));
      }
   }
}

void
draw_chosen_score (
   struct RastPort  *rp,
   struct rules     *rules,
   UBYTE             current_player,
   WORD              scoreboard[],
   BOOL              highlighted)
{
   register UBYTE   n;
   char   buf[4];
   
   for (n = 0; n < MAX_LABELS; ++n)
   {
      if (rules->labels[n])
      {
         if (scoreboard[n] == -1)
         {
            SetAPen (rp, board_pens[BOARD_PAPERPEN]);
            RectFill (rp,
                      board_grid[current_player + 1][n].left,
                      board_grid[current_player + 1][n].top,
                      board_grid[current_player + 1][n].left +
                      board_grid[current_player + 1][n].width - 1,
                      board_grid[current_player + 1][n].top +
                      board_grid[current_player + 1][n].height - 1);
         }
         else
         {
            sprintf (buf, "%d", scoreboard[n]);
            
            if (n == UPPER_TOTAL || n == BONUS ||
                n == TOTAL || n == SAVED_ROLLS || highlighted)
            {
               SetAPen (rp, board_pens[BOARD_HIGHLIGHTPEN]);
            }
            else
               SetAPen (rp, board_pens[BOARD_TEXTPEN]);
            
            Move (rp,
                board_grid[current_player + 1][n].left + INTERWIDTH / 4,
                board_grid[current_player + 1][n].top + rp->TxBaseline +
                (board_grid[current_player + 1][n].height - rp->TxHeight) / 2);
            Text (rp, "   ", 3 - strlen (buf));
            Text (rp, buf, strlen (buf));
         }
      }
   }
}
