/*
 * layout.c
 * ========
 * Handles layout of display.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <proto/graphics.h>
#include <libraries/gadtools.h>
#include <intuition/screens.h>

#include "game.h"
#include "display_globals.h"
#include "counter.h"
#include "layout.h"


#define max(a, b) (((a) > (b)) ? (a) : (b))


static void
field_extent (
   UBYTE   level,
   UBYTE   cell_space,
   UWORD   font_w,
   UWORD   font_h,
   UWORD  *used_w,
   UWORD  *used_h);


extern struct GfxBase  *GfxBase;

UBYTE   max_rows, max_columns;
UBYTE   cell_space;
UBYTE   cell_w, cell_h;
BOOL    digital_display;
BOOL    display_colors = TRUE;

LONG    game_pens[NUM_GAMEPENS];
UWORD  *gui_pens;

static ULONG   game_colors[NUM_GAMEPENS][3] = {
   { 0x00000000, 0x00000000, 0xA7A7A7A7 },
   { 0x00000000, 0xA1A1A1A1, 0x15151515 },
   { 0xE1E1E1E1, 0x00000000, 0x53535353 },
   { 0x00000000, 0x00000000, 0x78787878 },
   { 0x83838383, 0x00000000, 0x30303030 },
   { 0x00000000, 0xAAAAAAAA, 0xAAAAAAAA },
   { 0x00000000, 0x00000000, 0x00000000 },
   { 0x61616161, 0x61616161, 0x61616161 }
};


static void
field_extent (
   UBYTE   level,
   UBYTE   cell_space,
   UWORD   font_w,
   UWORD   font_h,
   UWORD  *used_w,
   UWORD  *used_h)
{
   UBYTE   cell_side;
   
   cell_side = max (font_w, font_h);
   cell_w = ((cell_space == NORMAL_SPACE) ? 4 : 2) * LINEWIDTH + cell_side;
   cell_h = ((cell_space == NORMAL_SPACE) ? 4 : 2) * LINEHEIGHT + cell_side;
   
   *used_w = levels[level].columns * cell_w + 2 * LINEWIDTH;
   *used_h = levels[level].rows * cell_h + 2 * LINEHEIGHT;
}

void
window_extent (
   struct Screen  *scr,
   UBYTE           level,
   UWORD           font_w,
   UWORD           font_h,
   UWORD          *used_w,
   UWORD          *used_h)
{
   field_extent (level, cell_space, font_w, font_h, used_w, used_h);
   if (digital_display)
   {
      UWORD   digital_w;
      
      digital_w = 2 * counter_width () + INTERWIDTH;
      *used_w = max (*used_w, digital_w);
      *used_h += counter_height () + INTERHEIGHT;
   }
   *used_w += 2 * (INTERWIDTH + LINEWIDTH) + scr->WBorLeft + scr->WBorRight;
   *used_h += 2 * (INTERHEIGHT + LINEHEIGHT) + scr->WBorTop + scr->WBorBottom +
              scr->Font->ta_YSize + 1;
   
   max_columns = (scr->Width - scr->WBorLeft - scr->WBorRight -
                  2 * (INTERWIDTH + INTERHEIGHT)) / cell_w;
   if (max_columns > MAX_COLUMNS)
      max_columns = MAX_COLUMNS;
   max_rows = (scr->Height - scr->WBorTop - scr->WBorBottom -
               scr->Font->ta_YSize - 2 * (INTERHEIGHT + LINEHEIGHT) -
               ((digital_display) ? counter_height () + INTERHEIGHT : 0)) / 
              cell_h;
   if (max_rows > MAX_ROWS)
      max_rows = MAX_ROWS;
}

void
init_pens (
   struct Screen  *scr)
{
   register UBYTE   i;
   
   for (i = 0; i < NUM_GAMEPENS; ++i)
   {
      if (GfxBase->LibNode.lib_Version >= 39L)
      {
         game_pens[i] = ObtainBestPen (scr->ViewPort.ColorMap,
                                       game_colors[i][0],
                                       game_colors[i][1],
                                       game_colors[i][2],
                                       OBP_Precision, PRECISION_IMAGE,
                                       TAG_DONE);
         if (game_pens[i] == gui_pens[BACKGROUNDPEN])
         {
            ReleasePen (scr->ViewPort.ColorMap, game_pens[i]);
            game_pens[i] = -1;
         }
      }
      else
         game_pens[i] = -1;
   }
}

void
free_pens (
   struct Screen  *scr)
{
   register UBYTE   i;
   
   for (i = 0; i < NUM_GAMEPENS; ++i)
   {
      if (GfxBase->LibNode.lib_Version >= 39L)
      {
         if (game_pens[i] != -1)
            ReleasePen (scr->ViewPort.ColorMap, game_pens[i]);
      }
   }
}

BOOL
layout_display (
   struct Screen  *scr,
   BOOL           *rom_font)
{
   UWORD   used_w, used_h;
   UWORD   field_w[4], field_h[4];
   register UBYTE   i;
   
   field_extent (EXPERT_LEVEL, NORMAL_SPACE, GfxBase->DefaultFont->tf_XSize,
                 GfxBase->DefaultFont->tf_YSize, &field_w[0], &field_h[0]);
   field_extent (EXPERT_LEVEL, NO_SPACE, GfxBase->DefaultFont->tf_XSize,
                 GfxBase->DefaultFont->tf_YSize, &field_w[1], &field_h[1]);
   field_extent (EXPERT_LEVEL, NORMAL_SPACE, ROMFONT_WIDTH, ROMFONT_HEIGHT,
                 &field_w[2], &field_h[2]);
   field_extent (EXPERT_LEVEL, NO_SPACE, ROMFONT_WIDTH, ROMFONT_HEIGHT,
                 &field_w[3], &field_h[3]);
   
   *rom_font = FALSE;
   digital_display = TRUE;
   for (i = 0; i < 4; ++i)
   {
      cell_space = i & NO_SPACE;
      if (i == 2)
         *rom_font = TRUE;
      
      if (*rom_font)
      {
         window_extent (scr, NOVICE_LEVEL, ROMFONT_WIDTH, ROMFONT_HEIGHT,
                        &zoom_bounds[2], &zoom_bounds[3]);
         window_extent (scr, EXPERT_LEVEL, ROMFONT_WIDTH, ROMFONT_HEIGHT,
                        &used_w, &used_h);
      }
      else
      {
         window_extent (scr, NOVICE_LEVEL, GfxBase->DefaultFont->tf_XSize,
                        GfxBase->DefaultFont->tf_YSize,
                        &zoom_bounds[2], &zoom_bounds[3]);
         window_extent (scr, EXPERT_LEVEL, GfxBase->DefaultFont->tf_XSize,
                        GfxBase->DefaultFont->tf_YSize, &used_w, &used_h);
      }
      
      if (used_w <= scr->Width && used_h <= scr->Height)
         return TRUE;
      
      if (i == 3 && digital_display)
      {
         i = 0;
         digital_display = FALSE;
      }
   }
   
   return FALSE;
}
