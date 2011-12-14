/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "About.h"
#include "BoardWindow.h"
#include "Cardgame.h"
#include "CButton.h"
#include "CSolitaire.h"
#include "Locales.h"
#include "ProfileManager.h"
#include "Soliton.h"
#include "Settings.h"

#define USE_IMG_CNEW1_COLORS
#include "IMG_CNew1.c"
static struct InternalImage iNew1 = {
  IMG_CNew1_body,         
  IMG_CNEW1_WIDTH,   IMG_CNEW1_HEIGHT,
  IMG_CNEW1_DEPTH,   IMG_CNEW1_COMPRESSION,
  IMG_CNEW1_MASKING, IMG_CNew1_colors
};

#define USE_IMG_CNEW2_COLORS
#include "IMG_CNew2.c"
static struct InternalImage iNew2 = {
  IMG_CNew2_body,         
  IMG_CNEW2_WIDTH,   IMG_CNEW2_HEIGHT,
  IMG_CNEW2_DEPTH,   IMG_CNEW2_COMPRESSION,
  IMG_CNEW2_MASKING, IMG_CNew2_colors
};

#define USE_IMG_CUNDO1_COLORS
#include "IMG_CUndo1.c"
static struct InternalImage iUndo1 = {
  IMG_CUndo1_body,    
  IMG_CUNDO1_WIDTH,   IMG_CUNDO1_HEIGHT,
  IMG_CUNDO1_DEPTH,   IMG_CUNDO1_COMPRESSION,
  IMG_CUNDO1_MASKING, IMG_CUndo1_colors
};

#define USE_IMG_CUNDO2_COLORS
#include "IMG_CUndo2.c"
static struct InternalImage iUndo2 = {
  IMG_CUndo2_body,    
  IMG_CUNDO2_WIDTH,   IMG_CUNDO2_HEIGHT,
  IMG_CUNDO2_DEPTH,   IMG_CUNDO2_COMPRESSION,
  IMG_CUNDO2_MASKING, IMG_CUndo2_colors
};

#define USE_IMG_CSTATS1_COLORS
#include "IMG_CStats1.c"
static struct InternalImage iStats1 = {
  IMG_CStats1_body,     
  IMG_CSTATS1_WIDTH,   IMG_CSTATS1_HEIGHT,
  IMG_CSTATS1_DEPTH,   IMG_CSTATS1_COMPRESSION,
  IMG_CSTATS1_MASKING, IMG_CStats1_colors
};

#define USE_IMG_CSTATS2_COLORS
#include "IMG_CStats2.c"
static struct InternalImage iStats2 = {
  IMG_CStats2_body,     
  IMG_CSTATS2_WIDTH,   IMG_CSTATS2_HEIGHT,
  IMG_CSTATS2_DEPTH,   IMG_CSTATS2_COMPRESSION,
  IMG_CSTATS2_MASKING, IMG_CStats2_colors
};

#define USE_IMG_CSWEEP1_COLORS
#include "IMG_CSweep1.c"
static struct InternalImage iSweep1 = {
  IMG_CSweep1_body,     
  IMG_CSWEEP1_WIDTH,   IMG_CSWEEP1_HEIGHT,
  IMG_CSWEEP1_DEPTH,   IMG_CSWEEP1_COMPRESSION,
  IMG_CSWEEP1_MASKING, IMG_CSweep1_colors
};

#define USE_IMG_CSWEEP2_COLORS
#include "IMG_CSweep2.c"
static struct InternalImage iSweep2 = {
  IMG_CSweep2_body,     
  IMG_CSWEEP2_WIDTH,   IMG_CSWEEP2_HEIGHT,
  IMG_CSWEEP2_DEPTH,   IMG_CSWEEP2_COMPRESSION,
  IMG_CSWEEP2_MASKING, IMG_CSweep2_colors
};

#define USE_IMG_CMOVE1_COLORS
#include "IMG_CMove1.c"
static struct InternalImage iMove1 = {
  IMG_CMove1_body,    
  IMG_CMOVE1_WIDTH,   IMG_CMOVE1_HEIGHT,
  IMG_CMOVE1_DEPTH,   IMG_CMOVE1_COMPRESSION,
  IMG_CMOVE1_MASKING, IMG_CMove1_colors
};

#define USE_IMG_CMOVE2_COLORS
#include "IMG_CMove2.c"
static struct InternalImage iMove2 = {
  IMG_CMove2_body,    
  IMG_CMOVE2_WIDTH,   IMG_CMOVE2_HEIGHT,
  IMG_CMOVE2_DEPTH,   IMG_CMOVE2_COMPRESSION,
  IMG_CMOVE2_MASKING, IMG_CMove2_colors
};

