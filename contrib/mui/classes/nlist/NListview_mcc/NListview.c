/***************************************************************************

 NListview.mcc - New Listview MUI Custom Class
 Registered MUI class, Serial Number: 1d51 (0x9d510020 to 0x9d51002F)

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/keymap.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>

#include <proto/muimaster.h>

#include "private.h"
#include "rev.h"

#ifdef __GNUC__
  #if defined(__AROS__)
    #include "../NListviews_mcp/NListviews_mcp.h"
  #elif defined(__PPC__)
    #pragma pack(2)
    #include "../NListviews_mcp/NListviews_mcp.h"
    #pragma pack()
  #else
    #include "../NListviews_mcp/NListviews_mcp.h"
  #endif
#else
  #include "/NListviews_mcp/NListviews_mcp.h"
#endif

#include "mcc_common.h"

#ifndef MUI_EHF_GUIMODE
#define MUI_EHF_GUIMODE     (1<<1)  /* set this if you dont want your handler to be called */
                                    /* when your object is disabled or invisible */
#endif

#include <MUI/NList_mcc.h>

#define imgbt(nr)\
  ImageObject,\
    ImageButtonFrame,\
    MUIA_Background, MUII_ButtonBack,\
    MUIA_InputMode , MUIV_InputMode_RelVerify,\
    MUIA_Image_Spec, (long) nr,\
  End

static LONG IMsgToChar(struct IntuiMessage *imsg, ULONG dccode, ULONG dcquali)
{
	UBYTE buf[4];
	struct InputEvent ie = { 0, IECLASS_RAWKEY, 0, 0, 0 };

	if (imsg->Class == IDCMP_RAWKEY)
	{
		ie.ie_Code         = imsg->Code & ~dccode;
		ie.ie_Qualifier    = imsg->Qualifier & ~dcquali;
		ie.ie_EventAddress = (APTR *)*((ULONG *)imsg->IAddress);
		ie.ie_TimeStamp    = *((struct timeval *)&imsg->Seconds);

		if ((MapRawKey(&ie,buf,3,0)>0))
			return((LONG)buf[0]);
	}

	return(-1);
}

static ULONG mNLV_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
  struct NLVData *data = INST_DATA(cl,obj);
  struct IntuiMessage *imsg = msg->imsg;

  if(imsg && data->LI_NList)
  {
    switch(imsg->Class)
    {
      case IDCMP_RAWKEY:
      {
        if (data->ControlChar && IMsgToChar(imsg, 0, (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)) == data->ControlChar)
				{
          set(data->LI_NList, MUIA_NList_Active, imsg->Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT) ? MUIV_NList_Active_Up : MUIV_NList_Active_Down);
        }
      }
      break;
    }
  }

  return 0;
}


static ULONG mNLV_HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct NLVData *data = INST_DATA(cl,obj);

  if (msg->muikey != MUIKEY_NONE)
  {
    if (data->LI_NList)
    {
      DoMethod(data->LI_NList, MUIM_HandleInput, msg->imsg, msg->muikey);
      return (0);
    }
  }

  return (DoSuperMethodA(cl,obj,(Msg) msg));
}



#define VertScrollBar \
    { data->PR_Vert = MUI_NewObject(MUIC_Scrollbar, TAG_DONE); \
    }

#define HorizScrollBar \
    { data->PR_Horiz = MUI_NewObject(MUIC_Scrollbar, MUIA_Group_Horiz, TRUE, TAG_DONE); \
    }


