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

// Libraries
#include <libraries/mui.h>

// Protos
#include <clib/alib_protos.h>
#include <proto/keymap.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/exec.h>

// Toolbar inclusions
#include "Toolbar_mcc.h"
#include "Notify.h"
#include "InstanceData.h"
#include "Dispatcher.h"
#include "Draw.h"

//static BOOL SUSPEND = FALSE;

/* TODO:
 Total omkodning af HandleEvent, så den bliver mere gennemskuelig.
 Mere fleksibel, så det er muligt at lave ting a la toolbars i Win95
*/

VOID Exclude(struct Toolbar_Data *data, ULONG exclude)
{
  struct MUIP_Toolbar_Description *desc = data->Desc;
  struct MinNode *node;

  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    struct TB_Element *elem = (struct TB_Element *)node;
    if(elem->Type == TDT_BUTTON)
    {
      if(exclude & (1L<<elem->Number))
      {
        if(desc[elem->Number].Flags & TDF_SELECTED)
        {
          desc[elem->Number].Flags -= TDF_SELECTED;
          elem->Redraw = TRUE;
          data->RedrawFlag = TRUE; // Unnecessary in some cases...
        }
      }
    }
  }
}

UBYTE ConvertKey (struct IntuiMessage *imsg)
{
  UBYTE  ascii = 0;
  struct InputEvent event;

  event.ie_NextEvent      = NULL;
  event.ie_Class          = IECLASS_RAWKEY;
  event.ie_SubClass       = 0;
  event.ie_Code           = imsg->Code;
  event.ie_Qualifier      = 0; // imsg->Qualifier - I don't need this.
  event.ie_EventAddress   = (APTR *) *((ULONG *)imsg->IAddress);
  MapRawKey(&event, &ascii, 1, NULL);

  return(ascii);
}

struct TB_Element *GetTool(struct Toolbar_Data *data, UWORD offset)
{
  struct TB_Element *elem = NULL;
  struct TB_Element *temp = NULL;
  struct MinNode *node;
  
  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
  {
    temp = (struct TB_Element *)node;
    if(temp->DstOffset> offset)
      break;
    elem = temp;
  }

  if(!elem || !temp) return NULL;

  // What about toolspaces...
  if(elem->Type == TDT_BUTTON && temp->Type == TDT_BUTTON)
  {
    WORD dst_offset = elem->DstOffset + data->ButtonSize;
    if(offset > dst_offset)
      elem = NULL;
  }

  return elem;
}

#define SWITCH(flag)  flag = (flag == UPDATE_UP ? UPDATE_DOWN : UPDATE_UP)

VOID Pressed(Object *obj, struct Toolbar_Data *data, struct TB_Element *tool, UWORD qualifier)
{
  ULONG number = tool->Number;
  struct MUIP_Toolbar_Description *desc = &data->Desc[number];

  if(!(desc->Flags & (TDF_GHOSTED | TDF_GONE)))
  {
    set(obj, MUIA_Toolbar_Qualifier, qualifier);
    if(desc->Flags & TDF_TOGGLE)
    {
      data->Update = tool;
      if(desc->Flags & TDF_SELECTED)
      {
        if(!(desc->Flags & TDF_RADIO))
        {
          data->UpdateFlag = UPDATE_UP;
          desc->Flags -= TDF_SELECTED;
          MUI_Redraw(obj, MADF_DRAWUPDATE);
          data->UpdateFlag = 0;
          DoMethod(obj, MUIM_Toolbar_CheckNotify, number, MUIV_Toolbar_Notify_Pressed, FALSE);
        }
      }
      else
      {
        data->UpdateFlag = UPDATE_DOWN;
        desc->Flags |= TDF_SELECTED;
        if(desc->MutualExclude)
        {
          Exclude(data, desc->MutualExclude);
        }
        MUI_Redraw(obj, MADF_DRAWUPDATE);
        data->UpdateFlag = 0;
        DoMethod(obj, MUIM_Toolbar_CheckNotify, number, MUIV_Toolbar_Notify_Pressed, TRUE);
//not implemented - Check_Notify(obj, data->notifylist, number, MUIV_Toolbar_Notify_Active, number);
      }
    }
    else
    {
      data->LastUpdate = tool; // Extra update for mouse-movements. Neccesary???
      data->Update = tool;
      data->UpdateFlag = UPDATE_DOWN;
      MUI_Redraw(obj, MADF_DRAWUPDATE);
      DoMethod(obj, MUIM_Toolbar_CheckNotify, number, MUIV_Toolbar_Notify_Pressed, TRUE);
    }
  }
}