struct BoardWindow_Data
{
  Object *solitaire;
  Object *wingroup;
  Object *BT_Move;
  Object *coolbuttons;
  Object *strip;
  Object *movegroup;
  Object *MEN_Profiles;
  Object *WI_ProfileManager;
};

enum {
  MEN_NEW = 1234, MEN_HELP, MEN_QUIT,
  MEN_SETABACUS, MEN_SETMUI, MEN_ABOUT, MEN_SWEEP, MEN_UNDO, MEN_STATS,
  MEN_MANPROFILES, MEN_PROFILES, MEN_GAME, MEN_KLONDIKE, MEN_FREECELL
};

/****************************************************************************************
  Quit
****************************************************************************************/

static IPTR _Quit(/*struct IClass* cl,*/ Object* obj/*, Msg msg*/)
{
  Object* app = (Object *) xget(obj, MUIA_ApplicationObject);
  struct Settings* s = (struct Settings *) xget(app, MUIA_Soliton_Settings);

  if (!s->reqs ||  MUI_RequestA(app, obj, 0, "Soltion",
                   GetStr(MSG_REQ_GADGETS), GetStr(MSG_REQ_QUIT), NULL) == 1)
  {
    DoMethod(app, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
  }
  return 0;
}


/****************************************************************************************
  NewSettings
****************************************************************************************/

static IPTR _NewSettings(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct BoardWindow_Data* data = (struct BoardWindow_Data *) INST_DATA(cl, obj);
  Object* app = (Object *) xget(obj, MUIA_ApplicationObject);
  struct Settings* s = (struct Settings *) xget(app, MUIA_Soliton_Settings);

  /* set new graphics */
  setatt(data->solitaire, MUIA_Cardgame_MoveSpeed, s->anim * 14 - 8);

  /* set side buttons */
  setatt(data->coolbuttons, MUIA_ShowMe, s->buttons);

  if(s->buttons) /* Cool buttons */
  {
    Object* parent = (Object*)xget(data->BT_Move, MUIA_Parent);
    if((s->indicator && !parent) || (!s->indicator && parent) )
    {
      if(DoMethod(data->movegroup, MUIM_Group_InitChange))
      {
        if(s->indicator)
          DoMethod(data->movegroup, OM_ADDMEMBER, data->BT_Move);
        else
          DoMethod(data->movegroup, OM_REMMEMBER, data->BT_Move);
        DoMethod(data->movegroup, MUIM_Group_ExitChange);
      }
    }
  }

  return 0;
}


/****************************************************************************************
  NewGame
****************************************************************************************/

static IPTR _NewGame(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct BoardWindow_Data* data = (struct BoardWindow_Data *) INST_DATA(cl, obj);
  struct Settings* s = (struct Settings *) xget(_app(obj), MUIA_Soliton_Settings);

  if(!s->reqs || MUI_RequestA(_app(obj), obj, 0, "Soliton",
  GetStr(MSG_REQ_GADGETS), GetStr(MSG_REQ_NEWGAME), NULL) == 1)
  {
    DoMethod(data->solitaire, MUIM_CSolitaire_NewGame);
  }

  return 0;
}


/****************************************************************************************
  New
****************************************************************************************/

static struct NewMenu Menu[] = {
  { NM_TITLE, (STRPTR)MSG_MENU_PROJECT         , 0, 0, 0, (APTR)0            },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_NEW     , 0, 0, 0, (APTR)MEN_NEW      },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_SWEEP   , 0, 0, 0, (APTR)MEN_SWEEP    },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_UNDO    , 0, 0, 0, (APTR)MEN_UNDO     },
  { NM_ITEM,  (STRPTR)NM_BARLABEL              , 0, 0, 0, (APTR)0            },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_STATS   , 0, 0, 0, (APTR)MEN_STATS    },
  { NM_ITEM,  (STRPTR)NM_BARLABEL              , 0, 0, 0, (APTR)0            },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_HELP    , 0, 0, 0, (APTR)MEN_HELP     },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_ABOUT   , 0, 0, 0, (APTR)MEN_ABOUT    },
  { NM_ITEM,  (STRPTR)NM_BARLABEL              , 0, 0, 0, (APTR)0            },
  { NM_ITEM,  (STRPTR)MSG_MENU_PROJECT_QUIT    , 0, 0, 0, (APTR)MEN_QUIT     },

  { NM_TITLE, (STRPTR)MSG_MENU_PROFILES        , 0, 0, 0, (APTR)MEN_PROFILES   },
  { NM_ITEM,  (STRPTR)MSG_MENU_MANPROFILES     , 0, 0, 0, (APTR)MEN_MANPROFILES},
  { NM_ITEM,  (STRPTR)NM_BARLABEL              , 0, 0, 0, (APTR)0              },

  { NM_TITLE, (STRPTR)MSG_MENU_SETTINGS        , 0, 0, 0, (APTR)0            },
  { NM_ITEM,  (STRPTR)MSG_MENU_SETTINGS_SOLITON, 0, 0, 0, (APTR)MEN_SETABACUS},
  { NM_ITEM,  (STRPTR)MSG_MENU_SETTINGS_MUI    , 0, 0, 0, (APTR)MEN_SETMUI   },

  { NM_TITLE, (STRPTR)MSG_MENU_GAME            , 0, 0, 0, (APTR)0            },
  { NM_ITEM, (STRPTR)MSG_MENU_GAME_KLONDIKE   , 0, CHECKIT|CHECKED, ~(1<<0), (APTR)MEN_KLONDIKE },
  { NM_ITEM, (STRPTR)MSG_MENU_GAME_FREECELL   , 0, CHECKIT, ~(1<<1), (APTR)MEN_FREECELL },

  { NM_END ,  (STRPTR)NULL                     , 0, 0, 0, (APTR)0            },
};

