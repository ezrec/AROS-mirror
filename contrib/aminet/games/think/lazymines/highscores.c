/*
 * highscores.c
 * ============
 * Handles highscores.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/dos.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <graphics/gfxmacros.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "display_globals.h"
#include "requesters.h"
#include "localize.h"
#include "game.h"
#include "highscores.h"


#define HSMAGIC      "LZM#30HS"

#define NUM_SCORES   10


extern struct Window  *main_win;
extern APTR   vis_info;

extern struct GfxBase * GfxBase;
extern struct Library * GadToolsBase;

struct highscore {
   char    name[31];
   UWORD   score;
};

static struct highscore   hiscores[2][3][NUM_SCORES];
static BOOL    need_save = FALSE;


static void
default_scorers (
   char  *name)
{
   int   i, j, k;
   
   
   for (i = 0; i < 2; ++i)
   {
      for (j = 0; j < 3; ++j)
      {
         for (k = 0; k < NUM_SCORES; ++k)
         {
            strcpy (hiscores[i][j][k].name, name);
            hiscores[i][j][k].score = 999;
         }
      }
   }
}


void
load_high_scores (
   char  *default_name)
{
   BPTR   fh;
   char   check[11];
   int    i, j, k;
   
   
   if (NULL != (fh = Open ("lazymines.hiscore", MODE_OLDFILE)))
   {
      Read (fh, check, sizeof (HSMAGIC));
      if (!strcmp (check, HSMAGIC))
      {
         for (i = 0; i < 2; ++i)
            for (j = 0; j < 3; ++j)
               for (k = 0; k < NUM_SCORES; ++k)
                  Read (fh, &hiscores[i][j][k], sizeof (hiscores[i][j][k]));
      }
      else
         default_scorers (default_name);
      
      Close (fh);
   }
   else
      default_scorers (default_name);
}


void
save_high_scores (void)
{
   BPTR   fh;
   int    i, j, k;
   
   
   if (need_save)
   {
      if (NULL != (fh = Open ("LazyMines.hiscore", MODE_NEWFILE)))
      {
         Write (fh, HSMAGIC, sizeof (HSMAGIC));
         for (i = 0; i < 2; ++i)
            for (j = 0; j < 3; ++j)
               for (k = 0; k < NUM_SCORES; ++k)
                  Write (fh, &hiscores[i][j][k], sizeof (hiscores[i][j][k]));
      
         Close (fh);
         SetProtection ("LazyMines.hiscore", 2);
      }
   }
}


UBYTE
update_high_score (
   UWORD   score)
{
   char    name[31];
   BYTE    n = NUM_SCORES - 1;
   UBYTE   get_name = 0;
   
   name[0] = '\0';
   
   while (n >= 0)
   {
      if (score <= hiscores[chosen_task][current_level - 1][n].score)
      {
         need_save = TRUE;
         
         if (n < NUM_SCORES - 1)
         {
            strcpy (hiscores[chosen_task][current_level - 1][n + 1].name,
                    hiscores[chosen_task][current_level - 1][n].name);
            hiscores[chosen_task][current_level - 1][n + 1].score =
                            hiscores[chosen_task][current_level - 1][n].score;
         }
         
         if (get_name == 0)
         {
            string_requester (main_win, vis_info,
                              localized_string (MSG_NAME_REQTITLE),
                              localized_string (MSG_NAME_GAD),
                              name, 30);
            get_name = n + 1;
         }
         else
            --get_name;
         
         strcpy (hiscores[chosen_task][current_level - 1][n].name, name);
         hiscores[chosen_task][current_level - 1][n].score = score;
      }
      --n;
   }
   
   return get_name;
}


void
display_high_scores (
   UBYTE   highlight_no)
{
   struct RastPort   layout_rp;
   ULONG             box_w, box_h, win_w, win_h, temp;
   register UBYTE    n;
   char              text_buf[38], win_title[128];
   STRPTR            label;
   
   struct NewGadget   ng;
   struct Gadget     *gad_list, *ok_gad;
   struct Window     *req_win;
   struct Requester   req;
   BOOL               win_sleep, done = FALSE;
   
   
   sprintf (win_title, "%s - %s (%s)",
            localized_string (MSG_HIGHSCORES_REQTITLE),
            localized_string (MSG_GAME_NOVICE + current_level - 1) + 2,
            localized_string (MSG_SETTINGS_TASK_ALL + chosen_task) + 2);
   InitRastPort (&layout_rp);
   box_w = 37 * layout_rp.TxWidth + 2 * (INTERWIDTH + LINEWIDTH);
   box_h = NUM_SCORES * layout_rp.TxHeight + 2 * (INTERHEIGHT + LINEHEIGHT);
   temp = TextLength (&main_win->WScreen->RastPort,
                      win_title, strlen (win_title)) + 24;
   if (temp > box_w)
      box_w = temp;
   
   label = localized_string (MSG_CONTINUE_GAD);
   ng.ng_TextAttr = main_win->WScreen->Font;
   ng.ng_VisualInfo = vis_info;
   ng.ng_Width = TextLength (&main_win->WScreen->RastPort,
                             label, strlen (label)) +
                 INTERWIDTH + 2 * LINEWIDTH;
   ng.ng_Height = main_win->WScreen->Font->ta_YSize +
                  INTERHEIGHT + 2 * LINEHEIGHT;
   win_w = ((box_w > ng.ng_Width) ? box_w : ng.ng_Width) + 2 * INTERWIDTH +
           main_win->BorderLeft + main_win->BorderRight;
   win_h = box_h + ng.ng_Height + 3 * INTERHEIGHT +
           main_win->BorderTop + main_win->BorderBottom;
   ng.ng_LeftEdge = (win_w - ng.ng_Width) / 2;
   ng.ng_TopEdge = main_win->BorderTop + 2 * INTERHEIGHT + box_h;
   ng.ng_GadgetText = label;
   ng.ng_GadgetID = 0;
   ng.ng_Flags = 0;
   
   ok_gad = CreateContext (&gad_list);
   ok_gad = CreateGadget (BUTTON_KIND, ok_gad, &ng, TAG_DONE);
   
   if (ok_gad)
   {
      req_win = OpenWindowTags (NULL,
                                WA_Left, main_win->LeftEdge +
                                         (main_win->Width - win_w) / 2,
                                WA_Top, main_win->TopEdge +
                                        (main_win->Height - win_h) / 2,
                                WA_Width, win_w,
                                WA_Height, win_h,
                                WA_Title, win_title,
                                WA_ScreenTitle, main_win->ScreenTitle,
                                WA_PubScreen, main_win->WScreen,
                                WA_IDCMP, BUTTONIDCMP | IDCMP_REFRESHWINDOW,
                                WA_DragBar, TRUE,
                                WA_DepthGadget, TRUE,
                                WA_Activate, TRUE,
                                TAG_DONE);
      if (req_win)
      {
         ULONG   winsig, timersig, sigmask;
         struct IntuiMessage  *msg;
         UWORD   dither_data[] = { 0xAAAA, 0x5555 };
         
         
         win_sleep = window_sleep (main_win, &req);
         SetAPen (req_win->RPort, gui_pens[SHINEPEN]);
         SetAfPt (req_win->RPort, dither_data, 1);
         RectFill (req_win->RPort, req_win->BorderLeft, req_win->BorderTop,
                   req_win->Width - req_win->BorderRight - 1,
                   req_win->Height - req_win->BorderBottom - 1);
         SetAPen (req_win->RPort, gui_pens[BACKGROUNDPEN]);
         SetAfPt (req_win->RPort, NULL, 0);
         RectFill (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                   req_win->BorderTop + INTERHEIGHT,
                   req_win->BorderLeft + INTERWIDTH + box_w - 1,
                   req_win->BorderTop + INTERHEIGHT + box_h - 1);
         DrawBevelBox (req_win->RPort, req_win->BorderLeft + INTERWIDTH,
                       req_win->BorderTop + INTERHEIGHT, box_w, box_h,
                       GT_VisualInfo, vis_info, GTBB_Recessed, TRUE,
                       TAG_DONE);
         AddGList (req_win, gad_list, -1, -1, NULL);
         RefreshGList (gad_list, req_win, NULL, -1);
         GT_RefreshWindow (req_win, NULL);
         
         for (n = 0; n < NUM_SCORES; ++n)
         {
            SetAPen (req_win->RPort,
                     gui_pens[((n + 1 == highlight_no) ? HIGHLIGHTTEXTPEN :
                                                         TEXTPEN)]);
            Move (req_win->RPort,
                  req_win->BorderLeft + 2 * INTERWIDTH + LINEWIDTH,
                  req_win->BorderTop + 2 * INTERHEIGHT + LINEHEIGHT +
                  n * req_win->RPort->TxHeight + req_win->RPort->TxBaseline);
            sprintf (text_buf, "%2d %s", n + 1,
                     hiscores[chosen_task][current_level - 1][n].name);
            Text (req_win->RPort, text_buf, strlen (text_buf));
            Move (req_win->RPort,
                  req_win->BorderLeft + box_w -
                  LINEWIDTH - 3 * req_win->RPort->TxWidth,
                  req_win->BorderTop + 2 * INTERHEIGHT + LINEHEIGHT +
                  n * req_win->RPort->TxHeight + req_win->RPort->TxBaseline);
            sprintf (text_buf, "%3d",
                     hiscores[chosen_task][current_level - 1][n].score);
            Text (req_win->RPort, text_buf, strlen (text_buf));
         }
         
         winsig = 1L << req_win->UserPort->mp_SigBit;
         timersig = timer_signal (timer_obj);
         while (!done)
         {
            sigmask = Wait (winsig | timersig);
            if (sigmask & winsig)
            {
               while (msg = GT_GetIMsg (req_win->UserPort))
               {
                  switch (msg->Class)
                  {
                  case IDCMP_GADGETUP:
                     done = TRUE;
                     break;
                  case IDCMP_REFRESHWINDOW:
                     GT_BeginRefresh (req_win);
                     GT_EndRefresh (req_win, TRUE);
                     break;
                  }
                  GT_ReplyIMsg (msg);
               }
            }
            if (sigmask & timersig)
            {
               timer_reply (timer_obj);
               if (time_on)
               {
                  timer_start (timer_obj, 0L, 1000000L);
                  counter_update (time_counter,
                                  counter_value (time_counter) + 1);
               }
            }
         }
         if (win_sleep)
            window_wakeup (main_win, &req);
         CloseWindow (req_win);
      }
      FreeGadgets (gad_list);
   }
}