BOOL Released (Object *obj, struct Toolbar_Data *data, UWORD qualifier)
{
  BOOL result = FALSE;

  if(data->UpdateFlag)
  {
    // Since toggle-buttons don't set the UpdateFlag then I know it is a select-button.
    // I use 0xff to remove buttons and other stuff from the qualifiers.
    // See devices/inputevent.h
    // The attribute MUIA_Toolbar_Qualifier will still have the entire qualifier.
    // please note that if the user used LALT+LCOMMAND to click at the button via the keyboard
    // we have to make sure that this is recognized here aswell
    if((data->KeyQualifier & 0xff) == (qualifier & 0xff) ||
       (data->KeyQualifier & (IEQUALIFIER_LALT|IEQUALIFIER_LCOMMAND)) == (IEQUALIFIER_LALT|IEQUALIFIER_LCOMMAND))
    {
      struct TB_Element *tool = data->Update;
      data->UpdateFlag = UPDATE_UP;
      MUI_Redraw(obj, MADF_DRAWUPDATE);
      data->UpdateFlag = 0;
      DoMethod(obj, MUIM_Toolbar_CheckNotify, tool->Number, MUIV_Toolbar_Notify_Pressed, FALSE);
      result = TRUE;
    }
    else
    {
      data->UpdateFlag = UPDATE_UP;
      MUI_Redraw(obj, MADF_DRAWUPDATE);
      data->UpdateFlag = 0;
    }
  }
  return result;
}

ULONG Toolbar_HandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
  LONG result = 0;

