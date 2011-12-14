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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>         /* strlen */

#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "CButton.h"
#include "Locales.h"

struct CButton_Data
{
  BOOL    visible;
  char    *text;
  Object  *img1, *img2, *gr1, *gr2, *txt1, *txt2;
  BOOL    image;
};


/****************************************************************************************
  Active
****************************************************************************************/

static IPTR CButton_Active(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct CButton_Data* data = (struct CButton_Data *) INST_DATA(cl, obj);

  if (data->visible)
  {
    BOOL sel = (BOOL)xget(obj, MUIA_Selected);
    if (sel)
      setatt(obj, MUIA_Group_ActivePage, 1);
    else
      setatt(obj, MUIA_Group_ActivePage, 0);
  }

  return TRUE;
}


/****************************************************************************************
  Setup
****************************************************************************************/

static IPTR CButton_Setup(struct IClass* cl, Object* obj, Msg msg)
{
  struct CButton_Data *data = (struct CButton_Data *) INST_DATA(cl,obj);
  struct Screen* scr = _screen(obj);
  BOOL soll = TRUE;

  if(scr->Height < 290 || scr->Width < 640)
    soll = FALSE; 

  if(soll && !data->image)
  {
    if(DoMethod(data->gr1, MUIM_Group_InitChange))
    {
      DoMethod(data->gr1, OM_ADDMEMBER, data->img1);
      DoMethod(data->gr1, MUIM_Group_Sort, data->img1, data->txt1, NULL);
      DoMethod(data->gr1, MUIM_Group_ExitChange);
    }
    if(DoMethod(data->gr2, MUIM_Group_InitChange))
    {
      DoMethod(data->gr2, OM_ADDMEMBER, data->img2);
      DoMethod(data->gr2, MUIM_Group_Sort, data->img2, data->txt2, NULL);
      DoMethod(data->gr2, MUIM_Group_ExitChange);
    }
  }

  if(!soll && data->image)
  {
    if(DoMethod(data->gr1, MUIM_Group_InitChange))
    {
      DoMethod(data->gr1, OM_REMMEMBER, data->img1);
      DoMethod(data->gr1, MUIM_Group_ExitChange);
    }
    if(DoMethod(data->gr2, MUIM_Group_InitChange))
    {
      DoMethod(data->gr2, OM_REMMEMBER, data->img2);
      DoMethod(data->gr2, MUIM_Group_ExitChange);
    }
  }
  data->image = soll;

  return DoSuperMethodA(cl, obj, msg);
}


/****************************************************************************************
  New / Dispose
****************************************************************************************/

static IPTR CButton_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct CButton_Data tmp;
  char *text;
  int i, j;
  long cc = 0;

  text     = (char *) GetTagData(MUIA_CButton_Text, 0, msg->ops_AttrList);
  tmp.img1 = (Object *) GetTagData(MUIA_CButton_Img1, 0, msg->ops_AttrList);
  tmp.img2 = (Object *) GetTagData(MUIA_CButton_Img2, 0, msg->ops_AttrList);
  tmp.text = (char *) AllocVec(strlen(text)+1, MEMF_PUBLIC);

  for(i = j = 0; i < strlen(text); i++)
  {
    if(text[i] == '_')
      cc = ToLower(text[i+1]);
    else
      tmp.text[j++] = text[i];
  }
  tmp.text[j] = 0;

  obj = (Object*)DoSuperNew(cl, obj, 
    MUIA_InputMode     , MUIV_InputMode_RelVerify,
    MUIA_Selected      , MUIV_InputMode_RelVerify,
    MUIA_ShowSelState  , FALSE,
    MUIA_InnerTop     , 3,
    MUIA_InnerBottom  , 3,
    MUIA_ControlChar  , cc,

    MUIA_Group_PageMode, TRUE,

    Child, tmp.gr1 = VGroup,
      MUIA_ShowSelState  , FALSE,
      Child, tmp.txt1 = TextObject,
        MUIA_Text_PreParse   , "\033c",
        MUIA_Text_Contents   , tmp.text,
        MUIA_Text_HiChar     , cc,
        MUIA_Font            , MUIV_Font_Tiny,
        MUIA_Text_SetMax     , TRUE,
        MUIA_ShowSelState    , FALSE,
        End,
      End,

    Child, tmp.gr2 = VGroup,
      MUIA_ShowSelState  , FALSE,
      Child, tmp.txt2 = TextObject,
        MUIA_Text_PreParse   , "\0333\033c",
        MUIA_Text_Contents   , tmp.text,
        MUIA_Text_HiChar     , cc,
        MUIA_Font            , MUIV_Font_Tiny,
        MUIA_Text_SetMax     , TRUE,
        MUIA_ShowSelState    , FALSE,
        End,
      End,

    Child, RectangleObject,
      MUIA_ShowSelState, FALSE,
      End,

    TAG_MORE, msg->ops_AttrList);

  if(!obj)
    return 0;

  tmp.visible = TRUE;
  tmp.image = FALSE;
  *((struct CButton_Data *)INST_DATA(cl,obj)) = tmp;

  DoMethod(obj, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, 
           obj, 1, MUIM_CButton_Active);
  DoMethod(obj, MUIM_Notify, MUIA_Pressed, TRUE,
           obj, 1, MUIM_CButton_Active);

  return (IPTR)obj;
}

