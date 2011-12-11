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

#include <stdio.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <libraries/iffparse.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "Locales.h"
#include "Settings.h"
#include "Soliton.h"

#include "IMG_Prefs.c"

struct Settings_Data
{
  Object *CH_Buttons;
  Object *CY_Anim;
  Object *CH_Reqs;
  Object *CH_RekoBack;

  /* Klondike */
  Object *SL_Open;
  Object *CH_Block;
  Object *CH_Autoopen;
  Object *CH_Indicator;

  /* Freecelle */
  Object *SL_Stack;
  Object *CH_EqualColor;

  struct Settings settings; /* only for get */
};

/****************************************************************************************
  Close
****************************************************************************************/

static ULONG Settings_Close(/*struct IClass* cl,*/ Object* obj, struct MUIP_Settings_Close* msg)
{
  Object* app;

  setatt(obj, MUIA_Window_Open, FALSE);
  getatt(obj, MUIA_ApplicationObject, &app);
  switch (msg->typ) 
  {
    case 0: 
    {
      DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV); 
      break;
    }
    case 1: 
    {
      DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENVARC); 
      // Fallthrough
    }
    case 2: 
    {
      DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENV);
      setatt(app, MUIA_Soliton_Settings, xget(obj, MUIA_Settings_Settings));
    }
  }
  return 0;
}

/****************************************************************************************
  Get
****************************************************************************************/

static ULONG Settings_Get(struct IClass* cl, Object* obj, struct opGet* msg)
{
  struct Settings *s;
  struct Settings_Data* data = (struct Settings_Data *) INST_DATA(cl, obj);

  switch (msg->opg_AttrID)
  {
    case MUIA_Settings_Settings:
      s = &data->settings;
      s->buttons    =   (BOOL)xget(data->CH_Buttons   , MUIA_Selected     );
      s->reqs       =   (BOOL)xget(data->CH_Reqs      , MUIA_Selected     );
      s->anim       =    (int)xget(data->CY_Anim      , MUIA_Numeric_Value);
      s->rekoback   =   (BOOL)xget(data->CH_RekoBack  , MUIA_Selected     );

      /* klondike */
      s->block      =   (BOOL)xget(data->CH_Block     , MUIA_Selected     );
      s->open       =  (ULONG)xget(data->SL_Open      , MUIA_Numeric_Value);
      s->autoopen   =   (BOOL)xget(data->CH_Autoopen  , MUIA_Selected     );
      s->indicator  =   (BOOL)xget(data->CH_Indicator , MUIA_Selected     );

      s->stack      =  (ULONG)xget(data->SL_Stack     , MUIA_Numeric_Value);
      s->equalcolor =   (BOOL)xget(data->CH_EqualColor, MUIA_Selected     );

      *(msg->opg_Storage) = (ULONG)s;
      return TRUE;
  }
  return DoSuperMethodA(cl, obj, (Msg)msg);
}

/****************************************************************************************
  New
****************************************************************************************/