/*  if(SUSPEND)
    return(NULL);*/

  // Some very practical defines...
  #define _between(a,x,b) ((x)>=(a) && (x)<=(b))
  #define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl,obj);

  if(xget(obj, MUIA_Disabled)) // Should keep a flag in the instance data instead...
    return(0); // WRONG - must DoMethod...

  if (msg->imsg && data->Active)
  {
    switch (msg->imsg->Class)
    {
      case IDCMP_RAWKEY:
      {
        UWORD qualifier = msg->imsg->Qualifier;

        if(!(qualifier & IEQUALIFIER_REPEAT))
        {
          ULONG activeobject = TRUE;
          if(qualifier & IEQUALIFIER_RCOMMAND)
            get(_win(obj), MUIA_Window_ActiveObject, &activeobject);

          if(activeobject) // If there is an active object we should accept ramiga.
          {
            UWORD code = msg->imsg->Code;
            if(code & IECODE_UP_PREFIX) // Key released
            {
//KPrintF("R: OldCode: %lx, Code: %lx, Qualifier: %lx, OldQualifier: %lx\n", data->KeyCode, msg->imsg->Code, qualifier, data->KeyQualifier);

              if(code == (data->KeyCode | IECODE_UP_PREFIX))
              {
                if(Released(obj, data, qualifier))
                  result = MUI_EventHandlerRC_Eat;
              }
            }
            else // Key pressed
            {
              UBYTE key = ConvertKey(msg->imsg);
//KPrintF("P: Key: %lx, OldCode: %lx, Code: %lx, Qualifier: %lx, OldQualifier: %lx\n", key, data->KeyCode, msg->imsg->Code, qualifier, data->KeyQualifier);

              if(key) // Real key
              {
                if(!data->UpdateFlag) // No other real keys pressed
                {
                  struct MinNode *node;

                  for(node = data->ToolList.mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
                  {
                    struct TB_Element *elem = (struct TB_Element *)node;

                    if(elem->Type == TDT_BUTTON)
                    {
                      if(data->Desc[elem->Number].Key == key)
                      {
                        Pressed(obj, data, elem, qualifier);
                        data->KeyCode = code;
                        // Remember to eat the keypress
                        result = MUI_EventHandlerRC_Eat;
                        // Only check one button/key
                        break;
                      }
                    }
                  }
                }
              }
              else // Qualifier pressed
              {
                if(data->UpdateFlag) // Key already pressed
                {
                  data->UpdateFlag = UPDATE_UP;
                  MUI_Redraw(obj, MADF_DRAWUPDATE);
                  data->UpdateFlag = 0;

                  result = MUI_EventHandlerRC_Eat;
                }
              }
            }
          }
        }
      }
      break;

      case IDCMP_MOUSEBUTTONS:
      {
        WORD mouse_x = msg->imsg->MouseX, mouse_y = msg->imsg->MouseY;

        if (_isinobject(mouse_x, mouse_y))
        {
          UWORD offset = data->Horizontal ? mouse_x : mouse_y;
          UWORD qualifier = msg->imsg->Qualifier;
          struct TB_Element *temp = GetTool(data, offset);

          if(msg->imsg->Code == SELECTDOWN) // Pressed
          {
/*            if(data->UpdateFlag)
            {
              KPrintF("Something is wrong....\n");
              SUSPEND = TRUE;
            }
*/
            if(temp && (temp->Type == TDT_BUTTON))
            {
              Pressed(obj, data, temp, qualifier);
            }
          }
          else if(msg->imsg->Code == SELECTUP) // Released
          {
            if(data->UpdateFlag)
            {
              if((struct TB_Element *)data->Update == temp) // Still above the button
              {
                Released(obj, data, qualifier);
              }
              else // Above another button/space
              {
                // We better check that everything is in order...
//                struct MUIP_Toolbar_Description &udesc = data->Desc[data->Update->Number];

                if(/*(!(udesc.Flags & TDF_SELECTED))
                &&*/  (data->UpdateFlag == UPDATE_DOWN))
                {
                  data->UpdateFlag = UPDATE_UP;
                  MUI_Redraw(obj, MADF_DRAWUPDATE);
                }
/*                else if((udesc.Flags & TDF_SELECTED)
                &&  (data->UpdateFlag == UPDATE_UP))
                {
KPrintF("Toolbar.mcc: Correction 2 would have helped.\n");
SUSPEND = TRUE;
//                  data->UpdateFlag = UPDATE_DOWN;
//                  MUI_Redraw(obj, MADF_DRAWUPDATE);
                }*/

              }
              data->UpdateFlag = 0;
            }
          }
        }
        else // Outside object.
        {
          // In theory it should not be neccesary to redraw, because
          // this would already have happened in IDCMP_MOUSEMOVE,
          // but apparently this is not a perfect world :-)
          if (msg->imsg->Code==SELECTUP)
          {
            if(data->UpdateFlag == UPDATE_DOWN)
            {
//              KPrintF("Selectup - saved?: %ld\n", data->UpdateFlag);
//              SUSPEND = TRUE;
              data->UpdateFlag = UPDATE_UP;
              MUI_Redraw(obj, MADF_DRAWUPDATE);
            }

            data->UpdateFlag = 0;
          }
        }
      }
      break;

      case IDCMP_MOUSEMOVE:
      {
        struct TB_Element *temp = NULL;
        WORD mouse_x = msg->imsg->MouseX, mouse_y = msg->imsg->MouseY;

        if (_isinobject(mouse_x, mouse_y))
        {
          UWORD offset = data->Horizontal ? mouse_x : mouse_y;
          temp = GetTool(data, offset);
        }

        if(data->LastUpdate != temp) //
        {
          STRPTR helpstring = NULL;
          BOOL dragable = FALSE;

          if(data->UpdateFlag)
          {
            if( data->LastUpdate == (struct TB_Element *)data->Update
              || temp == (struct TB_Element *)data->Update)
            {
                SWITCH(data->UpdateFlag);
                MUI_Redraw(obj, MADF_DRAWUPDATE);
            }
          }

          if (data->AutoActive)
          {
            if(temp && (temp->Type == TDT_BUTTON))
            {
                ULONG number = temp->Number;
                struct MUIP_Toolbar_Description *desc = &data->Desc[number];

                if ((desc->Flags & (TDF_SELECTED | TDF_GHOSTED |TDF_GONE)) == 0)
                {
                    ULONG OldFlags    = desc->Flags;
                    data->Update      = temp;
                    data->UpdateFlag  = UPDATE_DOWN;
                    desc->Flags       |= TDF_SELECTED;

                    MUI_Redraw(obj, MADF_DRAWUPDATE);
                    desc->Flags       = OldFlags;
                }
            }
          }

          if(temp) // Still inside the area
          {
            if(temp->Type == TDT_SPACE) dragable = TRUE;
            else
            {
              struct TB_Element *tool = (struct TB_Element *)temp;
              struct MUIP_Toolbar_Description *tdesc = &data->Desc[tool->Number];

              if(tdesc->Flags & TDF_GONE) dragable = TRUE;
              else
              {
                dragable = FALSE;
                // The helpstring will be null if neither HelpString or ToolText is present
                if(tdesc->HelpString) helpstring = tdesc->HelpString;
                else                  helpstring = tdesc->ToolText;
              }
            }
          }

          set(obj, MUIA_Toolbar_HelpString, helpstring);
          if(data->Dragable)
            set(obj, MUIA_Draggable, dragable);
        }

        data->LastUpdate = temp;
      }
      break;
    }
  }

  return result ? result : (LONG)DoSuperMethodA(cl, obj, (Msg)msg);
}
