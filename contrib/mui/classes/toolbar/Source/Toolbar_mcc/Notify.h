#ifndef NOTIFY_H
#define NOTIFY_H

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

ULONG Toolbar_KillNotify(struct IClass *cl,Object *obj,struct MUIP_Toolbar_KillNotify *msg);
ULONG Toolbar_KillNotifyObj(struct IClass *cl,Object *obj,struct MUIP_Toolbar_KillNotifyObj *msg);
ULONG Toolbar_Notify(struct IClass *cl,Object *obj,struct MUIP_Toolbar_Notify *msg);
VOID Toolbar_CheckNotify(struct IClass *cl,Object *obj,struct MUIP_Toolbar_CheckNotify *msg);

struct ToolbarNotify
{
  struct  MinNode TNNode; // MUST be the first variable!!!!
  UBYTE   Type;
  UBYTE   TrigButton;
  UBYTE   TrigAttribute;
  ULONG   TrigValue;

  Object  *DestObject;
  ULONG   Number_Of_Args;
  Msg     DestMessage; // Msg is defined in intuition/classusr.h
  Msg     DestMessageCopy;
};

#endif
