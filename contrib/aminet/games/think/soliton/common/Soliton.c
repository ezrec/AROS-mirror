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

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "About.h"
#include "BoardWindow.h"
#include "Locales.h"
#include "ProfileManager.h"
#include "Settings.h"
#include "Soliton.h"
#include "Statistics.h"

struct Soliton_Data
{
  Object *WI_Main, *WI_Settings, *WI_About, *WI_Statistics, *WI_ProfileManager;
};

/****************************************************************************************
  About
****************************************************************************************/

static IPTR Soliton_About(struct IClass* cl, Object* obj, Msg msg)
{
  struct Soliton_Data* data = (struct Soliton_Data *) INST_DATA(cl, obj);
  setatt(data->WI_About, MUIA_Window_RefWindow, data->WI_Main);
  setatt(data->WI_About, MUIA_Window_Open     , TRUE);
  return 0;
}


/****************************************************************************************
  OpenSettings
****************************************************************************************/

static IPTR Soliton_OpenSettings(struct IClass* cl, Object* obj, Msg msg)
{
  struct Soliton_Data* data = (struct Soliton_Data *) INST_DATA(cl, obj);
  setatt(data->WI_Settings, MUIA_Window_Open, TRUE);
  return 0;
}

/****************************************************************************************
  Statistics
****************************************************************************************/

static IPTR Soliton_Statistics(struct IClass* cl, Object* obj, struct MUIP_Soliton_Statistics* msg)
{
  ULONG sigs = 0;
  struct Soliton_Data* data = (struct Soliton_Data *) INST_DATA(cl, obj);

  /*
  **  Nur Statistiken zeigen, Spiel nicht zuende
  */

  if(!msg->winner)
  {
    setatt(data->WI_Statistics, MUIA_Window_Open, TRUE);
    return 0;
  }

  /*
  **  Spiel gewonnen
  */

  setatt(data->WI_Statistics, MUIA_Window_Open, FALSE);
  setatt(obj, MUIA_Application_Sleep, TRUE);

  setatt(data->WI_Statistics, MUIA_Statistics_Winner, TRUE);
  setatt(data->WI_Statistics, MUIA_Window_Open, TRUE);

  setatt(data->WI_Main, MUIA_Window_Activate, FALSE);
  setatt(data->WI_Statistics, MUIA_Window_Activate, TRUE);

  while (DoMethod(_app(obj), MUIM_Application_NewInput, &sigs) != MUIV_Statistics_Close)
  {
    if(sigs)
      sigs = Wait(sigs);
  }
  
  setatt(data->WI_Statistics, MUIA_Statistics_Winner, FALSE);
  setatt(obj, MUIA_Application_Sleep, FALSE);
  return 0;
}

/****************************************************************************************
  New / Dispose
****************************************************************************************/

static IPTR Soliton_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct Soliton_Data tmp;

  tmp.WI_ProfileManager = (Object*)NewObject(CL_ProfileManager->mcc_Class , NULL, TAG_DONE);

  obj = (Object*)DoSuperNew(cl, obj,
    MUIA_Application_Title      , APPNAME,
    MUIA_Application_Version    , VERSION_STRING,
    MUIA_Application_Copyright  , "GPL",
    MUIA_Application_Author     , "Kai Nickel",
    MUIA_Application_Description, GetStr(MSG_APPDESCRIPTION),
    MUIA_Application_Base       , "SOLITON",
    MUIA_Application_HelpFile   , "Soliton.guide",

    MUIA_Application_Window, 
      tmp.WI_Main = (Object*)NewObject(CL_BoardWindow->mcc_Class, NULL, 
                                       MUIA_BoardWindow_ProfileManager, tmp.WI_ProfileManager,
                                       TAG_DONE),

    MUIA_Application_Window, tmp.WI_ProfileManager,

    MUIA_Application_Window, 
      tmp.WI_Settings = (Object*)NewObject(CL_Settings->mcc_Class   , NULL, TAG_DONE),

    MUIA_Application_Window, 
      tmp.WI_Statistics = (Object*)NewObject(CL_Statistics->mcc_Class, NULL, TAG_DONE),

    MUIA_Application_Window, 
      tmp.WI_About = (Object*)NewObject(CL_About->mcc_Class   , NULL, 
                                        MUIA_Window_RefWindow , tmp.WI_Main,
                                        TAG_DONE),

    TAG_MORE, msg->ops_AttrList);

  if(obj) 
  {
    DoMethod(tmp.WI_ProfileManager, MUIM_ProfileManager_LoadProfiles);

    DoMethod(obj, MUIM_Application_Load, MUIV_Application_Load_ENVARC);
    DoMethod(obj, MUIM_Application_Save, MUIV_Application_Save_ENV);

    setatt(obj, MUIA_Soliton_Settings, xget(tmp.WI_Settings, MUIA_Settings_Settings)); 

    setatt(tmp.WI_Main, MUIA_Window_Open, TRUE);

    *((struct Soliton_Data*)INST_DATA(cl, obj)) = tmp;

    if(!xget(tmp.WI_Main, MUIA_Window_Open))
    {
      ErrorReq(MSG_CREATE_MAINWINDOW);
      MUI_DisposeObject(obj);
      return (IPTR)NULL;
    }

    return (IPTR)obj;
  }
  return 0;
}


