/*
 * draw.c
 * ======
 * Functions for drawing the graphics.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "layout_const.h"
#include "layout.h"
#include "draw.h"

#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/alib.h>

extern struct GfxBase * GfxBase;

static void
Line (
   struct RastPort  *rp,
   WORD              x1,
   WORD              y1,
   WORD              x2,
   WORD              y2)
{
   Move (rp, x1, y1);
   Draw (rp, x2, y2);
}

static void
draw_large_ellipse (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   BOOL              recessed)
{
   if (recessed)
      SetAPen (rp, gui_pens[SHADOWPEN]);
   else
      SetAPen (rp, gui_pens[SHINEPEN]);
   Line (rp, x + 12, y + 1, x + 11, y + 1);
   Line (rp, x + 10, y, x + 6, y);
   Line (rp, x + 5, y + 1, x + 4, y + 1);
   Line (rp, x + 3, y + 2, x + 2, y + 2);
   Line (rp, x + 1, y + 3, x + 1, y + 4);
   Line (rp, x, y + 5, x, y + 9);
   Line (rp, x + 1, y + 10, x + 1, y + 11);
   if (recessed)
      SetAPen (rp, gui_pens[SHINEPEN]);
   else
      SetAPen (rp, gui_pens[SHADOWPEN]);
   Line (rp, x + 4, y + 13, x + 5, y + 13);
   Line (rp, x + 6, y + 14, x + 10, y + 14);
   Line (rp, x + 11, y + 13, x + 12, y + 13);
   Line (rp, x + 13, y + 12, x + 14, y + 12);
   Line (rp, x + 15, y + 11, x + 15, y + 10);
   Line (rp, x + 16, y + 9, x + 16, y + 5);
   Line (rp, x + 15, y + 4, x + 15, y + 3);
   SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   Line (rp, x + 14, y + 2, x + 13, y + 2);
   Line (rp, x + 2, y + 12, x + 3, y + 12);
}

static void
draw_small_ellipse (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   BOOL              recessed)
{
   if (recessed)
      SetAPen (rp, gui_pens[SHADOWPEN]);
   else
      SetAPen (rp, gui_pens[SHINEPEN]);
   Line (rp, x + 4, y, x + 2, y);
   WritePixel (rp, x + 1, y + 1);
   Line (rp, x, y + 2, x, y + 4);
   if (recessed)
      SetAPen (rp, gui_pens[SHINEPEN]);
   else
      SetAPen (rp, gui_pens[SHADOWPEN]);
   Line (rp, x + 2, y + 6, x + 4, y + 6);
   WritePixel (rp, x + 5, y + 5);
   Line (rp, x + 6, y + 4, x + 6, y + 2);
   SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   WritePixel (rp, x + 5, y + 1);
   WritePixel (rp, x + 1, y + 5);
}

static void
draw_large_brick (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   BYTE              color)
{
   char   num_str[2];
   
   if (color == NOPEN)
      SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   else if (color_display)
      SetAPen (rp, game_pens[color]);
   else
      SetAPen (rp, gui_pens[FILLPEN]);
   Line (rp, x + 6, y + 1, x + 10, y + 1);
   Line (rp, x + 4, y + 2, x + 12, y + 2);
   Line (rp, x + 4, y + 12, x + 12, y + 12);
   Line (rp, x + 6, y + 13, x + 10, y + 13);
   Line (rp, x + 1, y + 5, x + 1, y + 9);
   Line (rp, x + 15, y + 5, x + 15, y + 9);
   RectFill (rp, x + 2, y + 3, x + 14, y + 11);
   draw_large_ellipse (rp, x, y, color == NOPEN);
   if (!color_display && color != NOPEN)
   {
      SetAPen (rp, gui_pens[FILLTEXTPEN]);
      SetBPen (rp, gui_pens[FILLPEN]);
      sprintf (num_str, "%d", color + 1);
      Move (rp, x + (BRICK1_W - rp->TxWidth) / 2,
              y + (BRICK1_H - rp->TxBaseline) / 2 + rp->TxBaseline);
      Text (rp, num_str, 1);
   }
}

static void
draw_small_brick (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   BYTE              color)
{
   if (color == NOPEN)
      SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   else if (color_display)
      SetAPen (rp, game_pens[color]);
   else if (color == WHITEPEN)
   {
      if (gui_pens[HIGHLIGHTTEXTPEN] != gui_pens[TEXTPEN])
         SetAPen (rp, gui_pens[HIGHLIGHTTEXTPEN]);
      else
         SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   }
   else
      SetAPen (rp, gui_pens[TEXTPEN]);
   Line (rp, x + 2, y + 1, x + 4, y + 1);
   Line (rp, x + 2, y + 5, x + 4, y + 5);
   RectFill (rp, x + 1, y + 2, x + 5, y + 4);
   draw_small_ellipse (rp, x, y, color == NOPEN);
}


void
place_large_brick (
   struct Window  *win,
   UBYTE           row,
   UBYTE           hole,
   BYTE            color)
{
   draw_large_brick (win->RPort,
                     playfield_grid[row][0].left + LINEWIDTH + INTERWIDTH / 2,
                     playfield_grid[row][0].top + LINEHEIGHT +
                     INTERHEIGHT / 2 + hole * (BRICK1_H + INTERHEIGHT / 2),
                     color);
}

void
place_small_brick (
   struct Window  *win,
   UBYTE           row,
   UBYTE           hole,
   BYTE            color)
{
   draw_small_brick (win->RPort,
                     playfield_grid[row][1].left + LINEWIDTH + INTERWIDTH / 2 +
                     (hole > 1) * (BRICK1_W - BRICK2_W),
                     playfield_grid[row][1].top + LINEHEIGHT +
                     INTERHEIGHT / 2 +
                     (hole & 1) * (BRICK2_H + INTERHEIGHT / 2),
                     color);
}

void
draw_solution (
   struct Window  *win,
   APTR            vis_info,
   BYTE            solution[],
   BOOL            show_it)
{
   register UBYTE   j;
   
   DrawBevelBox (win->RPort,
                 playfield_grid[11][0].left, playfield_grid[11][0].top,
                 playfield_grid[11][0].width, playfield_grid[11][0].height,
                 GT_VisualInfo, vis_info,
                 (show_it ? GTBB_Recessed : TAG_IGNORE), TRUE,
                 TAG_END);
   if (show_it)
   {
      for (j = 0; j < 4; ++j)
         place_large_brick (win, 11, j, solution[j]);
   }
   else
   {
      SetAPen (win->RPort, gui_pens[BACKGROUNDPEN]);
      RectFill (win->RPort, playfield_grid[11][0].left + LINEWIDTH,
                playfield_grid[11][0].top + LINEHEIGHT,
                playfield_grid[11][0].left + playfield_grid[11][0].width -
                LINEWIDTH - 1,
                playfield_grid[11][0].top + playfield_grid[11][0].height -
                LINEHEIGHT - 1);
   }
}

void
draw_playfield (
   struct Window  *win,
   APTR            vis_info,
   BYTE            guess[][4],
   BYTE            answer[][4],
   BOOL            show_solution)
{
   UBYTE   n, i, j;
   
   DrawBevelBox (win->RPort,
                 playfield_grid[0][0].left, playfield_grid[0][0].top,
                 playfield_grid[0][0].width, playfield_grid[0][0].height,
                 GT_VisualInfo, vis_info, TAG_END);
   for (n = 1; n <= 10; ++n)
   {
      for (i = 0; i < 2; ++i)
      {
         DrawBevelBox (win->RPort,
                       playfield_grid[n][i].left, playfield_grid[n][i].top,
                       playfield_grid[n][i].width, playfield_grid[n][i].height,
                       GT_VisualInfo, vis_info, GTBB_Recessed, TRUE, TAG_END);
         for (j = 0; j < 4; ++j)
         {
            if (i == 0)
               place_large_brick (win, n, j, guess[n - 1][j]);
            else
               place_small_brick (win, n, j, answer[n - 1][j]);
         }
      }
   }
   draw_solution (win, vis_info, guess[n - 1], show_solution);
}

void
draw_panel (
   struct Window  *win,
   UBYTE           num_colors,
   UBYTE           chosen_num_colors,
   BYTE            selection)
{
   UBYTE   n;
   
   if (num_colors > chosen_num_colors)
   {
      SetAPen (win->RPort, gui_pens[BACKGROUNDPEN]);
      RectFill (win->RPort, playfield_grid[0][1].left +
                chosen_num_colors * (BRICK1_W + INTERWIDTH),
                playfield_grid[0][1].top,
                playfield_grid[0][1].left +
                num_colors * (BRICK1_W + INTERWIDTH) - INTERWIDTH - 1,
                playfield_grid[0][1].top + playfield_grid[0][1].height - 1);
   }
   for (n = 0; n < chosen_num_colors; ++n)
   {
      draw_large_brick (win->RPort, playfield_grid[0][1].left +
                        n * (BRICK1_W + INTERWIDTH),
                        playfield_grid[0][1].top +
                        (playfield_grid[0][1].height - BRICK1_H) / 2,
                        ((n == selection) ? NOPEN : n));
   }
}

void
select_panel (
   struct Window  *win,
   BYTE           selection)
{
   static BYTE   last = NOPEN;
   
   if (last != NOPEN && last != selection)
   {
      draw_large_brick (win->RPort, playfield_grid[0][1].left +
                        last * (BRICK1_W + INTERWIDTH),
                        playfield_grid[0][1].top +
                        (playfield_grid[0][1].height - BRICK1_H) / 2, last);
   }
   last = selection;
   if (selection != NOPEN)
   {
      draw_large_brick (win->RPort, playfield_grid[0][1].left +
                        selection * (BRICK1_W + INTERWIDTH),
                        playfield_grid[0][1].top +
                        (playfield_grid[0][1].height - BRICK1_H) / 2,
                        NOPEN);
   }
}

void
delete_gadgets (
   struct RastPort  *rp,
   struct Gadget    *gad)
{
   SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   
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