static IPTR _New(struct IClass* cl, Object* obj, struct opSet * msg)
{
  Object *BT_New;
  Object *BT_Sweep;
  Object *TX_Score;
  Object *BT_Undo;
  Object *BT_Stats;
  Object *TX_Timer;
  struct BoardWindow_Data tmp;
  struct NewMenu *nm;

  for(nm = Menu; nm->nm_Type != NM_END; ++nm)
  {
    if(nm->nm_Label != NM_BARLABEL)
      nm->nm_Label = GetStr((IPTR)nm->nm_Label);
  }

  tmp.BT_Move = CButtonObject(GetStr(MSG_WIN_MOVE), 
                             GetStr(MSG_WIN_MOVE_HELP), 
                             &iMove1, &iMove2);

  TX_Score = TextObject,
              MUIA_Background   , MUII_BACKGROUND,
              MUIA_Text_PreParse, "\033c",
              MUIA_Text_Contents, "0",
              MUIA_ShortHelp    , GetStr(MSG_SCORE_HELP),
              MUIA_Font         , MUIV_Font_Tiny,
              End;

  TX_Timer = TextObject,
              MUIA_Background   , MUII_BACKGROUND,
              MUIA_Text_PreParse, "\033c",
              MUIA_Text_Contents, "0:00",
              MUIA_ShortHelp    , GetStr(MSG_TIMER_HELP),
              MUIA_Font         , MUIV_Font_Tiny,
              End;

  tmp.WI_ProfileManager = (Object*)GetTagData(MUIA_BoardWindow_ProfileManager , 0, msg->ops_AttrList);

  tmp.strip = MUI_MakeObject(MUIO_MenustripNM, Menu, MUIO_MenustripNM_CommandKeyCheck);
  tmp.MEN_Profiles = MenuObj(tmp.strip, MEN_PROFILES);


  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_Window_Title    , APPNAME,
    MUIA_Window_ID       , MAKE_ID('S','K','M','W'),
    MUIA_HelpNode        , "MAINWINDOW",
    MUIA_Window_Menustrip, tmp.strip,

    WindowContents, tmp.wingroup = HGroup,

        MUIA_InnerLeft    , 0,
        MUIA_InnerTop     , 0,
        MUIA_InnerRight   , 0,
        MUIA_InnerBottom  , 0,
        MUIA_Group_Spacing, 0,

        //  Cool buttons

        Child, tmp.coolbuttons = /*VirtgroupObject,*/ VGroup, ButtonFrame,
          MUIA_Background, MUII_BACKGROUND,

          Child, BT_New      = CButtonObject(GetStr(MSG_WIN_NEW),
                                             GetStr(MSG_WIN_NEW_HELP),
                                             &iNew1, &iNew2),
          Child, BT_Stats    = CButtonObject(GetStr(MSG_WIN_STATISTICS), 
                                             GetStr(MSG_WIN_STATISTICS_HELP),
                                             &iStats1, &iStats2),
          Child, BT_Sweep    = CButtonObject(GetStr(MSG_WIN_SWEEP), 
                                             GetStr(MSG_WIN_SWEEP_HELP),
                                             &iSweep1, &iSweep2),

          Child, tmp.movegroup = VGroup,  // Move group
            Child, BT_Undo     = CButtonObject(GetStr(MSG_WIN_UNDO),
                                               GetStr(MSG_WIN_UNDO_HELP),
                                               &iUndo1, &iUndo2),
            Child, tmp.BT_Move,
            End,
          Child, HVSpace,

          Child, HBarObject,
          Child, VGroup,
            Child, TX_Score,
            Child, TX_Timer,
            End,

          End,  //  Cool buttons

        Child, tmp.solitaire = (Object*)NewObject(CL_CSolitaire->mcc_Class, NULL, 
                  MUIA_CSolitaire_Timer     , TX_Timer,
                  MUIA_CSolitaire_MoveButton, tmp.BT_Move,
                  MUIA_CSolitaire_Score     , TX_Score,
                  TAG_DONE),

      End,
    TAG_MORE, msg->ops_AttrList);

  if(obj)
  {
    Object *gm;
  
    DoMethod(obj         , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , obj                     , 1, MUIM_BoardWindow_Quit);
  
    DoMethod(BT_New      , MUIM_Notify, MUIA_Pressed            , FALSE, obj                     , 1, MUIM_BoardWindow_NewGame);
    DoMethod(BT_Stats    , MUIM_Notify, MUIA_Pressed            , FALSE, MUIV_Notify_Application , 2, MUIM_Soliton_Statistics, FALSE);
    DoMethod(BT_Undo     , MUIM_Notify, MUIA_Pressed            , FALSE, tmp.solitaire           , 1, MUIM_CSolitaire_Undo);
    DoMethod(BT_Sweep    , MUIM_Notify, MUIA_Pressed            , FALSE, tmp.solitaire           , 1, MUIM_CSolitaire_Sweep);
    DoMethod(tmp.BT_Move , MUIM_Notify, MUIA_Pressed            , FALSE, tmp.solitaire           , 1, MUIM_CSolitaire_Move);
    DoMethod(MenuObj(tmp.strip, MEN_SWEEP      ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, tmp.solitaire          , 1, MUIM_CSolitaire_Sweep);
    DoMethod(MenuObj(tmp.strip, MEN_HELP       ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 5, MUIM_Application_ShowHelp, NULL, "Soliton.guide", "MAIN", 0);
    DoMethod(MenuObj(tmp.strip, MEN_QUIT       ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj                    , 1, MUIM_BoardWindow_Quit);
    DoMethod(MenuObj(tmp.strip, MEN_SETABACUS  ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 1, MUIM_Soliton_EditSettings);
    DoMethod(MenuObj(tmp.strip, MEN_SETMUI     ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 2, MUIM_Application_OpenConfigWindow, 0);
    DoMethod(MenuObj(tmp.strip, MEN_ABOUT      ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 1, MUIM_Soliton_About);
    DoMethod(MenuObj(tmp.strip, MEN_NEW        ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj                    , 1, MUIM_BoardWindow_NewGame);
    DoMethod(MenuObj(tmp.strip, MEN_STATS      ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUIV_Notify_Application, 2, MUIM_Soliton_Statistics, FALSE);
    DoMethod(MenuObj(tmp.strip, MEN_UNDO       ), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, tmp.solitaire          , 1, MUIM_CSolitaire_Undo);
    DoMethod(MenuObj(tmp.strip, MEN_MANPROFILES), MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, tmp.WI_ProfileManager  , 1, MUIM_ProfileManager_Open);

    gm = MenuObj(tmp.strip, MEN_KLONDIKE);
    DoMethod(gm, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, tmp.solitaire          , 2, MUIM_CSolitaire_GameMode, GAMEMODE_KLONDIKE);
    setatt(gm, MUIA_ObjectID, ID_MENU_KLONDIKE);
    gm = MenuObj(tmp.strip, MEN_FREECELL);
    DoMethod(gm, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, tmp.solitaire          , 2, MUIM_CSolitaire_GameMode, GAMEMODE_FREECELL);
    setatt(gm, MUIA_ObjectID, ID_MENU_FREECELL);

    DoMethod(obj, MUIM_Notify, MUIA_Window_Activate, MUIV_EveryTime, tmp.solitaire, 3, MUIM_Set, MUIA_Cardgame_TimerRunning, MUIV_TriggerValue);

    setatt(tmp.WI_ProfileManager, MUIA_ProfileManager_ProfileMenu, tmp.MEN_Profiles);
    setatt(tmp.WI_ProfileManager, MUIA_ProfileManager_Cardgame, tmp.solitaire);

    *((struct BoardWindow_Data *) INST_DATA(cl, obj)) = tmp;

    return (IPTR)obj;
  }
  return 0;
}

/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                        : return _New          (cl, obj, (struct opSet *) msg);
    case MUIM_BoardWindow_Quit         : return _Quit         (/*cl,*/ obj/*, msg*/);
    case MUIM_BoardWindow_NewSettings  : return _NewSettings  (cl, obj/*, msg*/);
    case MUIM_BoardWindow_NewGame      : return _NewGame      (cl, obj/*, msg*/);
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_BoardWindow = NULL;

BOOL BoardWindow_Init(void)
{
  if(!(CL_BoardWindow = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, 
  sizeof(struct BoardWindow_Data), _Dispatcher)))
  {
    ErrorReq(MSG_CREATE_BOARDWINDOWCLASS);
    return FALSE;
  }
  return TRUE;
}

void BoardWindow_Exit(void)
{
  if(CL_BoardWindow)
    MUI_DeleteCustomClass(CL_BoardWindow);
}

