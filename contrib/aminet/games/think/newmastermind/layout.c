/*
 * layout.c
 * ========
 * Handles layout of display.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <string.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include "localize.h"
#include "layout_const.h"
#include "layout.h"

#include <proto/graphics.h>


#define MAX(a, b) (((a) > (b)) ? (a) : (b))


extern struct GfxBase  *GfxBase;

BOOL    color_display = TRUE;
WORD    game_pens[NUM_GAMEPENS];
UWORD  *gui_pens;

static ULONG   game_colors[NUM_GAMEPENS][3] = {
   { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
   { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
   { 0xFFFFFFFF, 0x88888888, 0x00000000 },
   { 0xFFFFFFFF, 0x00000000, 0x00000000 },
   { 0x00000000, 0xDDDDDDDD, 0x00000000 },
   { 0x00000000, 0xAAAAAAAA, 0xFFFFFFFF },
   { 0x88888888, 0x00000000, 0xFFFFFFFF },
   { 0x00000000, 0x00000000, 0x00000000 }
};

struct box playfield_grid[12][2];


void
init_pens (
   struct Screen  *scr)
{
   register UBYTE   n;
   register BYTE    m;
   
   for (n = 0; n < NUM_GAMEPENS; ++n)
   {
      if (GfxBase->LibNode.lib_Version >= 39L)
      {
         game_pens[n] = ObtainBestPen (scr->ViewPort.ColorMap,
                                       game_colors[n][0],
                                       game_colors[n][1],
                                       game_colors[n][2],
                                       OBP_Precision, PRECISION_GUI,
                                       TAG_DONE);
         for (m = n - 1; m >= 0 && game_pens[n] != NOPEN; --m)
         {
            if (game_pens[n] == game_pens[m])
            {
               ReleasePen (scr->ViewPort.ColorMap, game_pens[n]);
               game_pens[n] = NOPEN;
            }
         }
      }
      else
         game_pens[n] = NOPEN;
   }
   
   for (n = 0; n < NUM_GAMEPENS && color_display; ++n)
      if (game_pens[n] == NOPEN)
         color_display = FALSE;
   
   if (!color_display)
      free_pens (scr);
}

void
free_pens (
   struct Screen  *scr)
{
   register UBYTE   n;
   
   for (n = 0; n < NUM_GAMEPENS; ++n)
      if (game_pens[n] != NOPEN)
         ReleasePen (scr->ViewPort.ColorMap, game_pens[n]);
}

void
gadget_extent (
   struct Screen  *scr,
   UWORD          *width,
   UWORD          *height)
{
   char  *label;
   
   label = localized_string (MSG_OK_GAD);
   *width = 2 * LINEWIDTH + INTERWIDTH +
            TextLength (&scr->RastPort, label, strlen (label));
   *height = 2 * LINEHEIGHT + INTERHEIGHT + scr->Font->ta_YSize;
}

BOOL
window_extent (
   struct Screen  *scr,
   UBYTE           num_colors,
   UWORD          *width,
   UWORD          *height)
{
   UWORD   temp;
   
   gadget_extent (scr, &temp, height);
   
   *width = 11 * (BRICK1_W + INTERWIDTH + 2 * LINEWIDTH) +
            9 * (INTERWIDTH / 2) + 3 * INTERWIDTH + 2 * LINEWIDTH;
   temp += num_colors * (BRICK1_W + INTERWIDTH);
   *width = MAX (*width, temp) +
            scr->WBorLeft + scr->WBorRight + 2 * INTERWIDTH;
   
   *height = playfield_grid[0][1].height = MAX (*height, BRICK1_H);
   *height += (2 * BRICK2_H + 3 * (INTERHEIGHT / 2) + 2 * LINEHEIGHT) +
              (4 * BRICK1_H + 5 * (INTERHEIGHT / 2) + 2 * LINEHEIGHT) +
              (INTERHEIGHT / 2) + 5 * INTERHEIGHT + 2 * LINEHEIGHT +
              scr->WBorTop + scr->Font->ta_YSize + scr->WBorBottom + 1;
   
   return (BOOL)(*width <= scr->Width && *height <= scr->Height);
}

void
init_playfield_grid (
   struct Window  *win)
{
   UBYTE   n;
   
   playfield_grid[0][0].left = win->BorderLeft + INTERWIDTH;
   playfield_grid[0][0].top = win->BorderTop + INTERHEIGHT;
   playfield_grid[0][0].width = 11 * (BRICK1_W + INTERWIDTH + 2 * LINEWIDTH) +
                                9 * (INTERWIDTH / 2) +
                                3 * INTERWIDTH + 2 * LINEWIDTH;
   playfield_grid[0][0].height = (2 * BRICK2_H + 3 * (INTERHEIGHT / 2) +
                                 2 * LINEHEIGHT) + (4 * BRICK1_H +
                                 5 * (INTERHEIGHT / 2) + 2 * LINEHEIGHT) +
                                 (INTERHEIGHT / 2) + 2 * INTERHEIGHT +
                                 2 * LINEHEIGHT;
   playfield_grid[0][1].left = playfield_grid[0][0].left;
   playfield_grid[0][1].top = playfield_grid[0][0].top +
                              playfield_grid[0][0].height + INTERHEIGHT;
   
   playfield_grid[1][1].left = playfield_grid[0][0].left +
                               LINEWIDTH + INTERWIDTH;
   playfield_grid[1][1].top = playfield_grid[0][0].top +
                              LINEHEIGHT + INTERHEIGHT;
   playfield_grid[1][1].width = BRICK1_W + INTERWIDTH + 2 * LINEWIDTH;
   playfield_grid[1][1].height = 2 * BRICK2_H + 3 * (INTERHEIGHT / 2) +
                                 2 * LINEHEIGHT;
   playfield_grid[1][0].left = playfield_grid[1][1].left;
   playfield_grid[1][0].top = playfield_grid[1][1].top +
                              playfield_grid[1][1].height + INTERHEIGHT / 2;
   playfield_grid[1][0].width = playfield_grid[1][1].width;
   playfield_grid[1][0].height = 4 * BRICK1_H + 5 * (INTERHEIGHT / 2) +
                                 2 * LINEHEIGHT;
   for (n = 2; n <= 10; ++n)
   {
      playfield_grid[n][1].left = playfield_grid[n - 1][1].left +
                                  playfield_grid[n - 1][1].width +
                                  INTERWIDTH / 2;
      playfield_grid[n][1].top = playfield_grid[n - 1][1].top;
      playfield_grid[n][1].width = playfield_grid[n - 1][1].width;
      playfield_grid[n][1].height = playfield_grid[n - 1][1].height;
      playfield_grid[n][0].left = playfield_grid[n][1].left;
      playfield_grid[n][0].top = playfield_grid[n - 1][0].top;
      playfield_grid[n][0].width = playfield_grid[n - 1][0].width;
      playfield_grid[n][0].height = playfield_grid[n - 1][0].height;
   }
   playfield_grid[n][0].left = playfield_grid[n - 1][0].left +
                               playfield_grid[n - 1][0].width +
                               INTERWIDTH;
   playfield_grid[n][0].top = playfield_grid[n - 1][0].top;
   playfield_grid[n][0].width = playfield_grid[n - 1][0].width;
   playfield_grid[n][0].height = playfield_grid[n - 1][0].height;
}