static void NLV_Scrollers(Object *obj,struct NLVData *data,LONG vert,LONG horiz)
{
  LONG scrollers = 0;

  if (vert & 0x0F)
  { data->Vert_ScrollBar = vert & 0x0F;

    if (data->Vert_ScrollBar == MUIV_NListview_VSB_Default)
    {
      LONG *scrollbar;

      if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NListview_VSB, (LONG) (&scrollbar)))
        data->VertSB = *scrollbar;
      else
        data->VertSB = MUIV_NListview_VSB_Always;
    }
    else
      data->VertSB = data->Vert_ScrollBar;

    switch (data->VertSB)
    {
      case MUIV_NListview_VSB_Always :
      case MUIV_NListview_VSB_Left :
        scrollers |= MUIV_NListview_VSB_On;
      case MUIV_NListview_VSB_Auto :
      case MUIV_NListview_VSB_FullAuto :
        if (!data->PR_Vert)
        { VertScrollBar;
          if (data->PR_Vert)
          { data->Vert_Attached = FALSE;
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Prop_Entries,MUIV_EveryTime,
              data->PR_Vert, 3, MUIM_NoNotifySet,MUIA_Prop_Entries,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Prop_Visible,MUIV_EveryTime,
              data->PR_Vert, 3, MUIM_NoNotifySet,MUIA_Prop_Visible,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Prop_First,MUIV_EveryTime,
              data->PR_Vert, 3, MUIM_NoNotifySet,MUIA_Prop_First,MUIV_TriggerValue);
            DoMethod(data->PR_Vert, MUIM_Notify, MUIA_Prop_First,MUIV_EveryTime,
              data->LI_NList, 3, MUIM_NoNotifySet,MUIA_NList_Prop_First,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_VertDeltaFactor,MUIV_EveryTime,
              data->PR_Vert, 3, MUIM_NoNotifySet,MUIA_Prop_DeltaFactor,MUIV_TriggerValue);
            /*set(data->PR_Vert,MUIA_Prop_DoSmooth, TRUE);*/
          }
        }
        break;
      case MUIV_NListview_VSB_None :
        scrollers |= MUIV_NListview_VSB_Off;
        break;
    }
  }

  if (horiz & 0x0F)
  { data->Horiz_ScrollBar = horiz & 0x0F;

    if (data->Horiz_ScrollBar == MUIV_NListview_HSB_Default)
    { LONG *scrollbar;
      if (DoMethod(obj, MUIM_GetConfigItem, MUICFG_NListview_HSB, (LONG) (&scrollbar)))
        data->HorizSB = *scrollbar;
      else
        data->HorizSB = DEFAULT_HSB;
    }
    else
      data->HorizSB = data->Horiz_ScrollBar;

    switch (data->HorizSB)
    {
      case MUIV_NListview_HSB_Always :
        scrollers |= MUIV_NListview_HSB_On;
      case MUIV_NListview_HSB_Auto :
      case MUIV_NListview_HSB_FullAuto :
        if (!data->PR_Horiz)
        { HorizScrollBar;
          if (data->PR_Horiz)
          { data->Horiz_Attached = FALSE;
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Horiz_Entries,MUIV_EveryTime,
              data->PR_Horiz, 3, MUIM_NoNotifySet,MUIA_Prop_Entries,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Horiz_Visible,MUIV_EveryTime,
              data->PR_Horiz, 3, MUIM_NoNotifySet,MUIA_Prop_Visible,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_Horiz_First,MUIV_EveryTime,
              data->PR_Horiz, 3, MUIM_NoNotifySet,MUIA_Prop_First,MUIV_TriggerValue);
            DoMethod(data->PR_Horiz, MUIM_Notify, MUIA_Prop_First,MUIV_EveryTime,
              data->LI_NList, 3, MUIM_NoNotifySet,MUIA_NList_Horiz_First,MUIV_TriggerValue);
            DoMethod(data->LI_NList, MUIM_Notify, MUIA_NList_HorizDeltaFactor,MUIV_EveryTime,
              data->PR_Horiz, 3, MUIM_NoNotifySet,MUIA_Prop_DeltaFactor,MUIV_TriggerValue);
          }
        }
        break;
      case MUIV_NListview_HSB_None :
        scrollers |= MUIV_NListview_HSB_Off;
        break;
    }
  }

  scrollers |= ((vert | horiz) & (MUIV_NListview_VSB_On | MUIV_NListview_HSB_On));

  if ((!data->PR_Vert) ||
      (((data->VertSB == MUIV_NListview_VSB_Auto) || (data->VertSB == MUIV_NListview_VSB_Always))&&
       ((scrollers & MUIV_NListview_VSB_On) == MUIV_NListview_VSB_Off)) ||
      ((data->VertSB == MUIV_NListview_VSB_None) &&
       ((scrollers & MUIV_NListview_VSB_On) == MUIV_NListview_VSB_On)) ||
      ((data->Vert_Attached)  && ((scrollers & MUIV_NListview_VSB_On) == MUIV_NListview_VSB_On)) ||
      ((!data->Vert_Attached) && ((scrollers & MUIV_NListview_VSB_On) == MUIV_NListview_VSB_Off)))
  { scrollers &= ~MUIV_NListview_VSB_On;
  }

  if ((!data->PR_Horiz) ||
      (((data->HorizSB == MUIV_NListview_HSB_Auto) || (data->HorizSB == MUIV_NListview_HSB_Always)) &&
       ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_Off)) ||
      ((data->HorizSB == MUIV_NListview_HSB_None) &&
       ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_On)) ||
      ((data->Horiz_Attached)  && ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_On)) ||
      ((!data->Horiz_Attached) && ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_Off)))
  { scrollers &= ~MUIV_NListview_HSB_On;
  }

  if (scrollers & MUIV_NListview_VSB_On)
  { if (!data->SETUP || DoMethod(obj,MUIM_Group_InitChange))
    { if ((scrollers & MUIV_NListview_VSB_On) == MUIV_NListview_VSB_On)
      { data->Vert_Attached = TRUE;
        DoMethod(obj,OM_ADDMEMBER,data->PR_Vert);

        if (data->VertSB == MUIV_NListview_VSB_Left)
          DoMethod(obj,MUIM_Group_Sort,data->PR_Vert ,data->Group, NULL);
      }
      else
      { data->Vert_Attached = FALSE;
        DoMethod(obj,OM_REMMEMBER,data->PR_Vert);
      }
      if (scrollers & MUIV_NListview_HSB_On)
      { if (!data->SETUP || DoMethod(data->Group,MUIM_Group_InitChange))
        { if ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_On)
          { data->Horiz_Attached = TRUE;
            DoMethod(data->Group,OM_ADDMEMBER,data->PR_Horiz);
          }
          else
          { data->Horiz_Attached = FALSE;
            DoMethod(data->Group,OM_REMMEMBER,data->PR_Horiz);
          }
          if (data->SETUP)
            DoMethod(data->Group,MUIM_Group_ExitChange);
        }
      }
      if (data->SETUP)
        DoMethod(obj,MUIM_Group_ExitChange);
    }
  }
  else if (scrollers & MUIV_NListview_HSB_On)
  { if (!data->SETUP || DoMethod(data->Group,MUIM_Group_InitChange))
    {
      if ((scrollers & MUIV_NListview_HSB_On) == MUIV_NListview_HSB_On)
      { data->Horiz_Attached = TRUE;
        DoMethod(data->Group,OM_ADDMEMBER,data->PR_Horiz);
      }
      else
      { data->Horiz_Attached = FALSE;
        DoMethod(data->Group,OM_REMMEMBER,data->PR_Horiz);
      }
      if (data->SETUP)
        DoMethod(data->Group,MUIM_Group_ExitChange);
    }
  }
}

