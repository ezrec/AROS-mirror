/*
 * lazymines.c
 * ===========
 * Main module.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <graphics/gfxbase.h>
#include <proto/graphics.h>
#include <intuition/imageclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/intuition.h>
#include <proto/alib.h>

#include "display_globals.h"
#include "localize.h"
#include "tooltypes.h"
#include "requesters.h"
#include "highscores.h"
#include "field.h"
#include "counter.h"
#include "layout.h"
#include "images.h"
#include "game.h"
#include "timer.h"
#include "button.h"


#define PRG_NAME        "LazyMines"
#define VERSION_NO      "3.2"
#define CREATION_YEAR   "1994-1998"
#define AUTHOR          "Håkan L. Younes"
#define EMAIL           "lorens@hem.passagen.se"

char   version[] = "$VER: LazyMines 3.2 (15.12.98)";


void event_loop (void);
BOOL process_menus (UWORD);
void update_display (void);
void win_game (void);
void game_over (void);
BOOL new_game (UWORD);
BOOL initialize (void);
BOOL init_display (void);
BOOL init_menu (void);
void finalize (void);
void finalize_display (void);


struct IntuitionBase  *IntuitionBase = NULL;
struct GfxBase        *GfxBase = NULL;
struct Library        *GadToolsBase = NULL, *UtilityBase = NULL;

struct TextAttr   topaz8 = {
   "topaz.font", 8, 0, FPF_ROMFONT
};

struct NewMenu new_menu[] = {
   { NM_TITLE, (STRPTR)MSG_GAME_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_NEW, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_NOVICE, 0, CHECKIT, ~0x04, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_AMATEUR, 0, CHECKIT, ~0x08, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_EXPERT, 0, CHECKIT, ~0x10, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_OPTIONAL, 0, CHECKIT, ~0x20, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_HIGHSCORE, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_ABOUT, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_QUIT, 0, 0, 0, 0 },
   { NM_TITLE, (STRPTR)MSG_SETTINGS_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_TASK, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_TASK_ALL, 0, CHECKIT, ~0x01, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_TASK_PATH, 0, CHECKIT, ~0x02, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_WARNINGS, 0, CHECKIT | MENUTOGGLE, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_NOCOLORS, 0, CHECKIT | MENUTOGGLE, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_AUTOOPEN, 0, CHECKIT, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_SAVE, 0, 0, 0, 0 },
   { NM_END,   NULL, 0, 0, 0, 0 }
};
#define MENU_Game       0
#define ITEM_New        0
#define ITEM_Novice     2
#define ITEM_Amateur    3
#define ITEM_Expert     4
#define ITEM_Optional   5
#define ITEM_High       7
#define ITEM_About      8
#define ITEM_Quit       10
#define MENU_Settings       1
#define ITEM_Task           0
#define ITEM_Warnings       1
#define ITEM_NoColors       2
#define ITEM_AutoOpen       4
#define ITEM_SaveSettings   6

char   pubscr_name[129];
char   scr_title[81];
struct Screen  *pub_screen = NULL;
struct DrawInfo  *draw_info = NULL;
struct TextFont  *romfont = NULL;
APTR   vis_info = NULL;
struct Window  *main_win = NULL;
struct Menu  *main_menu = NULL;
UWORD   zoom_bounds[4];
LONG    win_left = -1, win_top = -1;

#define LEFTDOWN     0x01
#define RIGHTDOWN    0x02
#define BUTTONDOWN   0x04

struct level   levels[] = {
   { 30, 16, 99 },
   {  8,  8, 25 },
   { 16, 16, 25 },
   { 30, 16, 33 }
};

UBYTE   current_level = EXPERT_LEVEL;
UBYTE   chosen_task = SWEEP_ALL;
UBYTE   auto_opening = MIN_OPENING;
BOOL    place_warnings = FALSE;

field_ptr     field = NULL;
counter_ptr   flag_counter = NULL;
counter_ptr   time_counter = NULL;
timer_ptr     timer_obj;

BOOL   playing = TRUE;
BOOL   time_on = FALSE;


int
main (
   int    argc,
   char  *argv[])
{
   init_locale ("LazyMines.catalog", 3L);
   
   pubscr_name[0] = '\0';
   
   handle_startup_msg (argv, argc == 0);
   if ((BYTE)auto_opening < 0)
      auto_opening = 0;
   else if (auto_opening > 10)
      auto_opening = 10;
   
   if (current_level == OPTIONAL_LEVEL)
   {
      new_menu[ITEM_Optional + 1].nm_Flags |= CHECKED;
      new_menu[ITEM_High + 1].nm_Flags |= NM_ITEMDISABLED;
   }
   else
      new_menu[ITEM_Novice + current_level].nm_Flags |= CHECKED;
   new_menu[ITEM_Quit + ITEM_Task + 4 + chosen_task].nm_Flags |= CHECKED;
   if (chosen_task == SWEEP_PATH)
      new_menu[ITEM_Quit + ITEM_AutoOpen + 5].nm_Flags |= NM_ITEMDISABLED;
   if (auto_opening > 0)
      new_menu[ITEM_Quit + ITEM_AutoOpen + 5].nm_Flags |= CHECKED;
   if (place_warnings)
      new_menu[ITEM_Quit + ITEM_Warnings + 5].nm_Flags |= CHECKED;
   if (!display_colors)
      new_menu[ITEM_Quit + ITEM_NoColors + 5].nm_Flags |= CHECKED;
   
   if (initialize ())
   {
      srand48 (time (NULL));
      load_high_scores (AUTHOR);
      event_loop ();
      save_high_scores ();
   }
   
   finalize ();
   finalize_locale ();

   return RETURN_OK;
}


void
event_loop (void)
{
   extern UBYTE   calm_data[], afraid_data[];
   
   ULONG   winsig, timersig, sigmask;
   WORD    row, col, real_row = -2, real_col = -2, mx, my;
   UBYTE   mouse_stat = 0;
   BOOL    ignore_click = TRUE;
   BOOL    quit = FALSE;
   ULONG   class;
   UWORD   code;
   struct IntuiMessage  *msg;
   ULONG   full_idcmp = main_win->IDCMPFlags;
   BOOL    zoomed = FALSE;
   
   
   while (!new_game (current_level))
      request_optional_size (main_win);
   
   row = (main_win->MouseY >= field_top (field)) ?
         (main_win->MouseY - field_top (field)) / cell_h : -1;
   col = (main_win->MouseX >= field_left (field)) ?
         (main_win->MouseX >= field_left (field)) / cell_w : -1;
   if (field_inside (field, row, col) && playing)
      main_win->Flags |= WFLG_RMBTRAP;
   else
      main_win->Flags &= ~WFLG_RMBTRAP;
   
   winsig = 1L << main_win->UserPort->mp_SigBit;
   timersig = timer_signal (timer_obj);
   while (!quit)
   {
      sigmask = Wait (winsig | timersig);
      if (sigmask & timersig)
      {
         timer_reply (timer_obj);
         if (time_on)
         {
            timer_start (timer_obj, 0L, 1000000L);
            if (!zoomed)
            {
               counter_update (time_counter,
                               counter_value (time_counter) + 1);
            }
         }
      }
      if (sigmask & winsig)
      {
         while ((msg = (struct IntuiMessage *)GetMsg (main_win->UserPort)))
         {
            class = msg->Class;
            code = msg->Code;
            row = (msg->MouseY >= field_top (field)) ?
                  (msg->MouseY - field_top (field)) / cell_h : -1;
            col = (msg->MouseX >= field_left (field)) ?
                  (msg->MouseX - field_left (field)) / cell_w : -1;
            mx = msg->MouseX;
            my = msg->MouseY;
            ReplyMsg ((struct Message *)msg);
            switch (class)
            {
            case IDCMP_MOUSEBUTTONS:
               if (ignore_click)
                  ignore_click = FALSE;
               else if (mouse_stat == 0 &&
                        code == SELECTDOWN && button_hit (mx, my))
               {
                  mouse_stat = BUTTONDOWN;
               }
               else if (mouse_stat == BUTTONDOWN && code == SELECTUP)
               {
                  mouse_stat = 0;
                  if (button_release (mx, my))
                     new_game (current_level);
               }
               else if (mouse_stat != BUTTONDOWN && playing)
               {
                  switch (code)
                  {
                  case SELECTDOWN:
                     if (mouse_stat == 0)
                     {
                        real_row = row;
                        real_col = col;
                     }
                     mouse_stat |= LEFTDOWN;
                     if (real_row == row && real_col == col &&
                         field_inside (field, row, col))
                     {
                        SetAttrs (face_image, IA_Data, afraid_data, TAG_DONE);
                        button_render ();
                        if (mouse_stat & RIGHTDOWN)
                           press_around (field, row, col);
                        else
                           press_this (field, row, col);
                     }
                     break;
                  case MENUDOWN:
                     if (mouse_stat == 0)
                     {
                        real_row = row;
                        real_col = col;
                     }
                     mouse_stat |= RIGHTDOWN;
                     if (real_row == row && real_col == col &&
                         field_inside (field, row, col) &&
                         mouse_stat & LEFTDOWN)
                     {
                        release_this (field, row, col);
                        press_around (field, row, col);
                     }
                     break;
                  case SELECTUP:
                     if (real_row == row && real_col == col &&
                         mouse_stat & LEFTDOWN)
                     {
                        if (!time_on)
                        {
                           time_on = TRUE;
                           timer_start (timer_obj, 0L, 1000000L);
                        }
                        if (field_inside (field, row, col))
                        {
                           if (mouse_stat & RIGHTDOWN)
                              playing = sweep_this (field, row, col);
                           else
                              playing = reveal_this (field, row, col);
                        }
                        
                        if (!playing)
                           game_over ();
                        if (field_swept (field))
                        {
                           time_on = playing = FALSE;
                           timer_stop (timer_obj);
                           win_game ();
                        }
                        else
                        {
                           time_on = playing;
                           if (!time_on)
                              timer_stop (timer_obj);
                        }
                     }
                     if (playing)
                     {
                        SetAttrs (face_image, IA_Data, calm_data, TAG_DONE);
                        button_render ();
                     }
                     mouse_stat = 0;
                     break;
                  case MENUUP:
                     if (real_row == row && real_col == col &&
                         mouse_stat & RIGHTDOWN)
                     {
                        if (!time_on)
                        {
                           time_on = TRUE;
                           timer_start (timer_obj, 0L, 1000000L);
                        }
                        if (field_inside (field, row, col))
                        {
                           if (mouse_stat & LEFTDOWN)
                              playing = sweep_this (field, row, col);
                           else
                              toggle_lock (field, row, col);
                        }
                        
                        if (!playing)
                           game_over ();
                        if (field_swept (field))
                        {
                           time_on = playing = FALSE;
                           timer_stop (timer_obj);
                           win_game ();
                        }
                        else
                        {
                           time_on = playing;
                           if (!time_on)
                              timer_stop (timer_obj);
                        }
                     }
                     if (playing)
                     {
                        SetAttrs (face_image, IA_Data, calm_data, TAG_DONE);
                        button_render ();
                     }
                     mouse_stat = 0;
                     break;
                  }
               }
               break;
            case IDCMP_MENUPICK:
               quit = process_menus (code);
               break;
            case IDCMP_MOUSEMOVE:
               ignore_click = FALSE;
               if (mouse_stat == BUTTONDOWN)
                  button_pressed (mx, my);
               else if (mouse_stat != 0)
               {
                  if (mouse_stat & LEFTDOWN)
                  {
                     if (mouse_stat & RIGHTDOWN)
                     {
                        if (field_inside (field, real_row, real_col))
                        {
                           if (real_row == row && real_col == col)
                              press_around (field, real_row, real_col);
                           else
                              release_around (field, real_row, real_col);
                        }
                     }
                     else
                     {
                        if (field_inside (field, real_row, real_col))
                        {
                           if (real_row == row && real_col == col)
                              press_this (field, real_row, real_col);
                           else
                              release_this (field, real_row, real_col);
                        }
                     }
                  }
               }
               else
               {
                  if (field_inside (field, row, col) && playing)
                     main_win->Flags |= WFLG_RMBTRAP;
                  else
                     main_win->Flags &= ~WFLG_RMBTRAP;
               }
               break;
            case IDCMP_ACTIVEWINDOW:
               ignore_click = !ignore_click;
               break;
            case IDCMP_CLOSEWINDOW:
               quit = TRUE;
               break;
            case IDCMP_CHANGEWINDOW:
               if (!zoomed && (main_win->Flags & WFLG_ZOOMED))
               {
                  ModifyIDCMP (main_win, IDCMP_CLOSEWINDOW |
                                         IDCMP_CHANGEWINDOW |
                                         IDCMP_REFRESHWINDOW);
                  zoomed = TRUE;
               }
               if (zoomed && !(main_win->Flags & WFLG_ZOOMED))
               {
                  ModifyIDCMP (main_win, full_idcmp);
                  zoomed = FALSE;
               }
               break;
            case IDCMP_REFRESHWINDOW:
               BeginRefresh (main_win);
               update_display ();
               EndRefresh (main_win, TRUE);
            break;
            }
         }
      }
      row = (main_win->MouseY >= field_top (field)) ?
            (main_win->MouseY - field_top (field)) / cell_h : -1;
      col = (main_win->MouseX >= field_left (field)) ?
            (main_win->MouseX >= field_left (field)) / cell_w : -1;
      if (zoomed || (field_inside (field, row, col) && playing) ||
          mouse_stat == BUTTONDOWN)
      {
         main_win->Flags |= WFLG_RMBTRAP;
      }
      else
         main_win->Flags &= ~WFLG_RMBTRAP;
   }
}


BOOL
process_menus (
   UWORD   code)
{
   char    buf[256];
   UWORD   menu_no, item_no, sub_no;
   struct MenuItem  *item;
   BOOL   quit = FALSE;
   
   
   while (code != MENUNULL)
   {
      item = ItemAddress (main_menu, code);
      menu_no = MENUNUM (code);
      item_no = ITEMNUM (code);
      sub_no = SUBNUM (code);
      
      if (!quit)
      {
         switch (menu_no)
         {
         case MENU_Game:
            switch (item_no)
            {
            case ITEM_New:
               while (!new_game (current_level))
               {
                  if (current_level == OPTIONAL_LEVEL)
                     request_optional_size (main_win);
               }
               break;
            case ITEM_Novice:
               OnMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_High, NOSUB));
               while (!new_game (NOVICE_LEVEL))
                  ;
               break;
            case ITEM_Amateur:
               OnMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_High, NOSUB));
               while (!new_game (AMATEUR_LEVEL))
                  ;
               break;
            case ITEM_Expert:
               OnMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_High, NOSUB));
               while (!new_game (EXPERT_LEVEL))
                  ;
               break;
            case ITEM_Optional:
               OffMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_High, NOSUB));
               do
                  request_optional_size (main_win);
               while (!new_game (OPTIONAL_LEVEL));
               break;
            case ITEM_High:
               display_high_scores (0);
               break;
            case ITEM_About:
               sprintf (buf, localized_string (MSG_ABOUT_REQMSG),
                              AUTHOR, EMAIL, CREATION_YEAR, AUTHOR);
               about_requester (main_win,
                                bigmine_image, PRG_NAME, VERSION_NO, buf);
               break;
            case ITEM_Quit:
               quit = TRUE;
               break;
            }
            break;
         case MENU_Settings:
            switch (item_no)
            {
            case ITEM_Task:
               if (sub_no != MENUNULL)
               {
                  chosen_task = sub_no;
                  if (chosen_task == SWEEP_ALL)
                  {
                     OnMenu (main_win,
                             FULLMENUNUM (MENU_Settings,
                                          ITEM_AutoOpen, NOSUB));
                  }
                  else
                  {
                     OffMenu (main_win,
                              FULLMENUNUM (MENU_Settings,
                                           ITEM_AutoOpen, NOSUB));
                  }
                  while (!new_game (current_level))
                  {
                     if (current_level == OPTIONAL_LEVEL)
                        request_optional_size (main_win);
                  }
               }
               break;
            case ITEM_AutoOpen:
               request_autoopening (main_win);
               if (auto_opening == 0)
               {
                  ClearMenuStrip (main_win);
                  item->Flags &= ~CHECKED;
                  ResetMenuStrip (main_win, main_menu);
               }
               break;
            case ITEM_Warnings:
               place_warnings = item->Flags & CHECKED;
               break;
            case ITEM_NoColors:
               display_colors = !(item->Flags & CHECKED);
               update_images (display_colors);
               update_display ();
               break;
            case ITEM_SaveSettings:
               save_tooltypes ();
            }
            break;
         }
      }
      code = item->NextSelect;
   }
   
   return quit;
}

void
update_display (void)
{
   DrawBevelBox (main_win->RPort, main_win->BorderLeft, main_win->BorderTop,
                 main_win->Width - main_win->BorderLeft -
                 main_win->BorderRight, main_win->Height -
                 main_win->BorderTop - main_win->BorderBottom,
                 GT_VisualInfo, vis_info,
                 TAG_DONE);
   if (digital_display)
   {
      counter_draw (flag_counter, vis_info);
      counter_draw (time_counter, vis_info);
      
      button_render ();
   }
   field_draw (field);
}

void
win_game (void)
{
   extern UBYTE   happy_data[];
   UBYTE   hi_pos;
   SetAttrs (face_image, IA_Data, happy_data, TAG_DONE);
   button_render ();
   field_win (field);
   if (current_level != OPTIONAL_LEVEL)
      if ((hi_pos = update_high_score (counter_value (time_counter))))
         display_high_scores (hi_pos);
}

void
game_over (void)
{
   extern UBYTE   dead_data[];
   
   SetAttrs (face_image, IA_Data, dead_data, TAG_DONE);
   button_render ();
   field_lose (field);
}

BOOL
new_game (
   UWORD   level)
{
   extern UBYTE   calm_data[];
   
   struct Requester   req;
   BOOL               win_sleep, ret_val;
   
   
   win_sleep = window_sleep (main_win, &req);
   
   if (!playing)
      SetAttrs (face_image, IA_Data, calm_data, TAG_DONE);
   if (level != current_level || level == OPTIONAL_LEVEL)
   {
      UWORD   win_w, win_h;
      ULONG   win_idcmp = main_win->IDCMPFlags;
      BOOL    done = FALSE;
      struct IntuiMessage  *msg;
      
      field_delete (field);
      if (digital_display)
         counter_delete (time_counter);
      
      SetAPen (main_win->RPort, gui_pens[BACKGROUNDPEN]);
      Move (main_win->RPort, main_win->BorderLeft + LINEWIDTH,
            main_win->Height - main_win->BorderBottom - 1);
      Draw (main_win->RPort, main_win->Width - main_win->BorderRight - 1,
            main_win->Height - main_win->BorderBottom - 1);
      Draw (main_win->RPort, main_win->Width - main_win->BorderRight - 1,
            main_win->BorderTop + LINEHEIGHT);
      Move (main_win->RPort, main_win->Width - main_win->BorderRight - 2,
            main_win->BorderTop + LINEHEIGHT);
      Draw (main_win->RPort, main_win->Width - main_win->BorderRight - 2,
            main_win->Height - main_win->BorderBottom - 2);
      
      ModifyIDCMP (main_win, IDCMP_CHANGEWINDOW);
      window_extent (pub_screen, level, main_win->RPort->TxWidth,
                     main_win->RPort->TxHeight, &win_w, &win_h);
      ChangeWindowBox (main_win,
                       (win_w > pub_screen->Width - main_win->LeftEdge) ?
                       pub_screen->Width - win_w : main_win->LeftEdge,
                       (win_h > pub_screen->Height - main_win->TopEdge) ?
                       pub_screen->Height - win_h : main_win->TopEdge,
                       win_w, win_h);
      while (!done)
      {
         WaitPort (main_win->UserPort);
         while ((msg = (struct IntuiMessage *)GetMsg (main_win->UserPort)))
         {
            if (msg->Class == IDCMP_CHANGEWINDOW)
               done = TRUE;
            ReplyMsg ((struct Message *)msg);
         }
      }
      ModifyIDCMP (main_win, win_idcmp);
      
      if (digital_display)
      {
         UWORD   btn_w;
         
         btn_w = main_win->Width -
                 main_win->BorderLeft - main_win->BorderRight -
                 2 * counter_width () - 3 * INTERWIDTH - 2 * LINEWIDTH;
         if (btn_w > 28)
            btn_w = 28;
         button_changebox ((main_win->Width - btn_w) / 2,
                           main_win->BorderTop + LINEHEIGHT + INTERHEIGHT +
                           (counter_height () - btn_w + 8) / 2,
                           btn_w, btn_w - 8);
         
         counter_move (time_counter,
                       main_win->Width - main_win->BorderRight -
                       LINEWIDTH - INTERWIDTH - counter_width (),
                       main_win->BorderTop + LINEHEIGHT + INTERHEIGHT);
         counter_draw (time_counter, vis_info);
      }
      DrawBevelBox (main_win->RPort, main_win->BorderLeft, main_win->BorderTop,
                    main_win->Width - main_win->BorderLeft -
                    main_win->BorderRight, main_win->Height -
                    main_win->BorderTop - main_win->BorderBottom,
                    GT_VisualInfo, vis_info,
                    TAG_DONE);
      
      current_level = level;
   }
   if (level == OPTIONAL_LEVEL)
   {
      field_size (field, levels[level].rows, levels[level].columns,
                  levels[level].bombs);
      counter_update (flag_counter, levels[current_level].bombs);
   }
   else
   {
      if (!playing)
         button_render ();
      
      field_size (field, levels[level].rows, levels[level].columns,
                  levels[level].rows * levels[level].columns *
                  levels[level].bombs /
                  ((chosen_task == SWEEP_ALL) ? 160 : 100));
      counter_update (flag_counter,
                      levels[level].rows * levels[level].columns *
                      levels[level].bombs /
                      ((chosen_task == SWEEP_ALL) ? 160 : 100));
   }
   playing = TRUE;
   if (time_on)
   {
      time_on = FALSE;
      timer_stop (timer_obj);
   }
   counter_update (time_counter, 0);
   
   ret_val = field_reset (field);
   
   if (win_sleep)
      window_wakeup (main_win, &req);
   
   return ret_val;
}

BOOL
initialize (void)
{
   if ((IntuitionBase = (struct IntuitionBase *)
                       OpenLibrary ("intuition.library", 37L)))
   {
      if ((GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 37L)))
      {
         if ((GadToolsBase = OpenLibrary ("gadtools.library", 37L)))
         {
            if ((UtilityBase = OpenLibrary ("utility.library", 37L)))
               return init_display ();
            else
               error_requester (NULL, MSG_OPEN_ERROR, "utility.library");
         }
         else
            error_requester (NULL, MSG_OPEN_ERROR, "gadtools.library");
      }
      else
         error_requester (NULL, MSG_OPEN_ERROR,  "graphics.library");
   }
   else
      error_requester (NULL, MSG_OPEN_ERROR, "intuition.library");
   
   return FALSE;
}

BOOL
init_display (void)
{
   BOOL    use_romfont;
   UWORD   win_w, win_h;
   
   strncpy (scr_title, PRG_NAME, 80);
   strncat (scr_title, " v", 80);
   strncat (scr_title, VERSION_NO, 80);
   strncat (scr_title, " - ©", 80);
   strncat (scr_title, CREATION_YEAR, 80);
   strncat (scr_title, " ", 80);
   strncat (scr_title, AUTHOR, 80);
   
   if ((pub_screen =
        LockPubScreen ((pubscr_name[0] == '\0') ? NULL : pubscr_name)) ||
       (pub_screen = LockPubScreen (NULL)))
   {
      if ((draw_info = GetScreenDrawInfo (pub_screen)))
      {
         gui_pens = draw_info->dri_Pens;
         init_pens (pub_screen);
         if (layout_display (pub_screen, &use_romfont))
         {
            if (levels[OPTIONAL_LEVEL].rows < MIN_ROWS)
               levels[OPTIONAL_LEVEL].rows = MIN_ROWS;
            else if (levels[OPTIONAL_LEVEL].rows > max_rows)
               levels[OPTIONAL_LEVEL].rows = max_rows;
            if (levels[OPTIONAL_LEVEL].columns < MIN_COLUMNS)
               levels[OPTIONAL_LEVEL].columns = MIN_COLUMNS;
            else if (levels[OPTIONAL_LEVEL].columns > max_columns)
               levels[OPTIONAL_LEVEL].columns = max_columns;
            if (levels[OPTIONAL_LEVEL].bombs < MIN_MINES)
               levels[OPTIONAL_LEVEL].bombs = MIN_MINES;
            else if (levels[OPTIONAL_LEVEL].bombs >
                     levels[OPTIONAL_LEVEL].rows *
                     levels[OPTIONAL_LEVEL].columns * 0.9)
            {
               levels[OPTIONAL_LEVEL].bombs = levels[OPTIONAL_LEVEL].rows *
                                              levels[OPTIONAL_LEVEL].columns *
                                              0.9;
            }
            if (use_romfont)
            {
               if (!(romfont = OpenFont (&topaz8)))
               {
                  error_requester (NULL, MSG_OPEN_ERROR, "topaz.font");
                  
                  return FALSE;
               }
               window_extent (pub_screen, current_level, ROMFONT_WIDTH,
                              ROMFONT_HEIGHT, &win_w, &win_h);
            }
            else
            {
               window_extent (pub_screen, current_level,
                              GfxBase->DefaultFont->tf_XSize,
                              GfxBase->DefaultFont->tf_YSize, &win_w, &win_h);
            }
            if ((vis_info = GetVisualInfo (pub_screen, TAG_DONE)))
            {
               zoom_bounds[0] = 0;
               zoom_bounds[1] = 0;
               zoom_bounds[3] = pub_screen->WBorTop +
                                pub_screen->Font->ta_YSize + 1;
               if (win_left == -1)
                  win_left = (pub_screen->Width - win_w) / 2;
               if (win_top == -1)
                  win_top = (pub_screen->Height - win_h) / 2;
               main_win = OpenWindowTags (NULL,
                           WA_Left, win_left,
                           WA_Top, win_top,
                           WA_Width, win_w,
                           WA_Height, win_h,
                           WA_Zoom, zoom_bounds,
                           WA_Activate, TRUE,
                           WA_CloseGadget, TRUE,
                           WA_DepthGadget, TRUE,
                           WA_DragBar, TRUE,
                           WA_Title, (digital_display) ?
                                     PRG_NAME : "    :    ",
                           WA_ScreenTitle, scr_title,
                           WA_PubScreen, pub_screen,
                           WA_NewLookMenus, TRUE,
                           WA_ReportMouse, TRUE,
                           WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK |
                                     IDCMP_MOUSEMOVE |
                                     IDCMP_ACTIVEWINDOW | IDCMP_CLOSEWINDOW |
                                     IDCMP_CHANGEWINDOW | IDCMP_REFRESHWINDOW,
                           TAG_DONE);
               if (main_win)
               {
                  if (!init_menu ())
                     return FALSE;
                  
                  if (use_romfont)
                     SetFont (main_win->RPort, romfont);
                  
                  if (init_images (pub_screen,
                                   digital_display, display_colors))
                  {
                     if (digital_display)
                     {
                        UWORD   btn_w;
                        flag_counter = counter_init (main_win,
                                                  main_win->BorderLeft +
                                                  LINEWIDTH + INTERWIDTH,
                                                  main_win->BorderTop +
                                                  LINEHEIGHT + INTERHEIGHT,
                                                  levels[current_level].bombs,
                                                  TRUE);
                        time_counter = counter_init (main_win,
                                                  main_win->Width -
                                                  main_win->BorderRight -
                                                  LINEWIDTH - INTERWIDTH -
                                                  counter_width (),
                                                  main_win->BorderTop +
                                                  LINEHEIGHT + INTERHEIGHT,
                                                  0, TRUE);
                        btn_w = main_win->Width -
                                main_win->BorderLeft - main_win->BorderRight -
                                2 * counter_width () - 3 * INTERWIDTH -
                                2 * LINEWIDTH;
                        if (btn_w > 28)
                           btn_w = 28;
                        button_init (main_win->RPort,
                                     (main_win->Width - btn_w) / 2,
                                     main_win->BorderTop + LINEHEIGHT +
                                     INTERHEIGHT +
                                     (counter_height () - btn_w + 8) / 2,
                                     btn_w,
                                     btn_w - 8,
                                     face_image);
                     }
                     else
                     {
                        flag_counter = counter_init (main_win, 0, 0,
                                                  levels[current_level].bombs,
                                                  FALSE);
                        time_counter = counter_init (main_win, 6, 0,
                                                  0, FALSE);
                     }
                     if (flag_counter && time_counter)
                     {
                        counter_draw (flag_counter, vis_info);
                        counter_draw (time_counter, vis_info);
                        button_render ();
                        
                        DrawBevelBox (main_win->RPort,
                                  main_win->BorderLeft, main_win->BorderTop,
                                  main_win->Width - main_win->BorderLeft -
                                  main_win->BorderRight, main_win->Height -
                                  main_win->BorderTop - main_win->BorderBottom,
                                  GT_VisualInfo, vis_info,
                                  TAG_DONE);
                     
                        if ((field  = field_init (main_win->RPort,
                                              main_win->BorderLeft +
                                              LINEWIDTH + INTERWIDTH,
                                              main_win->BorderTop +
                                              LINEHEIGHT + INTERHEIGHT +
                                              ((digital_display) ?
                                              counter_height () + INTERHEIGHT :
                                              0),
                                              levels[current_level].rows,
                                              levels[current_level].columns,
                                              levels[current_level].bombs)))
                        {
                           return (BOOL)(((timer_obj = mytimer_create ())) != NULL);
                        }
                     }
                     else
                     {
                        error_requester (NULL, MSG_CREATE_ERROR,
                                         localized_string (MSG_MINEFIELD));
                     }
                  }
                  else
                  {
                     error_requester (NULL, MSG_CREATE_ERROR,
                                      localized_string (MSG_COUNTERS));
                  }
               }
               else
               {
                  error_requester (NULL, MSG_OPEN_ERROR,
                                   localized_string (MSG_WINDOW));
               }
            }
            else
               error_requester (NULL, MSG_GET_ERROR, "VisualInfo");
         }
         else
            error_requester (NULL, MSG_SMALLSCREEN_ERROR, NULL);
      }
      else
         error_requester (NULL, MSG_GET_ERROR, "DrawInfo");
   }
   else
      error_requester (NULL, MSG_LOCKSCREEN_ERROR, NULL);
   
   return FALSE;
}

BOOL
init_menu (void)
{
   if (GfxBase->LibNode.lib_Version < 39L)
   {
      display_colors = FALSE;
      new_menu[ITEM_Quit + ITEM_NoColors + 5].nm_Flags |= CHECKED |
                                                          NM_ITEMDISABLED;
   }
   if ((main_menu = CreateLocMenus (new_menu, vis_info, TAG_DONE)))
   {
      if (LayoutMenus (main_menu, vis_info,
                       GTMN_NewLookMenus, TRUE, TAG_DONE))
      {
         if (SetMenuStrip (main_win, main_menu))
            return TRUE;
         else
            error_requester (main_win, MSG_SETMENU_ERROR, NULL);
      }
      else
         error_requester (main_win, MSG_LAYOUTMENU_ERROR, NULL);
   }
   else
   {
      error_requester (main_win, MSG_CREATE_ERROR,
                       localized_string (MSG_MENU));
   }
   
   return FALSE;
}

void
finalize (void)
{
   finalize_display ();
   CloseLibrary (UtilityBase);
   CloseLibrary (GadToolsBase);
   CloseLibrary ((struct Library *)GfxBase);
   CloseLibrary ((struct Library *)IntuitionBase);
}

void
finalize_display (void)
{
   if (timer_obj)
      timer_free (timer_obj);
   if (field)
      field_free (field);
   if (flag_counter)
      counter_free (flag_counter);
   if (time_counter)
      counter_free (time_counter);
   if (main_win)
   {
      free_images (pub_screen);
      ClearMenuStrip (main_win);
      CloseWindow (main_win);
   }
   FreeMenus (main_menu);
   if (romfont)
      CloseFont (romfont);
   if (vis_info)
      FreeVisualInfo (vis_info);
   if (draw_info)
      FreeScreenDrawInfo (pub_screen, draw_info);
   if (pub_screen)
   {
      free_pens (pub_screen);
      UnlockPubScreen (NULL, pub_screen);
   }
}
