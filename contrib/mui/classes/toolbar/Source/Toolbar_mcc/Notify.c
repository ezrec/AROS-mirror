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

// ANSI
#include <string.h>

// Libraries
#include <libraries/mui.h>

// Protos
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>

// Toolbar inclusions
#include "Toolbar_mcc.h"
#include "Notify.h"
#include "InstanceData.h"
#include "Dispatcher.h"

#define Type_EveryTime 1

ULONG Toolbar_KillNotify(struct IClass *cl, Object *obj, struct MUIP_Toolbar_KillNotify *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct MinNode *node;

  for(node = data->NotifyList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct ToolbarNotify *notify = (struct ToolbarNotify *)node;

    if( (msg->TrigAttr   == notify->TrigAttribute)
      && (msg->TrigButton == notify->TrigButton))
    {
      Remove((struct Node *)notify);
      FreeVec(notify);
      return TRUE; // Only remove one notify.
    }
  }

  return FALSE;
}

ULONG Toolbar_KillNotifyObj(struct IClass *cl,Object *obj, UNUSED struct MUIP_Toolbar_KillNotifyObj *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);

  while(!IsListEmpty((struct List *)&data->NotifyList))
  {
    struct ToolbarNotify *notify = (struct ToolbarNotify *)RemHead((struct List *)&data->NotifyList);

    if(notify)
      FreeVec(notify);
  }

  return TRUE;
}

ULONG Toolbar_Notify(struct IClass *cl, Object *obj, struct MUIP_Toolbar_Notify *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);

  struct ToolbarNotify *new_notify = (struct ToolbarNotify *)AllocVec(sizeof(struct ToolbarNotify), MEMF_CLEAR);
  if(!new_notify) return FALSE;

  new_notify->TrigButton      = msg->TrigButton;
  new_notify->TrigAttribute   = msg->TrigAttr;
  new_notify->TrigValue       = msg->TrigValue;
  new_notify->Number_Of_Args  = msg->FollowParams;
  new_notify->Type            = (msg->TrigValue == MUIV_EveryTime) ? Type_EveryTime : 0;

  // lets copy the arguments of the message into our two arrays
  if((new_notify->DestMessage = (Msg)AllocVec(sizeof(ULONG)*new_notify->Number_Of_Args, 0L)))
  {
    ULONG *para = (ULONG *)(((LONG)&msg->FollowParams)+4);

    memcpy(new_notify->DestMessage, para, new_notify->Number_Of_Args*sizeof(ULONG));

    if((new_notify->DestMessageCopy = (Msg)AllocVec(sizeof(ULONG)*new_notify->Number_Of_Args, 0L)))
    {
      memcpy(new_notify->DestMessageCopy, para, new_notify->Number_Of_Args*sizeof(ULONG));
      new_notify->DestObject = msg->DestObj; // Only reaches this point if allocations are succesful.
    }
  }

  if(!new_notify->DestObject)
  {
    if(new_notify->DestMessageCopy) FreeVec(new_notify->DestMessageCopy);
    if(new_notify->DestMessage)     FreeVec(new_notify->DestMessage);
    FreeVec(new_notify);

    return FALSE;
  }

  AddTail((struct List *)&data->NotifyList, (struct Node *)new_notify);

  if((msg->TrigAttr >= MUIV_Toolbar_Notify_LeftEdge) &&
     (msg->TrigAttr <= MUIV_Toolbar_Notify_BottomEdge) )
    data->PositionNotify = TRUE;

  return TRUE;
}

VOID Toolbar_CheckNotify(struct IClass *cl, Object *obj, struct MUIP_Toolbar_CheckNotify *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);
  struct MinNode *node;

  for(node = data->NotifyList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct ToolbarNotify *notify = (struct ToolbarNotify *)node;

    if((notify->TrigAttribute == msg->TrigAttr) &&
       (notify->TrigButton == msg->TrigButton)  &&
       ((notify->TrigValue == msg->TrigValue) || (notify->TrigValue == MUIV_EveryTime)))
    {
      Object *dest;
      ULONG *dstmsg = (ULONG *)notify->DestMessage;
      ULONG *dstmsg_orig = (ULONG *)notify->DestMessageCopy;
      ULONG i;

      // we have to find special Values that we need to replace
      // with the real values.
      for(i=0; i < notify->Number_Of_Args; i++)
      {
        switch(dstmsg_orig[i])
        {
          case MUIV_TriggerValue:
          {
            dstmsg[i] = msg->TrigValue;
          }
          break;

          case MUIV_NotTriggerValue:
          {
            dstmsg[i] = !msg->TrigValue;
          }
          break;

          case MUIV_Toolbar_Qualifier:
          {
            dstmsg[i] = xget(obj, MUIA_Toolbar_Qualifier);
          }
          break;
        }
      }

      switch((ULONG)notify->DestObject)
      {
        case MUIV_Notify_Self:
        {
          dest = obj;
        }
        break;
        
        case MUIV_Notify_Window:
        {
          dest = _win(obj);
        }
        break;
        
        case MUIV_Notify_Application:
        {
          dest = _app(obj);
        }
        break;
        
        default:
        {
          dest = notify->DestObject;
        }
      }

      // let us send the method to our destination object now.
      if(dest)
        DoMethodA(dest, notify->DestMessage);
    }
  }
}