#ifdef __AROS__
#if defined(__PPC__)
IPTR DoSuperNew(Class *cl, Object *obj, Tag tag1, ...) __stackparm;
#endif
IPTR DoSuperNew(Class *cl, Object *obj, Tag tag1, ...)
{
  AROS_SLOWSTACKMETHODS_PRE(tag1)
  
  retval = DoSuperNewTagList(cl, obj, NULL,  (struct TagItem *) AROS_SLOWSTACKMETHODS_ARG(tag1));

  AROS_SLOWSTACKMETHODS_POST
}
#elif !defined(__MORPHOS__)
Object * STDARGS VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  va_list args;

  #if defined(__amigaos4__)
  va_startlinear(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, va_getlinearva(args, ULONG), NULL);
  #else
  va_start(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, args, NULL);
  #endif

  va_end(args);

  return rc;
}
#endif

/* static ULONG mNLV_New(struct IClass *cl,Object *obj,Msg msg) */
static ULONG mNLV_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NLVData *data;
  struct TagItem *tag;
  BOOL  cyclechain = 0;
  Object *nlist = NULL;
  Object *vgroup = NULL;
  LONG Dropable = FALSE;

  if((tag = FindTagItem(MUIA_Draggable, msg->ops_AttrList)))
    tag->ti_Tag = TAG_IGNORE;

  if((tag = FindTagItem(MUIA_Dropable, msg->ops_AttrList)))
  {
    Dropable = tag->ti_Data;
    tag->ti_Tag = TAG_IGNORE;
  }

  if((tag = FindTagItem(MUIA_CycleChain, msg->ops_AttrList)))
  {
    tag->ti_Tag = TAG_IGNORE;
    cyclechain = tag->ti_Data;
  }

  if((tag = FindTagItem(MUIA_NListview_NList, msg->ops_AttrList)) ||
     (tag = FindTagItem(MUIA_Listview_List, msg->ops_AttrList)))
  {
    nlist = (Object *) tag->ti_Data;

    if(nlist)
    {
      if(Dropable)
      {
        nnset(nlist,MUIA_Dropable,Dropable);
      }

      if((tag = FindTagItem(MUIA_NList_DragType, msg->ops_AttrList)) ||
         (tag = FindTagItem(MUIA_Listview_DragType, msg->ops_AttrList)))
      {
        nnset(nlist,tag->ti_Tag,tag->ti_Data);
      }

      if((tag = FindTagItem(MUIA_Listview_Input, msg->ops_AttrList)) ||
         (tag = FindTagItem(MUIA_NList_Input, msg->ops_AttrList)))
      {
        nnset(nlist,tag->ti_Tag,tag->ti_Data);
      }

      if((tag = FindTagItem(MUIA_Listview_MultiSelect, msg->ops_AttrList)) ||
         (tag = FindTagItem(MUIA_NList_MultiSelect, msg->ops_AttrList)))
      {
        nnset(nlist,tag->ti_Tag,tag->ti_Data);
      }

      if((tag = FindTagItem(MUIA_Listview_DoubleClick, msg->ops_AttrList)) ||
         (tag = FindTagItem(MUIA_NList_DoubleClick, msg->ops_AttrList)))
      {
        nnset(nlist,tag->ti_Tag,tag->ti_Data);
      }

      if((tag = FindTagItem(MUIA_Listview_DefClickColumn, msg->ops_AttrList)) ||
         (tag = FindTagItem(MUIA_NList_DefClickColumn, msg->ops_AttrList)))
      {
        nnset(nlist,tag->ti_Tag,tag->ti_Data);
      }
    }
    else
    {
      obj = NULL;
      return((ULONG) obj);
    }
  }
  else
  {
    nlist = MUI_NewObject(MUIC_NList, MUIA_Dropable, Dropable, TAG_MORE, msg->ops_AttrList);
  }

  obj = (Object *) DoSuperNew(cl,obj,
    MUIA_Group_Horiz, TRUE,
    MUIA_Group_Spacing, 0,
    MUIA_CycleChain, cyclechain,
    NoFrame,
    Child, vgroup = (Object *) MUI_NewObject(MUIC_Group,
      MUIA_Group_Spacing, 0,
      Child, nlist,
    TAG_DONE),
    TAG_MORE, msg->ops_AttrList
  );

  if (obj)
  {
    data = INST_DATA(cl,obj);
    data->sem = FALSE;
    data->SETUP = FALSE;
    data->LI_NList = nlist;
    data->PR_Vert = NULL;
    data->PR_Horiz = NULL;
    data->Group = vgroup;
    data->Vert_Attached = FALSE;
    data->Horiz_Attached = FALSE;
    data->Vert_ScrollBar = MUIV_NListview_VSB_Default;
    data->Horiz_ScrollBar = MUIV_NListview_HSB_Default;
    if((tag = FindTagItem(MUIA_Listview_ScrollerPos, msg->ops_AttrList)))
    {
      if (tag->ti_Data == MUIV_Listview_ScrollerPos_None)
      {
        data->Vert_ScrollBar = MUIV_NListview_VSB_None;
        data->Horiz_ScrollBar = MUIV_NListview_HSB_None;
      }
      else if (tag->ti_Data == MUIV_Listview_ScrollerPos_Left)
      {
        data->Vert_ScrollBar = MUIV_NListview_VSB_Left;
      }
      else if (tag->ti_Data == MUIV_Listview_ScrollerPos_Right)
      {
        data->Vert_ScrollBar = MUIV_NListview_VSB_Always;
      }
    }

    if((tag = FindTagItem(MUIA_NListview_Vert_ScrollBar, msg->ops_AttrList)))
      data->Vert_ScrollBar = tag->ti_Data;

    if((tag = FindTagItem(MUIA_NListview_Horiz_ScrollBar, msg->ops_AttrList)))
      data->Horiz_ScrollBar = tag->ti_Data;

    if((tag = FindTagItem(MUIA_ControlChar, msg->ops_AttrList)))
      data->ControlChar = tag->ti_Data;

    data->VertSB = data->Vert_ScrollBar;
    data->HorizSB = data->Horiz_ScrollBar;

    DoMethod(data->LI_NList, MUIM_Notify, MUIA_NListview_Horiz_ScrollBar,MUIV_EveryTime,
      obj, 3, MUIM_Set,MUIA_NListview_Horiz_ScrollBar,MUIV_TriggerValue);

    set(data->LI_NList,MUIA_NList_KeepActive,(LONG) obj);
  }

  return((ULONG) obj);
}