static ULONG Settings_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  Object *BT_Save, *BT_Use, *BT_Cancel;
  struct Settings_Data tmp;
  static char* PG_Pages[4];

  PG_Pages[0] = GetStr(MSG_SETTINGS_DESIGN);
  PG_Pages[1] = GetStr(MSG_SETTINGS_KLONDIKE);
  PG_Pages[2] = GetStr(MSG_SETTINGS_FREECELL);
  PG_Pages[3] = NULL;

  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_Window_Title      , GetStr(MSG_SETTINGS_TITLE),
    MUIA_Window_ID         , MAKE_ID('S','S','E','T'),
    MUIA_Window_CloseGadget, FALSE,
    MUIA_HelpNode          , "SETTINGS",
    WindowContents, VGroup,

      Child, HGroup,
        Child, MakeImage(IMG_Prefs_body, IMG_PREFS_WIDTH, IMG_PREFS_HEIGHT, 
                         IMG_PREFS_DEPTH, IMG_PREFS_COMPRESSION, IMG_PREFS_MASKING, 
                         IMG_Prefs_colors),
        Child, TextObject,
          MUIA_Text_Contents, GetStr(MSG_SETTINGS_TOPIC),
          MUIA_Font         , MUIV_Font_Big,
          MUIA_Text_SetMax  , FALSE,
          End,
        End,

      Child, RegisterGroup(PG_Pages),
        MUIA_Register_Frame, TRUE,

        /* Appearance */
        Child, ColGroup(2), 

          Child, MakeLabel1(MSG_SETTINGS_ANIM),
          Child, tmp.CY_Anim = MakeNoneSlider(0, 9, 0, MSG_SETTINGS_ANIM, 
                                                       MSG_SETTINGS_ANIM_HELP),

          Child, MakeLabel1(MSG_SETTINGS_BUTTONS),
          Child, HGroup,
            Child, tmp.CH_Buttons = MakeCheck(MSG_SETTINGS_BUTTONS, TRUE, MSG_SETTINGS_BUTTONS_HELP),
            Child, HVSpace,
            End,

          Child, MakeLabel1(MSG_SETTINGS_REQS),
          Child, HGroup,
            Child, tmp.CH_Reqs = MakeCheck(MSG_SETTINGS_REQS, FALSE, MSG_SETTINGS_REQS_HELP),
            Child, HVSpace,
            End,

          Child, MakeLabel1(MSG_SETTINGS_REKOBACK),
          Child, HGroup,
            Child, tmp.CH_RekoBack = MakeCheck(MSG_SETTINGS_REKOBACK, TRUE, MSG_SETTINGS_REKOBACK_HELP),
            Child, HVSpace,
            End,

          End,

        /* Klondike */
        Child, ColGroup(2), 

          Child, MakeLabel1(MSG_SETTINGS_OPEN),
          Child, tmp.SL_Open = MakeSlider(1, 3, 3, MSG_SETTINGS_OPEN, MSG_SETTINGS_OPEN_HELP),

          Child, MakeLabel1(MSG_SETTINGS_AUTOOPEN),
          Child, HGroup,
            Child, tmp.CH_Autoopen = MakeCheck(MSG_SETTINGS_AUTOOPEN, TRUE, MSG_SETTINGS_AUTOOPEN_HELP),
            Child, HVSpace,
            End,

          Child, MakeLabel1(MSG_SETTINGS_INDICATOR),
          Child, HGroup,
            Child, tmp.CH_Indicator = MakeCheck(MSG_SETTINGS_INDICATOR, TRUE, MSG_SETTINGS_INDICATOR_HELP),
            Child, HVSpace,
            End,

          Child, MakeLabel1(MSG_SETTINGS_BLOCK),
          Child, HGroup,
            Child, tmp.CH_Block = MakeCheck(MSG_SETTINGS_BLOCK, FALSE, MSG_SETTINGS_BLOCK_HELP),
            Child, HVSpace,
            End,
          End,

        /* Freecell */
        Child, ColGroup(2), 

          Child, MakeLabel1(MSG_SETTINGS_STACK),
          Child, tmp.SL_Stack = MakeSlider(0, 2, 0, MSG_SETTINGS_STACK, MSG_SETTINGS_STACK_HELP),

          Child, MakeLabel1(MSG_SETTINGS_EQUALCOLOR),
          Child, HGroup,
            Child, tmp.CH_EqualColor = MakeCheck(MSG_SETTINGS_EQUALCOLOR, FALSE, MSG_SETTINGS_EQUALCOLOR_HELP),
            Child, HVSpace,
            End,

          End,

        End, /* Register Group */
      Child, HGroup,
        Child, BT_Save   = MakeButton(MSG_SETTINGS_SAVE  , MSG_SETTINGS_SAVE_HELP  ),
        Child, BT_Use    = MakeButton(MSG_SETTINGS_USE   , MSG_SETTINGS_USE_HELP   ),
        Child, BT_Cancel = MakeButton(MSG_SETTINGS_CANCEL, MSG_SETTINGS_CANCEL_HELP),
        End,

      End,

    TAG_MORE, msg->ops_AttrList);

  if(obj)
  {
    DoMethod(BT_Cancel, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Settings_Close, 0);
    DoMethod(BT_Use   , MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Settings_Close, 2);
    DoMethod(BT_Save  , MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Settings_Close, 1);

    /* Klondike */
    setatt(tmp.SL_Open      , MUIA_ObjectID, ID_OPEN      );
    setatt(tmp.CH_Block     , MUIA_ObjectID, ID_BLOCK     );
    setatt(tmp.CH_Autoopen  , MUIA_ObjectID, ID_AUTOOPEN  );
    setatt(tmp.CH_Indicator , MUIA_ObjectID, ID_INDICATOR );

    /* Freecell */
    setatt(tmp.SL_Stack     , MUIA_ObjectID, ID_STACK     );
    setatt(tmp.CH_EqualColor, MUIA_ObjectID, ID_EQUALCOLOR);

    setatt(tmp.CH_Buttons   , MUIA_ObjectID, ID_BUTTONS   );
    setatt(tmp.CY_Anim      , MUIA_ObjectID, ID_ANIM      );
    setatt(tmp.CH_Reqs      , MUIA_ObjectID, ID_REQS      );
    setatt(tmp.CY_Anim      , MUIA_Cycle_Active, 2);
    setatt(tmp.CH_RekoBack  , MUIA_ObjectID, ID_REKOBACK  );

    *((struct Settings_Data*)INST_DATA(cl, obj)) = tmp;

    return (ULONG)obj;
  }
  return 0;
}