/****************************************************************************************
  Get / Set
****************************************************************************************/

static IPTR Soliton_Get(struct IClass* cl, Object* obj, struct opGet* msg)
{
  struct Soliton_Data* data = (struct Soliton_Data *) INST_DATA(cl, obj);
  switch (msg->opg_AttrID)
  {

    /*
    **  MUIA_Soliton_Settings
    */

    case MUIA_Soliton_Settings:
    {
      struct Settings* s = (struct Settings*)xget(data->WI_Settings, MUIA_Settings_Settings);
      *(msg->opg_Storage) = (IPTR)s;
      return TRUE;
    }
  }
  return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR Soliton_Set(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct Soliton_Data* data = (struct Soliton_Data*)INST_DATA(cl, obj);
  struct TagItem *tag;

  /*
  **  MUIA_Soliton_Settings
  */

  if((tag = FindTagItem(MUIA_Soliton_Settings, msg->ops_AttrList)))
  {
    DoMethod(data->WI_Main, MUIM_BoardWindow_NewSettings);
    return TRUE;
  }

  /*
  **  MUIA_Soliton_Stats
  */

  if((tag = FindTagItem(MUIA_Soliton_Stats, msg->ops_AttrList)))
  {
    // nur noch an Stat-Fenster weiterleiten
    setatt(data->WI_Statistics, MUIA_Soliton_Stats, tag->ti_Data);
    return TRUE;
  }

  /*
  **  MUIA_Soliton_Profile
  */

  if((tag = FindTagItem(MUIA_Soliton_Profile, msg->ops_AttrList)))
  {
    setatt(data->WI_Settings, MUIA_Soliton_Profile, tag->ti_Data);
    DoMethod(data->WI_Main, MUIM_BoardWindow_NewSettings);
    return TRUE;
  }
  
  return DoSuperMethodA(cl, obj, (Msg)msg);
}


/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(Soliton_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                   : return(Soliton_New          (cl, obj, (struct opSet*)msg));
    case OM_GET                   : return(Soliton_Get          (cl, obj, (struct opGet*)msg));
    case OM_SET                   : return(Soliton_Set          (cl, obj, (struct opSet*)msg));
    case MUIM_Soliton_EditSettings: return(Soliton_OpenSettings (cl, obj, msg));
    case MUIM_Soliton_Statistics  : return(Soliton_Statistics   (cl, obj, (struct MUIP_Soliton_Statistics*)msg));
    case MUIM_Soliton_About       : return(Soliton_About        (cl, obj, msg));
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_Soliton = NULL;

BOOL Soliton_Init(void)
{
  if(!(CL_Soliton = MUI_CreateCustomClass(NULL, MUIC_Application, NULL, sizeof(struct Soliton_Data), Soliton_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_SOLITONCLASS);
    return FALSE;
  }
  return TRUE;
}

void Soliton_Exit(void)
{
  if(CL_Soliton)
    MUI_DeleteCustomClass(CL_Soliton);
}