static ULONG mNLV_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLVData *data = INST_DATA(cl,obj);

  if (data->PR_Vert && !data->Vert_Attached)
    MUI_DisposeObject(data->PR_Vert);
  if (data->PR_Horiz && !data->Horiz_Attached)
    MUI_DisposeObject(data->PR_Horiz);

  return(DoSuperMethodA(cl,obj,msg));
}


static ULONG mNLV_Setup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg)
{
  register struct NLVData *data = INST_DATA(cl,obj);
  data->SETUP = FALSE;

  if (!data->sem)
  { data->sem = TRUE;
    NLV_Scrollers(obj,data,data->Vert_ScrollBar,data->Horiz_ScrollBar);
    data->sem = FALSE;
  }

  // Add the event handler for RAWKEY now
	data->eh.ehn_Class  = cl;
	data->eh.ehn_Object = obj;
	data->eh.ehn_Events = IDCMP_RAWKEY;
	data->eh.ehn_Flags  = MUI_EHF_GUIMODE;
	data->eh.ehn_Priority = -1;
	if (_win(obj)) DoMethod(_win(obj), MUIM_Window_AddEventHandler, (ULONG)&data->eh);

  if (!(DoSuperMethodA(cl,obj,(Msg) msg)))
    return(FALSE);

  data->SETUP = TRUE;
  return(TRUE);
}


