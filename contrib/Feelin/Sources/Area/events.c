#include "Private.h"

//#define DB_HANDLEEVENT
//#define DB_HANDLEEVENT_CHECK
//#define DB_TRY_ADD_HANDLER

/*** Private ***************************************************************/

///area_try_add_handler
void area_try_add_handler(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Handler &&
       LOD -> Handler -> Events &&
       (!(FF_EventHandler_Active & LOD -> Handler -> Flags)) &&
       (FF_Area_CanDraw & _flags) &&
       (!(FF_Area_Disabled & _flags)))
   {
      F_Do(_render -> Window,FM_Window_AddEventHandler,LOD -> Handler);
   }
#ifdef DB_TRY_ADD_HANDLER
   else
   {
      F_Log(0,"TRY ADD FAILED");
      
      if (!LOD -> Handler)
      {
         F_Log(0,"   >> Handler is NULL");
      }
      else if (!LOD -> Handler -> Events)
      {
         F_Log(0,"   >> Not Events requested");
      }
      else if (FF_EventHandler_Active & LOD -> Handler -> Flags)
      {
         F_Log(0,"   >> The handler is active");
      }
      else if (!(FF_Area_CanDraw & _flags))
      {
         F_Log(0,"   >> Cannot draw");
      }
      else if (FF_Area_Disabled & _flags)
      {
         F_Log(0,"   >> Disabled");
      }
   }
#endif
}
//+
///area_try_rem_handler
void area_try_rem_handler(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Handler && (FF_EventHandler_Active & LOD -> Handler -> Flags))
   {
      if (_render)
      {
         F_Do(_render -> Window,FM_Window_RemEventHandler,LOD -> Handler);
      }
   }
}
//+

/*** Methods ***************************************************************/

///Area_ModifyHandler
F_METHODM(FEventHandler *,Area_ModifyHandler,FS_ModifyHandler)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   bits32 events;
   FObject winobj = _win;

//   F_Log(0,"ADD 0x%08lx - SUB 0x%08lx - HANDLER 0x%08lx",Msg -> Add,Msg -> Sub,handler);
 
   events = LOD -> Handler ? LOD -> Handler -> Events : 0;
   events |= Msg -> Add;
   events &= ~Msg -> Sub;

   if (LOD -> Handler)
   {
      if (LOD -> Handler -> Events == events) return LOD -> Handler;

      if (FF_EventHandler_Active & LOD -> Handler -> Flags)
      {
         F_Do(winobj,FM_Window_RemEventHandler,LOD -> Handler);
      }
   }

   if (events)
   {
      if (!LOD -> Handler)
      {
         LOD -> Handler = F_New(sizeof (FEventHandler));
         LOD -> Handler -> Object = Obj;
      }
   
      if (LOD -> Handler)
      {
         LOD -> Handler -> Events = events;
         
         area_try_add_handler(Class,Obj,FeelinBase);
      }
   }
   else if (LOD -> Handler)
   {
      F_Dispose(LOD -> Handler) ; LOD -> Handler = NULL;
   }
   return LOD -> Handler;
}
//+
///Area_HandleEvent
F_METHODM(bits32,Area_HandleEvent,FS_HandleEvent)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   FEvent *fev = Msg -> Event;
   BOOL selected = (FF_Area_Selected & _flags) ? TRUE : FALSE;
   
   #ifdef DB_HANDLEEVENT_CHECK
 
   if (FF_Area_Disabled & _flags)
   {
      F_Log(0,"Object is disabled but have received FM_HandleEvent !!");

      return 0;
   }
   if (!LOD -> Handler)
   {
      F_Log(0,"no handler, but event received");
   }
   else if (!(FF_EventHandler_Active & LOD -> Handler -> Flags))
   {
      F_Log(0,"handler not active, but event received");
   }

   #endif

   if (fev -> Key)
   {
/// Key
      if (!(FF_Area_Active & _flags)) return 0;

      switch (fev -> Key)
      {
         case FV_KEY_PRESS:
         {
            switch (LOD -> InputMode)
            {
               case FV_InputMode_Release:
               {
                  if (!selected)
                  {
                     F_Do(Obj,FM_Set,
                           FA_Selected, TRUE,
                           FA_Pressed,  TRUE, TAG_DONE);
                  }
               }
               break;

               case FV_InputMode_Immediate:
               {
                  F_SuperDo(Class,Obj,FM_Set,FA_Selected,TRUE,TAG_DONE);
               }
               break;

               case FV_InputMode_Toggle:
               {
                  F_Set(Obj,FA_Selected,(selected) ? FALSE : TRUE);
               }
               break;
            }
         }
         return FF_HandleEvent_Eat;

         case FV_KEY_RELEASE:
         {
            if (LOD -> InputMode == FV_InputMode_Release)
            {
               if (selected)
               {
                  F_Do(Obj,FM_Set,
                           FA_Selected, FALSE,
                           FA_Pressed,  FALSE, TAG_DONE);
               }
            }
         }
         return FF_HandleEvent_Eat;
      }
   }
