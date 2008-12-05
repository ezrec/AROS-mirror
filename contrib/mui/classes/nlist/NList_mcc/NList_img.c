/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#include "private.h"

struct MUI_CustomClass *NLI_Class = NULL;

static ULONG mNLI_Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  if (data->DoDraw)
    DoSuperMethodA(cl,obj,(Msg) msg);

  return(0);
}


static ULONG mNLI_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NLIData *data;

  if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg) msg)))
    return(0);

  data = INST_DATA(cl,obj);
  data->DoDraw = FALSE;

  return((ULONG) obj);
}


static ULONG mNLI_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLIData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for(tags=((struct opSet *)msg)->ops_AttrList;(tag=(struct TagItem *) NextTagItem(&tags));)
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Image_Spec:
        tag->ti_Tag = TAG_IGNORE;
        data->DoDraw = TRUE;
        MUI_Redraw(obj,MADF_DRAWOBJECT);
        data->DoDraw = FALSE;
        break;
      case MUIA_FillArea:
      case MUIA_Font:
      case MUIA_Background:
        tag->ti_Tag = TAG_IGNORE;
        break;
    }
  }
  return (DoSuperMethodA(cl,obj,msg));
}

DISPATCHERPROTO(NLI_Dispatcher)
{
  DISPATCHER_INIT
  
  switch (msg->MethodID)
  {
    case OM_NEW           : return (  mNLI_New(cl,obj,(APTR)msg));
    case MUIM_HandleInput : return (0);
    case OM_SET           : return (  mNLI_Set(cl,obj,(APTR)msg));
    case MUIM_Draw        : return ( mNLI_Draw(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
  
  DISPATCHER_EXIT
}

struct MUI_CustomClass *NLI_Create(void)
{
  NLI_Class = MUI_CreateCustomClass(NULL, MUIC_Image, NULL, sizeof(struct NLIData), ENTRY(NLI_Dispatcher));
  return (NLI_Class);
}


void NLI_Delete(void)
{
  if (NLI_Class)
    MUI_DeleteCustomClass(NLI_Class);
  NLI_Class = NULL;
}