static ULONG mNLV_Cleanup(struct IClass *cl,Object *obj,struct MUIP_Cleanup *msg)
{
  register struct NLVData *data = INST_DATA(cl,obj);
  data->SETUP = FALSE;

	if (_win(obj)) DoMethod(_win(obj), MUIM_Window_RemEventHandler, (ULONG)&data->eh);

  return (DoSuperMethodA(cl,obj,(Msg) msg));
}


/*
 * static ULONG mNLV_4d50(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
 * {
 *   register struct NLVData *data = INST_DATA(cl,obj);
 *   if (!data->sem)
 *   { data->sem = TRUE;
 *     NLV_Scrollers(obj,data,data->Vert_ScrollBar,data->Horiz_ScrollBar);
 *     data->sem = FALSE;
 *   }
 *   return (DoSuperMethodA(cl,obj,(Msg) msg));
 * }
 */


static ULONG mNLV_Notify(struct IClass *cl,Object *obj,struct MUIP_Notify *msg)
{
  register struct NLVData *data = INST_DATA(cl,obj);
  switch (msg->TrigAttr)
  {
    case MUIA_List_Prop_Entries :
    case MUIA_List_Prop_Visible :
    case MUIA_List_Prop_First :
    case MUIA_NList_Horiz_Entries :
    case MUIA_NList_Horiz_Visible :
    case MUIA_NList_Horiz_First :
    case MUIA_NList_HorizDeltaFactor :
    case MUIA_NList_Prop_Entries :
    case MUIA_NList_Prop_Visible :
    case MUIA_NList_Prop_First :
    case MUIA_NList_VertDeltaFactor :
    case MUIA_NList_SelectChange :
    case MUIA_NList_MultiClick :
    case MUIA_NList_DoubleClick :
    case MUIA_NList_EntryClick :
    case MUIA_NList_Active :
    case MUIA_NList_First :
    case MUIA_NList_Entries :
    case MUIA_NList_TitleClick :
    case MUIA_List_Active :
    case MUIA_Listview_SelectChange :
    case MUIA_Listview_DoubleClick :
      return (DoMethodA(data->LI_NList,(Msg) msg));
  }
  return (DoSuperMethodA(cl,obj,(Msg) msg));
}



