/***************************************************************************

 Toolbar MCC - MUI Custom Class for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

// Protos
#include <clib/alib_protos.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/intuition.h>

// Other
#include <libraries/mui.h>

// Toolbar inclusions
#include "Toolbar_mcc.h"
#include "InstanceData.h"
#include "Draw.h"
#include "HandleEvent.h"
#include "SetGet.h"
#include "rev.h"

struct TB_Element *FindTool(struct Toolbar_Data *data, ULONG number)
{
  struct TB_Element *result = NULL;
  struct MinNode *node;

  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct TB_Element *elem = (struct TB_Element *)node;
    if(elem->Type == TDT_BUTTON)
    {
      if(elem->Number == number)
      {
        result = elem;
        break;
      }
    }
  }

  return(result);
}

UWORD Toolbar_Edge(struct IClass *cl, Object *obj, struct MUIP_Toolbar_Edge *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct TB_Element *tool = FindTool(data, msg->Button);
  UWORD result = 0;

  switch(msg->MethodID)
  {
    case MUIM_Toolbar_LeftEdge:
      result = tool->DstX;
    break;
    case MUIM_Toolbar_RightEdge:
      result = tool->DstX + data->ButtonWidth - 1;
    break;
    case MUIM_Toolbar_TopEdge:
      result = tool->DstY;
    break;
    case MUIM_Toolbar_BottomEdge:
      result = tool->DstY + data->ButtonHeight - 1;
    break;
  }

  return(result);
}

ULONG Toolbar_GetAttrib(struct IClass *cl, Object *obj, struct opGet *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  ULONG result = TRUE;
  ULONG *value = msg->opg_Storage;

  switch (msg->opg_AttrID)
  {
      case MUIA_Version:
        *value = (ULONG)LIB_VERSION;
      break;
      case MUIA_Revision:
        *value = (ULONG)LIB_REVISION;
      break;
      case MUIA_Toolbar_HelpString:
        *value = (ULONG)data->HelpString;
      break;
      case MUIA_Toolbar_Path:
        *value = (ULONG)data->Path;
      break;
      case MUIA_Toolbar_Permutation:
        *value = (ULONG)data->Permutation;
      break;
      case MUIA_Toolbar_Qualifier:
        *value = (ULONG)data->KeyQualifier;
      break;
      default:
        result = DoSuperMethodA(cl, obj, (Msg)msg);
  }

  return result;
}

ULONG Toolbar_SetAttrib(struct IClass *cl, Object *obj, struct opSet *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct TagItem *tags, *tag;

  for(tags=msg->ops_AttrList; (tag=NextTagItem(&tags)) != NULL; )
  {
    switch (tag->ti_Tag)
    {
      case MUIA_Toolbar_HelpString:
        data->HelpString = (STRPTR)tag->ti_Data;
      break;
      case MUIA_Toolbar_Path:
        data->Path = (STRPTR)tag->ti_Data;
        // A small hack...
        DoMethod(obj, MUIM_Toolbar_ReloadImages, data->NormalData, data->SelectData, data->GhostData);
      break;
      case MUIA_Toolbar_Permutation:
      {
        if(data->PermutationCreated)
        {
          data->PermutationCreated = FALSE;
          FreeVec(data->Permutation);
        }

        data->Permutation = (BYTE *)tag->ti_Data;
        // There is that hack again...
        DoMethod(obj, MUIM_Toolbar_ReloadImages, data->NormalData, data->SelectData, data->GhostData);
      }
      break;
      case MUIA_Toolbar_Qualifier:
        data->KeyQualifier = (UWORD)tag->ti_Data;
      break;
/*      case MUIA_Draggable:
        data->dragable = (BOOL)tag->ti_Data;
      break;
*/    }
  }

  return(DoSuperMethodA(cl, obj, (Msg)msg));

}

ULONG Toolbar_MultiSet(UNUSED struct IClass *cl, Object *obj, struct MUIP_Toolbar_MultiSet *msg)
{
  LONG *buttons = &msg->Button;

  while(*buttons != -1)
  {
    DoMethod(obj, MUIM_Toolbar_Set, *buttons, msg->Flag, msg->Value);
    buttons++;
  }
  return(TRUE);
}

ULONG Toolbar_Set(struct IClass *cl,Object *obj,struct MUIP_Toolbar_Set *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct MUIP_Toolbar_Description *desc = &data->Desc[msg->Button];

  if(msg->Value)
  {
    if(!(desc->Flags & msg->Flag))
    {
      desc->Flags |= msg->Flag; // Add flag

      if(data->Active)
      {
        struct TB_Element *tool = FindTool(data, msg->Button);

        if(msg->Flag == MUIV_Toolbar_Set_Gone)
        {
          data->SingleGone = tool;
        }
        else if(msg->Flag == MUIV_Toolbar_Set_Selected)
        {
          if(desc->MutualExclude)
          {
            Exclude(data, desc->MutualExclude);
          }
        }

        data->RedrawFlag = TRUE;
        tool->Redraw = TRUE;
        data->UpdateFlag = 0; // Solving the problem with pressing a button simultaneously with it being ghosted by set
        MUI_Redraw(obj, MADF_DRAWUPDATE);
      }
    }
  }
  else
  {
    if(desc->Flags & msg->Flag)
    {
      desc->Flags &= (-1)-(msg->Flag); //Remove flag

      if(data->Active)
      {
        if(msg->Flag == MUIV_Toolbar_Set_Selected)
        {
          if(desc->MutualExclude)
          {
            Exclude(data, desc->MutualExclude);
          }
        }

        data->RedrawFlag = TRUE;
        FindTool(data, msg->Button)->Redraw = TRUE;
        MUI_Redraw(obj, MADF_DRAWUPDATE);
      }
    }
  }

// Måske burde der resettes i updateflag-sammenhæng...

  return(TRUE);
}
