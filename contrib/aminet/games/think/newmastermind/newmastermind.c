/*
 * newmastermind.c
 * ===============
 * Main module.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <graphics/gfxbase.h>
#include <intuition/gadgetclass.h>
#include "requesters.h"
#include "localize.h"
#include "tooltypes.h"
#include "layout.h"
#include "draw.h"
#include "game.h"

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/alib.h>

#ifdef SAS_C
void __regargs _CXBRK (void) {}
#endif


#define PRG_NAME        "NewMasterMind"
#define VERSION_NO      "1.2"
#define CREATION_YEAR   "1994-1998"
#define AUTHOR          "Håkan L. Younes"
#define MAIL_ADDRESS    "(lorens@hem.passagen.se)"

STRPTR version = "$VER: NewMasterMind 1.2 (9.12.98)";


struct IntuitionBase  *IntuitionBase = NULL;
struct Library  *GadToolsBase;
struct GfxBase  *GfxBase = NULL;

struct TextAttr   topaz8 = {
   "topaz.font", 8, 0, FPF_ROMFONT
};

struct NewMenu new_menu[] = {
   { NM_TITLE, (STRPTR)MSG_GAME_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_NEW, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_ABOUT, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_QUIT, 0, 0, 0, 0 },
   { NM_TITLE, (STRPTR)MSG_SETTINGS_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_COLORDISPLAY, 0, CHECKIT | MENUTOGGLE, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_OPPONENT, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_HUMAN, 0, CHECKIT, ~0x01, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_COMPUTER, 0, CHECKIT, ~0x02, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_CORRECTION, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_CHILDREN, 0, CHECKIT, ~0x1, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_ADULTS, 0, CHECKIT, ~0x2, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_NUMCOLORS, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMCOLORS_4, 0, CHECKIT, ~0x01, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMCOLORS_6, 0, CHECKIT, ~0x02, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMCOLORS_8, 0, CHECKIT, ~0x04, 0 },
   {  NM_ITEM, NM_BARLABEL, 0 , 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_SAVE, 0 , 0, 0, 0 },
   { NM_END,   NULL, 0 , 0, 0, 0 }
};
#define MENU_Game    0
#define ITEM_New     0
#define ITEM_About   2
#define ITEM_Quit    4
#define MENU_Settings       1
#define ITEM_ColorDisplay   0
#define ITEM_Opponent       2
#define ITEM_Correction     3
#define ITEM_NumColors      4
#define ITEM_SaveSettings   6

char   pubscr_name[129];
char   scr_title[81];
struct Screen  *pub_screen = NULL;
struct DrawInfo  *draw_info = NULL;
APTR   vis_info = NULL;
struct Window  *main_win = NULL;
struct TextFont  *romfont = NULL;
struct Gadget  *ok_gad = NULL, *gt_gads = NULL;
struct NewGadget   new_ok_gad;
struct Menu  *main_menu = NULL;

#define OKGAD_ID   10


BOOL    chosen_display = TRUE;
UBYTE   num_colors, chosen_num_colors = 6;
UBYTE   correction, chosen_correction = ADULTS;
UBYTE   opponent, chosen_opponent = COMPUTER;


BOOL initialize (void);
BOOL init_display (void);
BOOL init_gadgets (void);
BOOL init_menu (void);
void event_loop (void);
BOOL process_menus (UWORD code);
void finalize (void);


int
main (
   int    argc,
   char  *argv[])
{
   pubscr_name[0] = '\0';
   
   handle_startup_msg (argv, (argc == 0));
   opponent = chosen_opponent;
   correction = chosen_correction;
   if (chosen_num_colors < 4 || chosen_num_colors == 5)
      chosen_num_colors = 4;
   else if (chosen_num_colors > 8 || chosen_num_colors == 7)
      chosen_num_colors = 8;
   num_colors = chosen_num_colors;
   new_menu[ITEM_Quit + ITEM_Opponent + opponent + 4].nm_Flags |= CHECKED;
   new_menu[ITEM_Quit + ITEM_Correction + correction + 6].nm_Flags |= CHECKED;
   new_menu[ITEM_Quit + ITEM_NumColors +
            (num_colors / 2 - 2) + 8].nm_Flags |= CHECKED;
   
   init_locale ("newmastermind.catalog");
   srand48 (time (NULL));
   if (initialize ())
   {
      event_loop ();
   }
   finalize ();
   finalize_locale ();
   
   return 0;
}

void
event_loop (void)
{
   struct IntuiMessage  *msg;
   ULONG   class;
   UWORD   code;
   WORD    mx, my;
   BOOL    quit;
   
   quit = !new_game ();
   
   while (!quit)
   {
      WaitPort (main_win->UserPort);
      while ((msg = GT_GetIMsg (main_win->UserPort)))
      {
         class = msg->Class;
         code = msg->Code;
         mx = msg->MouseX;
         my = msg->MouseY;
         GT_ReplyIMsg (msg);
         switch (class)
         {
         case IDCMP_MOUSEBUTTONS:
            if (code == SELECTDOWN && current_row > 0)
               handle_mouseclick (mx, my);
            break;
         case IDCMP_MOUSEMOVE:
            if (active_color != NOPEN && current_row > 0)
               handle_mousemove (mx, my);
            break;
         case IDCMP_GADGETUP:
            end_of_turn ();
            break;
         case IDCMP_MENUPICK:
            quit = process_menus (code);
            break;
         case IDCMP_CLOSEWINDOW:
            quit = TRUE;
            break;
         case IDCMP_REFRESHWINDOW:
            GT_BeginRefresh (main_win);
            GT_EndRefresh (main_win, TRUE);
            break;
         }
      }
   }
}

BOOL
process_menus (
   UWORD   code)
{
   UWORD   menu_no, item_no, sub_no;
   struct MenuItem  *item;
   BOOL quit = FALSE;
   char   buf_1[81], buf_2[256];
   
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
               quit = !new_game ();
               break;
            case ITEM_About:
               sprintf (buf_1, localized_string (MSG_ABOUT_REQTITLE),
                        PRG_NAME);
               sprintf (buf_2, localized_string (MSG_ABOUT_REQMSG),
                        PRG_NAME, VERSION_NO, AUTHOR, MAIL_ADDRESS,
                        CREATION_YEAR, AUTHOR);
               msg_requester (main_win, buf_1,
                              localized_string (MSG_CONTINUE_GAD), buf_2);
               break;
            case ITEM_Quit:
               quit = TRUE;
               break;
            }
            break;
         case MENU_Settings:
            switch (item_no)
            {
            case ITEM_ColorDisplay:
               chosen_display = color_display = !color_display;
               draw_panel (main_win, num_colors, num_colors, active_color);
               draw_playfield (main_win, vis_info, guess, answer,
               current_row == 11 || current_row == 0);
               break;
            case ITEM_Opponent:
               if (sub_no != MENUNULL)
                  chosen_opponent = sub_no;
               break;
            case ITEM_Correction:
               if (sub_no != MENUNULL)
                  chosen_correction = sub_no;
               break;
            case ITEM_NumColors:
               if (sub_no != MENUNULL)
                  chosen_num_colors = sub_no * 2 + 4;
               break;
            case ITEM_SaveSettings:
               save_tooltypes ();
               break;
            }
            break;
         }
      }
      code = item->NextSelect;
   }
   
   return quit;
}

BOOL
initialize (void)
{
   if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary ("intuition.library", 37L)))
   {
      if ((GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 37L)))
      {
         if ((GadToolsBase = OpenLibrary ("gadtools.library", 37L)))
            return init_display ();
         else
         {
            msg_requester (NULL, "Error Message", "OK",
                           "Couldn't open gadtools.library!");
         }
      }
      else
      {
         msg_requester (NULL, "Error Message", "OK",
                        "Couldn't open graphics.library!");
      }
   }
   else
      printf ("Couldn't open intuition.library!");
   
   return FALSE;
}

BOOL
init_display (void)
{
   UWORD   win_w, win_h;
   
   sprintf (scr_title, "%s v%s - ©%s %s",
            PRG_NAME, VERSION_NO, CREATION_YEAR, AUTHOR);
   
   if ((pub_screen =
        LockPubScreen ((pubscr_name[0] == '\0') ? NULL : pubscr_name)) ||
       (pub_screen = LockPubScreen (NULL)))
   {
      if ((draw_info = GetScreenDrawInfo (pub_screen)))
      {
         gui_pens = draw_info->dri_Pens;
         init_pens (pub_screen);
         if (!color_display)
         {
            new_menu[ITEM_Quit + ITEM_ColorDisplay + 3].nm_Flags |=
                                                               NM_ITEMDISABLED;
         }
         if (color_display && !chosen_display)
            color_display = chosen_display;
         if (!color_display)
            new_menu[ITEM_Quit + ITEM_ColorDisplay + 3].nm_Flags |= CHECKED;

         if (!(romfont = OpenFont (&topaz8)))
         {
            msg_requester (NULL, "Init Error", "OK",
                           "Couldn't open topaz.font!");
            return FALSE;
         }
         if (window_extent (pub_screen, 8, &win_w, &win_h))
         {
            if ((vis_info = GetVisualInfo (pub_screen, TAG_DONE)))
            {
               main_win = OpenWindowTags (NULL,
                           WA_Left, (pub_screen->Width - win_w) / 2,
                           WA_Top, (pub_screen->Height - win_h) / 2,
                           WA_Width, win_w,
                           WA_Height, win_h,
                           WA_AutoAdjust, FALSE,
                           WA_Activate, TRUE,
                           WA_CloseGadget, TRUE,
                           WA_DepthGadget, TRUE,
                           WA_DragBar, TRUE,
                           WA_Title, (IPTR)PRG_NAME,
                           WA_ScreenTitle, (IPTR)scr_title,
                           WA_PubScreen, (IPTR)pub_screen,
                           WA_NewLookMenus, TRUE,
                           WA_ReportMouse, TRUE,
                           WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_MENUPICK |
                                     IDCMP_MOUSEMOVE | IDCMP_CLOSEWINDOW |
                                     IDCMP_REFRESHWINDOW | BUTTONIDCMP,
                           TAG_DONE);
               if (main_win)
               {
                  SetFont (main_win->RPort, romfont);
                  init_playfield_grid (main_win);
                  draw_panel (main_win, num_colors, chosen_num_colors, NOPEN);
                  
                  return (BOOL)(init_gadgets () && init_menu ());
               }
               else
               {
                  msg_requester (NULL, "Init Error", "OK",
                                 "Couldn't open window!");
               }
            }
            else
            {
               msg_requester (NULL, "Init Error", "OK",
                              "Couldn't get VisualInfo!");
            }
         }
         else
         {
            msg_requester (NULL, "Init Error", "OK",
                           "Screen is too small!");
         }
      }
      else
      {
         msg_requester (NULL, "Init Error", "OK",
                        "Couldn't get DrawInfo!");
      }
   }
   else
   {
      msg_requester (NULL, "Init Error", "OK",
                     "Couldn't lock public screen!");
   }
   
   return FALSE;
}

BOOL
init_gadgets (void)
{
   char  *label;
   
   label = localized_string (MSG_OK_GAD);
   gadget_extent (pub_screen,
                  (UWORD *)&new_ok_gad.ng_Width,
                  (UWORD *)&new_ok_gad.ng_Height);
   new_ok_gad.ng_LeftEdge = playfield_grid[0][1].left +
                            num_colors * (BRICK1_W + INTERWIDTH);
   new_ok_gad.ng_TopEdge = playfield_grid[0][1].top +
                     (playfield_grid[0][1].height - new_ok_gad.ng_Height) / 2;
   new_ok_gad.ng_GadgetText = label;
   new_ok_gad.ng_TextAttr = pub_screen->Font;
   new_ok_gad.ng_GadgetID = OKGAD_ID;
   new_ok_gad.ng_Flags = 0;
   new_ok_gad.ng_VisualInfo = vis_info;
   new_ok_gad.ng_UserData = NULL;
   
   ok_gad = CreateContext (&gt_gads);
   ok_gad = CreateGadget (BUTTON_KIND, ok_gad, &new_ok_gad,
                          GA_Disabled, TRUE, TAG_DONE);
   if (ok_gad)
   {
      AddGList (main_win, gt_gads, -1, -1, NULL);
      RefreshGadgets (gt_gads, main_win, NULL);
      GT_RefreshWindow (main_win, NULL);
      
      return TRUE;
   }
   else
      msg_requester (NULL, "Init Error", "OK", "Couldn't create gadgets!");
   
   return FALSE;
}

BOOL
init_menu (void)
{
   if ((main_menu = CreateLocMenus (new_menu, vis_info, TAG_DONE)))
   {
      if (LayoutMenus (main_menu, vis_info,
                       GTMN_NewLookMenus, TRUE, TAG_DONE))
      {
         if (SetMenuStrip (main_win, main_menu))
            return TRUE;
         else
         {
            msg_requester (main_win, "Error Message", "OK",
                           "Couldn't set menustrip!");
         }
      }
      else
      {
         msg_requester (main_win, "Error Message", "OK",
                        "Couldn't layout menus!");
      }
   }
   else
   {
      msg_requester (main_win, "Error Message", "OK",
                     "Couldn't create menus!");
   }
   
   return FALSE;
}

void
finalize (void)
{
   if (main_win)
   {
      ClearMenuStrip (main_win);
      CloseWindow (main_win);
   }
   if (main_menu)
      FreeMenus (main_menu);
   if (gt_gads)
      FreeGadgets (gt_gads);
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
   CloseLibrary (GadToolsBase);
   CloseLibrary ((struct Library *)GfxBase);
   CloseLibrary ((struct Library *)IntuitionBase);
}