static ULONG mNLV_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NLVData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for(tags=((struct opSet *)msg)->ops_AttrList;(tag=(struct TagItem *) NextTagItem(&tags));)
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Listview_ScrollerPos :
        if (tag->ti_Data == MUIV_Listview_ScrollerPos_None)
          tag->ti_Data = MUIV_NListview_VSB_None;
        else if (tag->ti_Data == MUIV_Listview_ScrollerPos_Left)
          tag->ti_Data = MUIV_NListview_VSB_Left;
        else if (tag->ti_Data == MUIV_Listview_ScrollerPos_Right)
          tag->ti_Data = MUIV_NListview_VSB_Always;
        else
          tag->ti_Data = MUIV_NListview_VSB_Default;
      case MUIA_NListview_Vert_ScrollBar :
        tag->ti_Tag = TAG_IGNORE;
        if (!data->sem)
        { data->sem = TRUE;
          NLV_Scrollers(obj,data,tag->ti_Data,0);
          data->sem = FALSE;
        }
        break;
      case MUIA_NListview_Horiz_ScrollBar :
        tag->ti_Tag = TAG_IGNORE;
        if (!data->sem)
        { data->sem = TRUE;
          NLV_Scrollers(obj,data,0,tag->ti_Data);
          data->sem = FALSE;
        }
        break;
    }
  }
  return (DoSuperMethodA(cl,obj,msg));
}


static ULONG mNLV_Get(struct IClass *cl,Object *obj,Msg msg)
{
  struct NLVData *data = INST_DATA(cl,obj);
  LONG  *store = ((struct opGet *)msg)->opg_Storage;

  switch (((struct opGet *)msg)->opg_AttrID)
  {
    case MUIA_Listview_List:
    case MUIA_NListview_NList:
      *store = (LONG) data->LI_NList;
      return (TRUE);
    case MUIA_NListview_Vert_ScrollBar:
      *store = (LONG) data->VertSB;
      return (TRUE);
    case MUIA_NListview_Horiz_ScrollBar:
      *store = (LONG) data->HorizSB;
      return (TRUE);
    case MUIA_NListview_VSB_Width:
      if (data->PR_Vert && data->Vert_Attached)
        *store = (LONG) _width(data->PR_Vert);
      else
        *store = (LONG) 0;
      return (TRUE);
    case MUIA_NListview_HSB_Height:
      if (data->PR_Horiz && data->Horiz_Attached)
        *store = (LONG) _height(data->PR_Horiz);
      else
        *store = (LONG) 0;
      return (TRUE);
    case MUIA_Version:
      *store = LIB_VERSION;
      return(TRUE);
      break;
    case MUIA_Revision:
      *store = LIB_REVISION;
      return(TRUE);
      break;
  }
  return (DoSuperMethodA(cl,obj,msg));
}