/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(Settings_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW:              return(Settings_New   (cl, obj, (struct opSet *) msg));
    case OM_GET:              return(Settings_Get   (cl, obj, (struct opGet *) msg));
    case MUIM_Settings_Close: return(Settings_Close (/*cl,*/ obj, (struct MUIP_Settings_Close *) msg));
    case MUIM_Settings_GameMode:
      ((struct Settings_Data *) INST_DATA(cl, obj))->settings.gamemode =
      ((struct MUIP_Settings_GameMode *)msg)->mode;
      break;
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_Settings = NULL;

BOOL Settings_Init(void)
{
  if(!(CL_Settings = MUI_CreateCustomClass(NULL, MUIC_Window, NULL,
  sizeof(struct Settings_Data), Settings_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_SETTINGSCLASS);
    return FALSE;
  }
  return TRUE;
}

void Settings_Exit(void)
{
  if(CL_Settings)
    MUI_DeleteCustomClass(CL_Settings);
}

/****************************************************************************************

                                  None_Slider

****************************************************************************************/

struct MUI_CustomClass *CL_NoneSlider = NULL;

struct NoneSlider_Data
{
  char buf[20];
};

Object* MakeNoneSlider(int min, int max, int val, int text, int help)
{
  return (Object*)NewObject(CL_NoneSlider->mcc_Class, 0,
                                   MUIA_Numeric_Value, val,
                                   MUIA_Numeric_Min  , min,
                                   MUIA_Numeric_Max  , max,
                                   MUIA_ControlChar  , GetControlChar(GetStr(text)),
                                   MUIA_CycleChain   , 1,
                                   MUIA_Numeric_Value, val,
                                   MUIA_ShortHelp    , GetStr(help),
                                   TAG_DONE);
}

DISPATCHER(NoneSlider_Dispatcher)
{
  if (msg->MethodID == MUIM_Numeric_Stringify)
  {
    struct NoneSlider_Data* data = (struct NoneSlider_Data *) INST_DATA(cl,obj);
    struct MUIP_Numeric_Stringify* m = (struct MUIP_Numeric_Stringify *) msg;

    if (m->value == 0)
    {
      strcpy(data->buf, GetStr(MSG_SETTINGS_NOANIM));
    }
    else
    {
      sprintf(data->buf, "%3ld", m->value);
    }
    return (ULONG)data->buf;
  }
  return DoSuperMethodA(cl,obj,msg);
}

BOOL NoneSlider_Init(void)
{
  if(!(CL_NoneSlider = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, 
  sizeof(struct NoneSlider_Data), NoneSlider_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_NONESLIDERCLASS);
    return FALSE;
  }
  return TRUE;
}

void NoneSlider_Exit(void)
{
  if(CL_NoneSlider)
    MUI_DeleteCustomClass(CL_NoneSlider);
}