static IPTR CButton_Dispose(struct IClass* cl, Object* obj, Msg msg)
{
  struct CButton_Data* data = (struct CButton_Data *) INST_DATA(cl,obj);

  FreeVec(data->text);

  if(!data->image)
  {
    MUI_DisposeObject(data->img1);
    MUI_DisposeObject(data->img2);
  }

  return DoSuperMethodA(cl, obj, msg);
}


/****************************************************************************************
  Set
****************************************************************************************/

static IPTR CButton_Set(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct CButton_Data* data = (struct CButton_Data *) INST_DATA(cl, obj);
  struct TagItem *tag;

  if((tag = FindTagItem(MUIA_CButton_Visible, msg->ops_AttrList)))
  {
    if(tag->ti_Data && !data->visible)
    {
      data->visible = TRUE;
      setatt(obj, MUIA_Group_ActivePage, 0 );
    }
    else if(!tag->ti_Data && data->visible)
    {
      data->visible = FALSE;
      setatt(obj, MUIA_Group_ActivePage, 2 );
    }
    return TRUE;
  }

  return DoSuperMethodA(cl, obj, (Msg)msg);
}


/****************************************************************************************
  Dispatcher / Ini / Exit
****************************************************************************************/

DISPATCHER(CButton_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW             : return(CButton_New    (cl, obj, (struct opSet *)msg));
    case OM_DISPOSE         : return(CButton_Dispose(cl, obj, msg));
    case OM_SET             : return(CButton_Set    (cl, obj, (struct opSet *)msg));
    case MUIM_Setup         : return(CButton_Setup  (cl, obj, msg));
    case MUIM_CButton_Active: return(CButton_Active (cl, obj/*, msg*/));
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_CButton = NULL;

BOOL CButton_Init()
{
  if(!(CL_CButton = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, 
  sizeof(struct CButton_Data), CButton_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_CBUTTONCLASS);
    return FALSE;
  }
  return TRUE;
}

void CButton_Exit()
{
  if(CL_CButton)
    MUI_DeleteCustomClass(CL_CButton);
}

Object* CButtonObject(char* text, char* help, struct InternalImage *img1, struct InternalImage *img2)
{
  Object *i1 = MakeImage(img1->data, img1->w, img1->h, img1->d, 
                         img1->compression, img1->mask, img1->colors),
         *i2 = MakeImage(img2->data, img2->w, img2->h, img2->d, 
                         img2->compression, img2->mask, img2->colors);
  Object* res = (Object*)NewObject(
    CL_CButton->mcc_Class , NULL, 
    MUIA_CButton_Text     , text,
    MUIA_ShortHelp        , help,
    MUIA_CButton_Img1     , i1,
    MUIA_CButton_Img2     , i2,
    TAG_DONE);
  return res;
}