DISPATCHERPROTO(_Dispatcher)
{
  DISPATCHER_INIT
  
  switch (msg->MethodID)
  {
    case OM_NEW                  : return (                mNLV_New(cl,obj,(APTR)msg));
    case OM_DISPOSE              : return (            mNLV_Dispose(cl,obj,(APTR)msg));
    case MUIM_Setup              : return (              mNLV_Setup(cl,obj,(APTR)msg));
    case MUIM_Cleanup            : return (            mNLV_Cleanup(cl,obj,(APTR)msg));
/*    case 0x80424d50              : return (               mNLV_4d50(cl,obj,(APTR)msg));*/
/*    case 0x8042845b              : return (               mNLV_845b(cl,obj,(APTR)msg));*/
    case MUIM_HandleInput        : return (        mNLV_HandleInput(cl,obj,(APTR)msg));
    case MUIM_HandleEvent        : return (        mNLV_HandleEvent(cl,obj,(APTR)msg));
    case OM_SET                  : return (                mNLV_Set(cl,obj,(APTR)msg));
    case OM_GET                  : return (                mNLV_Get(cl,obj,(APTR)msg));
    case MUIM_KillNotify         :
    case MUIM_KillNotifyObj      :
    case MUIM_Notify             : return (             mNLV_Notify(cl,obj,(APTR)msg));
    case MUIM_NList_QueryBeginning   : return(0);
    case MUIM_DragQuery              : return(MUIV_DragQuery_Refuse);
    case MUIM_List_Sort              :
    case MUIM_List_Insert            :
    case MUIM_List_InsertSingle      :
    case MUIM_List_GetEntry          :
    case MUIM_List_Clear             :
    case MUIM_List_Jump              :
    case MUIM_List_Select            :
    case MUIM_List_TestPos           :
    case MUIM_List_Redraw            :
    case MUIM_List_Exchange          :
    case MUIM_List_Move              :
    case MUIM_List_NextSelected      :
    case MUIM_List_Remove            :
    case MUIM_List_CreateImage       :
    case MUIM_List_DeleteImage       :
    case MUIM_NList_Sort             :
    case MUIM_NList_Sort2            :
    case MUIM_NList_Insert           :
    case MUIM_NList_InsertSingle     :
    case MUIM_NList_GetEntry         :
    case MUIM_NList_Clear            :
    case MUIM_NList_Jump             :
    case MUIM_NList_Select           :
    case MUIM_NList_TestPos          :
    case MUIM_NList_Redraw           :
    case MUIM_NList_RedrawEntry      :
    case MUIM_NList_Exchange         :
    case MUIM_NList_Move             :
    case MUIM_NList_NextSelected     :
    case MUIM_NList_Remove           :
    case MUIM_NList_CreateImage      :
    case MUIM_NList_DeleteImage      :
    case MUIM_NList_CopyToClip       :
    case MUIM_NList_UseImage         :
    case MUIM_NList_ReplaceSingle    :
    case MUIM_NList_InsertWrap       :
    case MUIM_NList_InsertSingleWrap :
    case MUIM_NList_GetEntryInfo     :
    case MUIM_NList_GetSelectInfo    :
    case MUIM_NList_CopyTo           :
    case MUIM_NList_DropType         :
    case MUIM_NList_DropDraw         :
    case MUIM_NList_DoMethod         :
    case MUIM_NList_ColWidth         :
    case MUIM_NList_ColToColumn      :
    case MUIM_NList_ColumnToCol      :
      { struct NLVData *data = INST_DATA(cl,obj);
        return (DoMethodA(data->LI_NList,msg));
      }
  }
  return(DoSuperMethodA(cl,obj,msg));
  
  DISPATCHER_EXIT
}

