/*
 * layout.c
 * ========
 * Handles layout of display.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <graphics/gfxbase.h>
#include "requesters.h"
#include "rules.h"
#include "draw.h"
#include "layout_const.h"
#include "layout.h"

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

extern struct Library * GadToolsBase;

#define MAX(a, b)   (((a) > (b)) ? (a) : (b))


struct TextAttr   topaz8 = {
   "topaz.font", 8, 0, FPF_ROMFONT
};

extern struct GfxBase  *GfxBase;
extern struct TextFont  *romfont;
extern char   scr_title[];
extern char   win_title[];
extern struct Gadget   die_gads[];
extern struct Gadget  *roll_gad, *disp_gad, *gt_gads;
extern struct NewGadget   new_roll_gad, new_disp_gad;


static void
upper_board_extent (
   struct rules  *rules,
   UBYTE          num_players,
   BOOL           split_board,
   UWORD          font_w,
   UWORD          font_h,
   UWORD         *width,
   UWORD         *height)
{
   *width = (upper_label_len (rules) + 3 * num_players) * font_w +
            (num_players + 1) * (INTERWIDTH / 2 + LINEWIDTH) + LINEWIDTH;
   *height = (num_upper_labels (rules) + 1) *
             (font_h + INTERHEIGHT / 2 + LINEHEIGHT) + LINEHEIGHT;
}

static void
lower_board_extent (
   struct rules  *rules,
   UBYTE          num_players,
   BOOL           split_board,
   UWORD          font_w,
   UWORD          font_h,
   UWORD         *width,
   UWORD         *height)
{
   *width = (lower_label_len (rules) + 3 * num_players) * font_w +
            (num_players + 1) * (INTERWIDTH / 2 + LINEWIDTH) + LINEWIDTH;
   *height = num_lower_labels (rules) *
             (font_h + INTERHEIGHT / 2 + LINEHEIGHT) + LINEHEIGHT;
   if (split_board)
      *height += font_h + INTERHEIGHT / 2 + LINEHEIGHT;
}

static void
gadget_extent (
   struct Screen  *scr,
   struct rules   *rules,
   UWORD          *width,
   UWORD          *height,
   BOOL            vert_gads)
{
   if (vert_gads)
   {
      *width = MAX (new_roll_gad.ng_Width, die_gads[0].Width);
      *height = rules->num_dice * (die_gads[0].Height + INTERHEIGHT) +
                new_roll_gad.ng_Height + new_disp_gad.ng_Height + INTERHEIGHT;
   }
   else
   {
      *width = rules->num_dice * (die_gads[0].Width + INTERWIDTH) +
               new_roll_gad.ng_Width + new_disp_gad.ng_Width + INTERWIDTH;
      *height = MAX (new_roll_gad.ng_Height, die_gads[0].Height);
   }
}

BOOL
layout_display (
   struct Screen  *scr,
   struct Window **win,
   struct rules   *rules,
   UBYTE           num_players)
{
   register UBYTE   n;
   BOOL    done, split_board, board_romfont, vert_gads;
   UWORD   used_w, used_h;
   UWORD   board_w[2], board_h[2];
   UWORD   gadget_w = 0, gadget_h = 0;
   
   done = FALSE;
   for (n = 1; n <= 8 && !done; ++n)
   {
      split_board = !(n & 1);
      board_romfont = (n > 4);
      vert_gads = (n + 1) / 2 & 1;
      
      upper_board_extent (rules, num_players, split_board,
                          (board_romfont) ? ROMFONT_WIDTH :
                                            GfxBase->DefaultFont->tf_XSize,
                          (board_romfont) ? ROMFONT_HEIGHT :
                                            GfxBase->DefaultFont->tf_YSize,
                          &board_w[0], &board_h[0]);
      lower_board_extent (rules, num_players, split_board,
                          (board_romfont) ? ROMFONT_WIDTH :
                                            GfxBase->DefaultFont->tf_XSize,
                          (board_romfont) ? ROMFONT_HEIGHT :
                                            GfxBase->DefaultFont->tf_YSize,
                          &board_w[1], &board_h[1]);
      if (split_board)
      {
         used_w = board_w[0] + board_w[1] - LINEWIDTH;
         used_h = MAX (board_h[0], board_h[1]);
      }
      else
      {
         used_w = MAX (board_w[0], board_w[1]);
         used_h = board_h[0] + board_h[1] - LINEHEIGHT;
      }
      gadget_extent (scr, rules, &gadget_w, &gadget_h, vert_gads);
      if (vert_gads)
      {
         used_w += gadget_w + 3 * INTERWIDTH + scr->WBorLeft + scr->WBorRight;
         used_h = MAX (used_h, gadget_h) + 2 * INTERHEIGHT + scr->WBorTop +
                  scr->WBorBottom + scr->Font->ta_YSize + 1;
      }
      else
      {
         used_w = MAX (used_w, gadget_w) + 2 * INTERWIDTH +
                  scr->WBorLeft + scr->WBorRight;
         used_h += gadget_h + 3 * INTERHEIGHT + scr->WBorTop +
                   scr->WBorBottom + scr->Font->ta_YSize + 1;
      }
      
      if (used_w <= scr->Width && used_h <= scr->Height)
         done = TRUE;
   }
   if (!done)
   {
      msg_requester (NULL, "Error Message", "OK", "Screen is too small!");
      
      return FALSE;
   }
   
   if (board_romfont && romfont == NULL)
   {
      if (!(romfont = OpenFont (&topaz8)))
      {
         msg_requester (NULL, "Error Message", "OK",
                        "Couldn't open topaz.font!");
         
         return FALSE;
      }
   }
   
   if (*win == NULL)
   {
      *win = OpenWindowTags (NULL,
                             WA_Left, (scr->Width - used_w) / 2,
                             WA_Top, (scr->Height - used_h) / 2,
                             WA_Width, used_w,
                             WA_Height, used_h,
                             WA_AutoAdjust, FALSE,
                             WA_Activate, TRUE,
                             WA_CloseGadget, TRUE,
                             WA_DepthGadget, TRUE,
                             WA_DragBar, TRUE,
                             WA_Title, win_title,
                             WA_ScreenTitle, scr_title,
                             WA_PubScreen, scr,
                             WA_NewLookMenus, TRUE,
                             WA_IDCMP, BUTTONIDCMP | IDCMP_MOUSEBUTTONS |
                                       IDCMP_MENUPICK | IDCMP_CLOSEWINDOW |
                                       IDCMP_REFRESHWINDOW |
                                       IDCMP_CHANGEWINDOW,
                             TAG_DONE);
   }
   else
   {
      struct IntuiMessage  *msg;
      BOOL   done = FALSE;
      
      delete_scoreboard ((*win)->RPort);
      delete_gadgets ((*win)->RPort, die_gads);
      RemoveGList (*win, die_gads, -1);
      FreeGadgets (gt_gads);
      ChangeWindowBox (*win,
                       (used_w > scr->Width - (*win)->LeftEdge) ?
                       scr->Width - used_w : (*win)->LeftEdge,
                       (used_h > scr->Height - (*win)->TopEdge) ?
                       scr->Height - used_h : (*win)->TopEdge,
                       used_w, used_h);
      WaitPort ((*win)->UserPort);
      while (!done && (msg = (struct IntuiMessage *)GetMsg ((*win)->UserPort)))
      {
         done = (msg->Class == IDCMP_CHANGEWINDOW);
         ReplyMsg ((struct Message *)msg);
      }
   }
   if (*win == NULL)
   {
      msg_requester (NULL, "Error Message", "OK", "Couldn't open window!");
      
      return FALSE;
   }
   
   for (n = 0; n < rules->num_dice; ++n)
   {
      die_gads[n].NextGadget = (n == rules->num_dice - 1) ? NULL :
                                                            &die_gads[n + 1];
      if (vert_gads)
      {
         die_gads[n].LeftEdge = used_w - (*win)->BorderRight - INTERWIDTH -
                                gadget_w + (gadget_w - die_gads[n].Width) / 2;
         die_gads[n].TopEdge = ((n > 0) ?
                             die_gads[n - 1].TopEdge + die_gads[n - 1].Height :
                             (*win)->BorderTop) + INTERHEIGHT;
      }
      else
      {
         die_gads[n].LeftEdge = ((n > 0) ?
                             die_gads[n - 1].LeftEdge + die_gads[n - 1].Width :
                             (*win)->BorderLeft) + INTERWIDTH;
         die_gads[n].TopEdge = used_h - (*win)->BorderBottom - INTERHEIGHT -
                               gadget_h + (gadget_h - die_gads[n].Height) / 2;
      }
      die_gads[n].Flags |= GFLG_DISABLED;
   }
   AddGList (*win, die_gads, -1, -1, NULL);
   if (vert_gads)
   {
      new_roll_gad.ng_LeftEdge = used_w - (*win)->BorderRight - INTERWIDTH -
                                 gadget_w +
                                 (gadget_w - new_roll_gad.ng_Width) / 2;
      new_roll_gad.ng_TopEdge = die_gads[n - 1].TopEdge +
                                die_gads[n - 1].Height + INTERHEIGHT;
      new_disp_gad.ng_LeftEdge = new_roll_gad.ng_LeftEdge;
      new_disp_gad.ng_TopEdge = new_roll_gad.ng_TopEdge +
                                new_roll_gad.ng_Height + INTERHEIGHT;
   }
   else
   {
      new_roll_gad.ng_LeftEdge = die_gads[n - 1].LeftEdge +
                                 die_gads[n - 1].Width + INTERWIDTH;
      new_roll_gad.ng_TopEdge = used_h - (*win)->BorderBottom - INTERHEIGHT -
                                gadget_h +
                                (gadget_h - new_roll_gad.ng_Height) / 2;
      new_disp_gad.ng_LeftEdge = new_roll_gad.ng_LeftEdge +
                                 new_roll_gad.ng_Width + INTERWIDTH;
      new_disp_gad.ng_TopEdge = new_roll_gad.ng_TopEdge;
   }
   roll_gad = CreateContext (&gt_gads);
   roll_gad = CreateGadget (BUTTON_KIND, roll_gad, &new_roll_gad, TAG_DONE);
   disp_gad = CreateGadget (NUMBER_KIND, roll_gad, &new_disp_gad,
                            GTNM_Number, 3, GTNM_Border, TRUE, TAG_DONE);
   if (disp_gad == NULL)
   {
      msg_requester (NULL, "Error Message", "OK",
                     "Couldn't create gadgets!");
      
      return FALSE;
   }
   AddGList (*win, gt_gads, -1, -1, NULL);
   RefreshGadgets (die_gads, *win, NULL);
   GT_RefreshWindow (*win, NULL);
   
   SetFont ((*win)->RPort, (board_romfont) ? romfont : GfxBase->DefaultFont);
   init_board_grid (*win, rules, num_players, split_board);
   draw_scoreboard ((*win)->RPort, rules, num_players);
   
   return TRUE;
}

void
init_board_grid (
   struct Window  *win,
   struct rules   *rules,
   UBYTE           num_players,
   BOOL            split_board)
{
   register UBYTE   i, j, prev;
   UBYTE   limit;
   
   limit = num_upper_labels (rules);
   upper_board.left = win->BorderLeft + INTERWIDTH;
   upper_board.top = win->BorderTop + INTERHEIGHT;
   upper_board_extent (rules, num_players, split_board,
                       win->RPort->TxWidth, win->RPort->TxHeight,
                       &upper_board.width, &upper_board.height);
   lower_board.left = upper_board.left +
                      ((split_board) ? upper_board.width - LINEWIDTH : 0);
   lower_board.top = upper_board.top +
                     ((split_board) ? 0 : upper_board.height - LINEHEIGHT);
   lower_board_extent (rules, num_players, split_board,
                      win->RPort->TxWidth, win->RPort->TxHeight,
                      &lower_board.width, &lower_board.height);
   if (!split_board)
   {
      upper_board.width = MAX (upper_board.width, lower_board.width);
      lower_board.width = upper_board.width;
   }
   
   board_grid[0][0].left = upper_board.left + LINEWIDTH;
   board_grid[0][0].top = upper_board.top + win->RPort->TxHeight +
                          INTERHEIGHT / 2 + 2 * LINEHEIGHT;
   board_grid[0][0].width = INTERWIDTH / 2 +
                            upper_label_len (rules) * win->RPort->TxWidth;
   if (!split_board)
   {
      board_grid[0][limit].width = INTERWIDTH / 2 +
                                 lower_label_len (rules) * win->RPort->TxWidth;
      board_grid[0][0].width = MAX (board_grid[0][0].width,
                                    board_grid[0][limit].width);
   }
   board_grid[0][0].height = win->RPort->TxHeight + INTERHEIGHT / 2;
   for (i = 1; i <= num_players; ++i)
   {
      board_grid[i][0].left = board_grid[i - 1][0].left +
                              board_grid[i - 1][0].width + LINEWIDTH;
      board_grid[i][0].top = board_grid[0][0].top;
      board_grid[i][0].width = 3 * win->RPort->TxWidth + INTERWIDTH / 2;
      board_grid[i][0].height = board_grid[0][0].height;
   }
   prev = 0;
   for (j = 1; j < limit; ++j)
   {
      if (!rules->labels[j])
         ++limit;
      else
      {
         for (i = 0; i <= num_players; ++i)
         {
            board_grid[i][j].left = board_grid[i][prev].left;
            board_grid[i][j].top = board_grid[i][prev].top +
                                   board_grid[i][prev].height + LINEHEIGHT;
            board_grid[i][j].width = board_grid[i][prev].width;
            board_grid[i][j].height = board_grid[i][prev].height;
         }
         prev = j;
      }
   }
   if (split_board)
   {
      while (!rules->labels[j])
         ++j;
      
      board_grid[0][j].left = lower_board.left + LINEWIDTH;
      board_grid[0][j].top = board_grid[0][0].top;
      board_grid[0][j].width = INTERWIDTH / 2 +
                               lower_label_len (rules) * win->RPort->TxWidth;
      board_grid[0][j].height = board_grid[0][0].height;
      for (i = 1; i <= num_players; ++i)
      {
         board_grid[i][j].left = board_grid[i - 1][j].left +
                                 board_grid[i - 1][j].width + LINEWIDTH;
         board_grid[i][j].top = board_grid[0][j].top;
         board_grid[i][j].width = 3 * win->RPort->TxWidth + INTERWIDTH / 2;
         board_grid[i][j].height = board_grid[0][j].height;
      }
      prev = j;
      ++j;
   }
   for (; j < MAX_LABELS; ++j)
   {
      if (rules->labels[j])
      {
         for (i = 0; i <= num_players; ++i)
         {
            board_grid[i][j].left = board_grid[i][prev].left;
            board_grid[i][j].top = board_grid[i][prev].top +
                                   board_grid[i][prev].height + LINEHEIGHT;
            board_grid[i][j].width = board_grid[i][prev].width;
            board_grid[i][j].height = board_grid[i][prev].height;
         }
         prev = j;
      }
   }
}

