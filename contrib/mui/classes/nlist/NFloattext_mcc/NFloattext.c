/***************************************************************************

 NFloattext.mcc - New Floattext MUI Custom Class
 Registered MUI class, Serial Number: 1d51 (0x9d5100a1 to 0x9d5100aF)

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
#include <exec/io.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include <libraries/mui.h>

#include <proto/muimaster.h>

#include "private.h"
#include "rev.h"

#include "mcc_common.h"

#define imgbt(nr)\
  ImageObject,\
    ImageButtonFrame,\
    MUIA_Background, MUII_ButtonBack,\
    MUIA_InputMode , MUIV_InputMode_RelVerify,\
    MUIA_Image_Spec, (long) nr,\
  End

static char *CopyText(char *textin)
{
  char *textout = NULL;

  if (textin)
  {
    if((textout = AllocVec(strlen(textin)+2,0L)))
    {
      LONG pos = 0;
      while (textin[pos] != '\0')
      {
        textout[pos] = textin[pos];
        pos++;
      }
      textout[pos] = '\0';
    }
  }

  return (textout);
}

#ifdef __AROS__
IPTR DoSuperNew(Class *cl, Object *obj, Tag tag1, ...) __stackparm;
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

static ULONG mNFT_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
  register struct NFTData *data;
  struct TagItem *tag;
  LONG Justify = FALSE;
  LONG Align = ALIGN_LEFT;
  char *Text = NULL;
  LONG tagts = MUIA_NList_TypeSelect;
  LONG tagip = MUIA_NList_Input;
  LONG Copied = FALSE;

  if((tag = FindTagItem(MUIA_NFloattext_SkipChars, msg->ops_AttrList)))
    tag->ti_Tag = MUIA_NList_SkipChars;
  else if((tag = FindTagItem(MUIA_Floattext_SkipChars, msg->ops_AttrList)))
    tag->ti_Tag = MUIA_NList_SkipChars;

  if((tag = FindTagItem(MUIA_NFloattext_TabSize, msg->ops_AttrList)))
    tag->ti_Tag = MUIA_NList_TabSize;
  else if((tag = FindTagItem(MUIA_Floattext_TabSize, msg->ops_AttrList)))
    tag->ti_Tag = MUIA_NList_TabSize;

  if((tag = FindTagItem(MUIA_NFloattext_Text, msg->ops_AttrList)))
  {
    Text = (char *) tag->ti_Data;
    Copied = FALSE;
  }
  else if((tag = FindTagItem(MUIA_Floattext_Text, msg->ops_AttrList)))
  {
    Text = (char *) tag->ti_Data;
    Copied = TRUE;
  }

  if((tag = FindTagItem(MUIA_NList_TypeSelect, msg->ops_AttrList)))
    tagts = TAG_IGNORE;

  if((tag = FindTagItem(MUIA_NList_Input, msg->ops_AttrList)))
  {
    tagip = TAG_IGNORE;
    if (tag->ti_Data)
      tagts = TAG_IGNORE;
  }

  if ((tag = FindTagItem(MUIA_NFloattext_Justify, msg->ops_AttrList)) ||
      (tag = FindTagItem(MUIA_Floattext_Justify, msg->ops_AttrList)))
  {
    if (tag->ti_Data)
    {
      Justify = TRUE;
      Align = ALIGN_JUSTIFY;
    }
    else
    {
      Justify = FALSE;
      Align = ALIGN_LEFT;
    }
  }

  if((tag = FindTagItem(MUIA_NFloattext_Align, msg->ops_AttrList)))
  {
    data = INST_DATA(cl,obj);
    data->NFloattext_Align = tag->ti_Data & ALIGN_MASK;

    if (Align == ALIGN_JUSTIFY)
      Justify = TRUE;
    else
      Justify = FALSE;
  }

  obj = (Object *) DoSuperNew(cl,obj,
    tagts, MUIV_NList_TypeSelect_Char,
    tagip, FALSE,
    TAG_MORE, msg->ops_AttrList
  );

  if (obj)
  {
    data = INST_DATA(cl,obj);
    data->NFloattext_Justify = Justify;
    data->NFloattext_Align = Align;
    data->NFloattext_entry = NULL;
    data->NFloattext_entry_len = 0;
    data->NFloattext_Copied = FALSE;

    if (Copied && Text)
    {
      data->NFloattext_Text = CopyText(Text);
      data->NFloattext_Copied = TRUE;
    }
    else
      data->NFloattext_Text = Text;

    if (data->NFloattext_Text)
      DoMethod(obj,MUIM_NList_InsertWrap,data->NFloattext_Text,-2,MUIV_NList_Insert_Bottom,1,data->NFloattext_Align);
  }

  return((ULONG) obj);
}


static ULONG mNFT_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NFTData *data;
  data = INST_DATA(cl,obj);
  DoMethod(obj,MUIM_NList_Clear,NULL);
  if (data->NFloattext_Copied && data->NFloattext_Text)
    FreeVec(data->NFloattext_Text);
  data->NFloattext_Text = NULL;
  if (data->NFloattext_entry)
    FreeVec(data->NFloattext_entry);
  data->NFloattext_entry = NULL;

  return(DoSuperMethodA(cl,obj,msg));
}


static ULONG mNFT_Set(struct IClass *cl,Object *obj,Msg msg)
{
  register struct NFTData *data = INST_DATA(cl,obj);
  struct TagItem *tags,*tag;

  for (tags=((struct opSet *)msg)->ops_AttrList;(tag=(struct TagItem *) NextTagItem(&tags));)
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Floattext_Text:
      case MUIA_NFloattext_Text:
        DoMethod(obj,MUIM_NList_Clear,NULL);
        if (data->NFloattext_Copied && data->NFloattext_Text)
          FreeVec(data->NFloattext_Text);
        data->NFloattext_Copied = FALSE;
        if (tag->ti_Tag == MUIA_Floattext_Text)
        { data->NFloattext_Text = CopyText((char *) tag->ti_Data);
          data->NFloattext_Copied = TRUE;
        }
        else
          data->NFloattext_Text = (char *) tag->ti_Data;
        if (data->NFloattext_Text)
          DoMethod(obj,MUIM_NList_InsertWrap,data->NFloattext_Text,-2,MUIV_NList_Insert_Bottom,1,data->NFloattext_Align);
        break;
      case MUIA_Floattext_Justify:
      case MUIA_NFloattext_Justify:
        if (tag->ti_Data)
        { data->NFloattext_Justify = TRUE;
          data->NFloattext_Align = ALIGN_JUSTIFY;
        }
        else
        { data->NFloattext_Justify = FALSE;
          data->NFloattext_Align = ALIGN_LEFT;
        }
        break;
      case MUIA_NFloattext_Align:
        data->NFloattext_Align = tag->ti_Data & ALIGN_MASK;
        if (data->NFloattext_Align == ALIGN_JUSTIFY)
          data->NFloattext_Justify = TRUE;
        else
          data->NFloattext_Justify = FALSE;
        break;
      case MUIA_Floattext_SkipChars :
      case MUIA_NFloattext_SkipChars :
        tag->ti_Tag = MUIA_NList_SkipChars;
        break;
      case MUIA_Floattext_TabSize :
      case MUIA_NFloattext_TabSize :
        tag->ti_Tag = MUIA_NList_TabSize;
        break;
    }
  }
  return (DoSuperMethodA(cl,obj,msg));
}


static ULONG mNFT_Get(struct IClass *cl,Object *obj,Msg msg)
{
  struct NFTData *data = INST_DATA(cl,obj);
  LONG  *store = ((struct opGet *)msg)->opg_Storage;

  switch (((struct opGet *)msg)->opg_AttrID)
  {
    case MUIA_Floattext_SkipChars:
    case MUIA_NFloattext_SkipChars:
      ((struct opGet *)msg)->opg_AttrID = MUIA_NList_SkipChars;
      break;
    case MUIA_Floattext_TabSize:
    case MUIA_NFloattext_TabSize:
      ((struct opGet *)msg)->opg_AttrID = MUIA_NList_TabSize;
      break;
    case MUIA_Floattext_Text:
    case MUIA_NFloattext_Text:
      *store = (LONG) data->NFloattext_Text;
      return(TRUE);
    case MUIA_Floattext_Justify:
    case MUIA_NFloattext_Justify:
      *store = data->NFloattext_Justify;
      return(TRUE);
    case MUIA_NFloattext_Align:
      *store = data->NFloattext_Align;
      return(TRUE);
    case MUIA_Version:
      *store = LIB_VERSION;
      return(TRUE);
    case MUIA_Revision:
      *store = LIB_REVISION;
      return(TRUE);
  }
  return (DoSuperMethodA(cl,obj,msg));
}


static ULONG mNFT_GetEntry(struct IClass *cl,Object *obj,struct MUIP_NFloattext_GetEntry *msg)
{
  register struct NFTData *data = INST_DATA(cl,obj);
  struct MUI_NList_GetEntryInfo gei;
  gei.pos = msg->pos;
  gei.line = 0;
  DoMethod(obj, MUIM_NList_GetEntryInfo, &gei);
  if ((gei.entry_pos >= 0) && gei.entry)
  { if (gei.wrapcol)
    { char *entry = gei.entry;
      if (!data->NFloattext_entry || (data->NFloattext_entry_len < gei.charlen))
      { if (data->NFloattext_entry)
          FreeVec(data->NFloattext_entry);
        data->NFloattext_entry_len = gei.charlen+18;
        data->NFloattext_entry = (char *) AllocVec(gei.charlen+20,0L);
      }
      if (data->NFloattext_entry)
      { LONG pos = 0;
        while (pos < gei.charlen)
        { data->NFloattext_entry[pos] = entry[gei.charpos+pos];
          pos++;
        }
        data->NFloattext_entry[pos] = '\0';
        *msg->entry = (APTR) data->NFloattext_entry;
      }
      else
        *msg->entry = (APTR) (&entry[gei.charpos]);
    }
    else
      *msg->entry = (APTR) gei.entry;
  }
  else
    *msg->entry = NULL;
  return (TRUE);
}


DISPATCHERPROTO(_Dispatcher)
{
  DISPATCHER_INIT
  
  switch (msg->MethodID)
  {
    case OM_NEW                   : return (           mNFT_New(cl,obj,(APTR)msg));
    case OM_DISPOSE               : return (       mNFT_Dispose(cl,obj,(APTR)msg));
    case OM_SET                   : return (           mNFT_Set(cl,obj,(APTR)msg));
    case OM_GET                   : return (           mNFT_Get(cl,obj,(APTR)msg));
    case MUIM_List_GetEntry :
    case MUIM_NFloattext_GetEntry : return (      mNFT_GetEntry(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
  
  DISPATCHER_EXIT
}