//+
   else
   {
      switch (fev -> Class)
      {
///event:key
         case FF_EVENT_KEY:
         {
            if (FF_EVENT_KEY_REPEAT & fev -> Flags)
            {
               return 0;
            }
            
            if (LOD -> ControlChar && (ToLower(LOD -> ControlChar) == ToLower(fev -> DecodedChar)))
            { 
               switch (LOD -> InputMode)
               {
                  case FV_InputMode_Release:
                  {
                     if (FF_EVENT_KEY_UP & fev -> Flags)
                     {
                        if (selected)
                        {
                           F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_TICK);

                           F_Do(Obj,FM_Set,
                                    FA_Pressed,    FALSE,
                                    FA_Selected,   FALSE, TAG_DONE);
                        }
                     }
                     else
                     {
                        F_Do(Obj,FM_ModifyHandler,FF_EVENT_TICK,0);

                        F_Do(Obj,FM_Set,
                                 FA_Pressed,    TRUE,
                                 FA_Selected,   TRUE,  TAG_DONE);
                     }
                  }
                  break;

                  case FV_InputMode_Toggle:
                  {
                     if (!(FF_EVENT_KEY_UP & fev -> Flags))
                     {
                        F_Set(Obj,FA_Selected,(selected) ? FALSE : TRUE);
                     }
                  }
                  break;
                  
                  case FV_InputMode_Immediate:
                  {
                     F_SuperDo(Class,Obj,FM_Set,FA_Selected,TRUE,TAG_DONE);
                  }
                  break;
               }
               return FF_HandleEvent_Eat;
            }
            else if (selected && LOD -> InputMode == FV_InputMode_Release)
            {
               F_Do(Obj,FM_Set,
                        FA_Selected,   FALSE,
                        FA_Pressed,    FALSE,
                        FA_NoNotify,   TRUE,  TAG_DONE);
            }
         }
         break;
//+
///event:button
         case FF_EVENT_BUTTON:
         {
            if (fev -> Code == FV_EVENT_BUTTON_SELECT)
            {
               if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
               {
                  
/*** button-select-down ****************************************************/
 
                  if (_inside(fev -> MouseX,_x,_x + _w - 1) &&
                      _inside(fev -> MouseY,_y,_y + _h - 1))
                  {
                     switch (LOD -> InputMode)
                     {
                        case FV_InputMode_Immediate:
                        {
                           F_SuperDo(Class,Obj,FM_Set,FA_Selected,TRUE,TAG_DONE);
                        }
                        break;

                        case FV_InputMode_Toggle:
                        {
                           F_Set(Obj,FA_Selected,(selected) ? FALSE : TRUE);
                        }
                        break;

                        case FV_InputMode_Release:
                        {
                           F_Do(Obj,FM_Set,
                                    FA_Selected, TRUE,
                                    FA_Pressed,  TRUE, TAG_DONE);

                           F_Do(Obj,FM_ModifyHandler,FF_EVENT_TICK | FF_EVENT_MOTION,0);
                        }
                        break;
                     }

                     /* if the object is draggable, we force  the  'motion'
                     event */

                     if (FF_AREA_DRAGGABLE & LOD -> Flags)
                     {
                        LOD -> Flags |= FF_AREA_DRAGGING;

                        F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                     }

                     return FF_HandleEvent_Eat;
                  }
               }
               else
               {

/*** button-select-up ******************************************************/
 
                  if (FF_Area_Pressed & _flags)
                  {
                     
                     /* our object was pressed, the  mouse  button  is  now
                     released. first we change object state */
 
                     if (selected)
                     {
                        F_Set(Obj,FA_Selected,FALSE);
                     }
                  
                     /* now we check if the mouse is still over our object,
                     in which case we set the attribute FA_Pressed to FALSE
                     to indicate that the button was  pressed  and  is  now
                     released. Oterwise we simply clear the FF_Area_Pressed
                     flag. */

                     if (_inside(fev -> MouseX,_x,_x + _w - 1) &&
                         _inside(fev -> MouseY,_y,_y + _h - 1))
                     {
                        F_Set(Obj,FA_Pressed,FALSE);
                     }
                     else
                     {
                        _flags &= ~FF_Area_Pressed;
                     }
                     
                     /* remove  'tick'  and  'motion'  from  the  requested
                     events */
 
                     F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_TICK | FF_EVENT_MOTION);

                     return FF_HandleEvent_Eat;
                  }
               }
            }
            if (!(FF_EVENT_BUTTON_DOWN & fev -> Flags))
            {
               if (FF_AREA_DRAGGABLE & LOD -> Flags)
               {
                  F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION | FF_EVENT_TICK);
               }
            }
 
         }
         break;
//+
///event:motion
         case FF_EVENT_MOTION:
         {
            
            /* if the object is draggable we launch the  DnD  operation  on
            the first mouve of the mouse. Otherwise, we toggle the selected
            state of the object depending on the mouse being in or  out  of
            it. */
 
            if (FF_AREA_DRAGGABLE & LOD -> Flags)
            {
               F_Do(Obj,FM_Set,
                        FA_Selected,   FALSE,
                        FA_Pressed,    FALSE,
                        FA_NoNotify,   TRUE, TAG_DONE);

               F_Do(_app,FM_Application_PushMethod,Obj,FM_DnDDo,2,fev -> MouseX,fev -> MouseY);
            
               F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_TICK | FF_EVENT_MOTION);

               return FF_HandleEvent_Eat;
            }
            else if (FF_Area_Pressed & _flags)
            {
               if (_inside(fev -> MouseX,_x,_x + _w - 1) &&
                   _inside(fev -> MouseY,_y,_y + _h - 1))
               {
                  if (!selected)
                  {
                     F_Set(Obj,FA_Selected,TRUE);
                  }
               }
               else
               {
                  if (selected)
                  {
                     F_Set(Obj,FA_Selected,FALSE);
                  }
               }
            }

         }
         break;
//+
///event:tick
         case FF_EVENT_TICK:
         {
            if (FF_Area_Pressed & _flags)
            {
               F_Set(Obj,FA_Timer,TRUE);
            }
         }
         break;
//+
      }
   }
   return 0;
}
//+
