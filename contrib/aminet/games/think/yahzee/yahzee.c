/*
 * yahzee.c
 * ========
 * Main module.
 *
 * Copyright © 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <libraries/gadtools.h>
#include "requesters.h"
#include "localize.h"
#include "tooltypes.h"
#include "highscores.h"
#include "rules.h"
#include "layout.h"
#include "layout_const.h"
#include "draw.h"
#include "dice.h"

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>


#ifdef SAS_C
void __regargs _CXBRK (void) {}
#endif


#define PRG_NAME        "Yahzee"
#define VERSION_NO      "2.4"
#define CREATION_YEAR   "1994-1998"
#define AUTHOR          "Håkan L. Younes"
#define MAIL_ADDRESS    "(lorens@hem.passagen.se)"

STRPTR version = "$VER: Yahzee 2.4 (18.7.98)";


struct IntuitionBase  *IntuitionBase = NULL;
struct Library  *GadToolsBase;
struct GfxBase  *GfxBase = NULL;

char   pubscr_name[129];
char   scr_title[81];
char   win_title[81];
struct Screen  *pub_screen = NULL;
APTR   vis_info = NULL;
struct Window  *main_win = NULL;
struct TextFont  *romfont = NULL;
struct Gadget   die_gads[MAX_NUM_DICE];
struct Gadget  *roll_gad = NULL, *disp_gad = NULL, *gt_gads = NULL;
struct NewGadget   new_roll_gad, new_disp_gad;
struct Menu  *main_menu = NULL;
UBYTE   text_pen;

#define ROLLGAD_ID   10

struct NewMenu new_menu[] = {
   { NM_TITLE, (STRPTR)MSG_GAME_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_NEW, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_UNDO, 0, NM_ITEMDISABLED, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_HIGHSCORE, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_ABOUT, 0, 0, 0, 0 },
   {  NM_ITEM, NM_BARLABEL, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_GAME_QUIT, 0, 0, 0, 0 },
   { NM_TITLE, (STRPTR)MSG_SETTINGS_MENU, 0, 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_NUMPLAYERS, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_1, 0, CHECKIT, ~0x01, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_2, 0, CHECKIT, ~0x02, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_3, 0, CHECKIT, ~0x04, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_4, 0, CHECKIT, ~0x08, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_5, 0, CHECKIT, ~0x10, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_NUMPLAYERS_6, 0, CHECKIT, ~0x20, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_SCORING, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_SCORING_DEFAULT, 0, CHECKIT, ~0x1, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_SCORING_AMERICAN, 0, CHECKIT, ~0x2, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_GAMETYPE, 0, 0, 0, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_GAMETYPE_ORIGINAL, 0, CHECKIT, ~0x01, 0 },
   {   NM_SUB, (STRPTR)MSG_SETTINGS_GAMETYPE_MAXI, 0, CHECKIT, ~0x02, 0 },
   {  NM_ITEM, NM_BARLABEL, 0 , 0, 0, 0 },
   {  NM_ITEM, (STRPTR)MSG_SETTINGS_SAVE, 0 , 0, 0, 0 },
   { NM_END,   NULL, 0 , 0, 0, 0 }
};
#define MENU_Game    0
#define ITEM_New     0
#define ITEM_Undo    2
#define ITEM_High    4
#define ITEM_About   5
#define ITEM_Quit    7
#define MENU_Settings        1
#define ITEM_NumPlayers      0
#define ITEM_ScoringMethod   1
#define ITEM_GameType        2
#define ITEM_SaveSettings    4

UBYTE   num_players, chosen_num_players = 1;
UBYTE   chosen_scoring = 0;
UBYTE   chosen_gametype = 0;
struct rules  *rules;

void event_loop (void);
BOOL process_menus (UWORD);
BOOL initialize (void);
BOOL init_display (void);
void init_gadgets (void);
BOOL init_menu (void);
void finalize (void);


int
main (
   int    argc,
   char  *argv[])
{
   pubscr_name[0] = '\0';
   
   handle_startup_msg (argv, (argc == 0));
   if (chosen_num_players < 1)
      chosen_num_players = 1;
   else if (chosen_num_players > 6)
      chosen_num_players = 6;
   num_players = chosen_num_players;
   rules = &game_definition[chosen_gametype][chosen_scoring];
   new_menu[ITEM_Quit + ITEM_NumPlayers + chosen_num_players + 3].nm_Flags |=
                                                                       CHECKED;
   new_menu[ITEM_Quit + ITEM_ScoringMethod +
            chosen_scoring + MAX_PLAYERS + 4].nm_Flags |= CHECKED;
   new_menu[ITEM_Quit + ITEM_GameType +
            chosen_gametype + MAX_PLAYERS + 6].nm_Flags |= CHECKED;
   
   init_locale ("yahzee.catalog");
   if (initialize ())
   {
      load_high_scores (AUTHOR);
      srand48 (time (NULL));
      event_loop ();
      save_high_scores ();
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
   BOOL   quit = FALSE;
   
   new_game ();
   
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
         case IDCMP_GADGETUP:
            if (die_gads[0].Flags & GFLG_DISABLED)
               OffMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_Undo, NOSUB));
            roll_dice ();
            break;
         case IDCMP_MOUSEBUTTONS:
            if (code == SELECTDOWN)
               if (choose_score (mx, my) > 1)
                  OnMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_Undo, NOSUB));
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
               OffMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_Undo, NOSUB));
               quit = !new_game ();
               break;
            case ITEM_Undo:
               OffMenu (main_win, FULLMENUNUM (MENU_Game, ITEM_Undo, NOSUB));
               undo_last_choice ();
               break;
            case ITEM_High:
               display_high_scores (game_number (rules));
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
            case ITEM_NumPlayers:
               if (sub_no != MENUNULL)
                  chosen_num_players = sub_no + 1;
               break;
            case ITEM_ScoringMethod:
               if (sub_no != MENUNULL)
                  chosen_scoring = sub_no;
               break;
            case ITEM_GameType:
               if (sub_no != MENUNULL)
                  chosen_gametype = sub_no;
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
   struct DrawInfo  *draw_info;
   
   sprintf (scr_title, "%s v%s - ©%s %s",
            PRG_NAME, VERSION_NO, CREATION_YEAR, AUTHOR);
   strncpy (win_title, PRG_NAME, 80);
   
   if ((pub_screen =
        LockPubScreen ((pubscr_name[0] == '\0') ? NULL : pubscr_name)) ||
       (pub_screen = LockPubScreen (NULL)))
   {
      draw_info = GetScreenDrawInfo (pub_screen);
      die_images[0].PlaneOnOff = (draw_info) ? draw_info->dri_Pens[SHADOWPEN] :
                                               1;
      init_board_pens ((draw_info) ? draw_info->dri_Pens : NULL);
      text_pen = (draw_info) ? draw_info->dri_Pens[TEXTPEN] : 1;
      FreeScreenDrawInfo (pub_screen, draw_info);
      
      if ((vis_info = GetVisualInfo (pub_screen, TAG_DONE)))
      {
         init_gadgets ();
         if (layout_display (pub_screen, &main_win, rules, num_players))
            return init_menu ();
      }
      else
      {
         msg_requester (NULL, "Error Message", "OK",
                        "Couldn't get VisualInfo!");
      }
   }
   else
   {
      msg_requester (NULL, "Error Message", "OK",
                     "Couldn't lock public screen!");
   }
   
   return FALSE;
}

void
init_gadgets (void)
{
   register UBYTE   n;
   char  *roll_label;
   
   for (n = 0; n < MAX_NUM_DICE; ++n)
   {
      die_gads[n].Width = die_images[0].Width;
      die_gads[n].Height = die_images[0].Height;
      die_gads[n].Flags = GFLG_GADGHIMAGE | GFLG_GADGIMAGE;
      die_gads[n].Activation = GACT_IMMEDIATE | GACT_TOGGLESELECT;
      die_gads[n].GadgetType = GTYP_BOOLGADGET;
      die_gads[n].GadgetRender = &die_images[6];
      die_gads[n].SelectRender = &die_images[0];
      die_gads[n].GadgetText = NULL;
      die_gads[n].MutualExclude = 0L;
      die_gads[n].SpecialInfo = NULL;
      die_gads[n].GadgetID = n + 1;
      die_gads[n].UserData = NULL;
   }
   
   roll_label = localized_string (MSG_ROLL_GAD);
   new_roll_gad.ng_Width = 2 * LINEWIDTH + INTERWIDTH +
                            TextLength (&pub_screen->RastPort,
                                        roll_label, strlen (roll_label));
   new_roll_gad.ng_Height = 2 * LINEHEIGHT + INTERHEIGHT +
                             pub_screen->Font->ta_YSize;
   new_roll_gad.ng_GadgetText = roll_label;
   new_roll_gad.ng_TextAttr = pub_screen->Font;
   new_roll_gad.ng_GadgetID = ROLLGAD_ID;
   new_roll_gad.ng_Flags = 0;
   new_roll_gad.ng_VisualInfo = vis_info;
   new_roll_gad.ng_UserData = NULL;
   
   new_disp_gad.ng_Width = new_roll_gad.ng_Width;
   new_disp_gad.ng_Height = new_roll_gad.ng_Height;
   new_disp_gad.ng_GadgetText = NULL;
   new_disp_gad.ng_TextAttr = pub_screen->Font;
   new_disp_gad.ng_GadgetID = 0;
   new_disp_gad.ng_Flags = 0;
   new_disp_gad.ng_VisualInfo = vis_info;
   new_disp_gad.ng_UserData = NULL;
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
   if (pub_screen)
      UnlockPubScreen (NULL, pub_screen);
   if (GadToolsBase)
      CloseLibrary (GadToolsBase);
   if (GfxBase)
      CloseLibrary ((struct Library *)GfxBase);
   if (IntuitionBase)
      CloseLibrary ((struct Library *)IntuitionBase);
}
