/*
 * highscores.c
 * ============
 * Handles highscores.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include "requesters.h"
#include "localize.h"
#include "layout_const.h"
#include "highscores.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>


#define NUM_SCORES   10


extern struct Window  *main_win;
extern APTR   vis_info;
extern UBYTE   text_pen;

extern struct GfxBase * GfxBase;

static char    names[NUM_SCORES][4][31];
static UWORD   scores[NUM_SCORES][4];
static UBYTE   saved_rolls[NUM_SCORES][2];
static BOOL    need_save = FALSE;


void
load_high_scores (
   char  *default_name)
{
   register UWORD   i = 0, j = 0, k = 0, m;
   FILE  *score_file;
   int   ch;
   char   num_str[4];
   UBYTE   status = 0;
   
   if ((score_file = fopen ("yahzee.hiscore", "r")))
   {
      while ((ch = fgetc (score_file)) != EOF && k < NUM_SCORES)
      {
         if (status == 0)
         {
            if (ch == '\n')
            {
               names[k][j][i] = '\0';
               ++status;
               i = 0;
            }
            else
            {
               names[k][j][i] = ch;
               ++i;
            }
         }
         else
         {
            if (ch == '\n')
            {
               num_str[i] = '\0';
               if (status == 1)
                  scores[k][j] = atoi (num_str);
               else
                  saved_rolls[k][j - 2] = atoi (num_str);
               ++status;
               if (status > 2 || j < 2)
               {
                  status = 0;
                  ++j;
                  if (j > 3)
                  {
                     j = 0;
                     ++k;
                  }
               }
               i = 0;
            }
            else
            {
               num_str[i] = ch;
               ++i;
            }
         }
      }
      fclose (score_file);
   }
   
   for (m = k; m < NUM_SCORES; ++m)
   {
      for (i = j; i < 4; ++i)
      {
         switch (status)
         {
         case 0:
            strncpy (names[m][i], default_name, 30);
         case 1:
            scores[m][i] = 0;
         case 2:
            if (i >= 2)
               saved_rolls[m][i - 2] = 0;
            status = 0;
            break;
         }
      }
   }
}

void
save_high_scores (void)
{
   register UBYTE   i, j;
   FILE *score_file;
   
   if (need_save)
   {
      if ((score_file = fopen ("yahzee.hiscore", "w")))
      {
         for (j = 0; j < NUM_SCORES; ++j)
         {
            for (i = 0; i < 4; ++i)
            {
               fputs (names[j][i], score_file);
               fputc ('\n', score_file);
               fprintf (score_file, "%d\n", scores[j][i]);
               if (i >= 2)
                  fprintf (score_file, "%d\n", saved_rolls[j][i - 2]);
            }
         }
         fclose (score_file);
         SetProtection ("yahzee.hiscore", 2);
      }
   }
}

BOOL
update_high_score (
   UBYTE   game,
   UBYTE   player,
   UWORD   score,
   UBYTE   rolls)
{
   char   buf[81], name[31];
   BYTE   n = NUM_SCORES - 1;
   BOOL   get_name = TRUE;
   
   name[0] = '\0';
   
   while (n >= 0)
   {
      if (score > scores[n][game] || (score == scores[n][game] && game < 2) ||
          (score == scores[n][game] && game >= 2 &&
           rolls >= saved_rolls[n][game - 2]))
      {
         need_save = TRUE;
         
         if (n < NUM_SCORES - 1)
         {
            strcpy (names[n + 1][game], names[n][game]);
            scores[n + 1][game] = scores[n][game];
            if (game >= 2)
               saved_rolls[n + 1][game - 2] = saved_rolls[n][game - 2];
         }
         
         if (get_name)
         {
            sprintf (buf, localized_string (MSG_NAME_REQTITLE), player + 1);
            string_requester (main_win, vis_info, buf,
                              localized_string (MSG_NAME_GAD),
                              name, 30);
            get_name = FALSE;
         }
         
         strcpy (names[n][game], name);
         scores[n][game] = score;
         if (game >= 2)
            saved_rolls[n][game - 2] = rolls;
      }
      --n;
   }
   
   return (BOOL)(!get_name);
}

void
display_high_scores (
   UBYTE   game)
{
   register UBYTE   n;
   struct Window  *win;
   struct IntuiMessage  *msg;
   BOOL   done = FALSE;
   struct Requester   req;
   BOOL   win_sleep = FALSE;
   char   text_buf[81];
   UWORD   width, height;
   WORD    left, top;
   
   win_sleep = window_sleep (main_win, &req);
   width = main_win->BorderLeft + main_win->BorderRight +
           (34 + ((game >= 2) ? 5 : 0)) * main_win->RPort->TxWidth +
           2 * INTERWIDTH;
   height = main_win->BorderTop + main_win->BorderBottom +
            NUM_SCORES * main_win->RPort->TxHeight + 2 * INTERHEIGHT;
   left = main_win->LeftEdge + (main_win->Width - width) / 2;
   top = main_win->TopEdge + (main_win->Height - height) / 2;
   win = OpenWindowTags (NULL,
                         WA_Left, (width > main_win->WScreen->Width - left) ?
                                  main_win->WScreen->Width - width : left,
                         WA_Top, (height > main_win->WScreen->Height - top) ?
                                 main_win->WScreen->Height - height : top,
                         WA_Width, width,
                         WA_Height, height,
                         WA_AutoAdjust, FALSE,
                         WA_Title, (IPTR)localized_string (MSG_HIGHSCORE_REQTITLE),
                         WA_ScreenTitle, (IPTR)(main_win->ScreenTitle),
                         WA_PubScreen, (IPTR)(main_win->WScreen),
                         WA_IDCMP, IDCMP_MOUSEBUTTONS,
                         WA_DragBar, TRUE,
                         WA_DepthGadget, TRUE,
                         WA_Activate, TRUE,
                         TAG_DONE);
   if (win != NULL)
   {
      SetAPen (win->RPort, text_pen);
      for (n = 0; n < NUM_SCORES; ++n)
      {
         Move (win->RPort, win->BorderLeft + INTERWIDTH,
               win->BorderTop + INTERHEIGHT + n * win->RPort->TxHeight +
               win->RPort->TxBaseline);
         if (game < 2)
            sprintf (text_buf, "%-30s %3d", names[n][game], scores[n][game]);
         else
         {
            sprintf (text_buf, "%-30s %3d (%2d)",
                  names[n][game], scores[n][game], saved_rolls[n][game - 2]);
         }
         Text (win->RPort, text_buf, strlen (text_buf));
      }
      
      while (!done)
      {
         WaitPort (win->UserPort);
         while ((msg = (struct IntuiMessage *)GetMsg (win->UserPort)))
         {
            done = (msg->Class == IDCMP_MOUSEBUTTONS &&
                    msg->Code == SELECTDOWN);
            ReplyMsg ((struct Message *)msg);
         }
      }
      CloseWindow (win);
   }
   if (win_sleep)
      window_wakeup (main_win, &req);
}
