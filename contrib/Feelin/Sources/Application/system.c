#include "Private.h"

///app_collect_signals
bits32 app_collect_signals(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FSignalHandler *node;

    bits32 wait = LOD -> UserSignals | SIGBREAKF_CTRL_C | (1 << LOD -> AppPort -> mp_SigBit) | (1 << LOD -> WindowPort -> mp_SigBit) | (1 << LOD -> TimersPort -> mp_SigBit);

    if (LOD -> BrokerPort)
    {
        wait |= (1 << LOD -> BrokerPort -> mp_SigBit);
    }

    for (node = (FSignalHandler *)(LOD -> Signals.Head) ; node ; node = node -> Next)
    {
        wait |= node -> fsh_Signals;
    }

    LOD -> WaitSignals = wait;
    
    return wait;
}
//+

/*** Private ***************************************************************/

///app_check_window_port
STATIC int32 app_check_window_port(FClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> WindowPort)
   {
      struct IntuiMessage *msg = (struct IntuiMessage *)(GetMsg(LOD -> WindowPort));

      if (msg)
      {
         FEvent *fev = F_New(sizeof (FEvent));

         if (fev)
         {
            fev -> IMsg = msg;
 
            if ((((struct AppMessage *)(msg)) -> am_Type == AMTYPE_APPWINDOW ||
                 ((struct AppMessage *)(msg)) -> am_Type == AMTYPE_APPICON ||
                 ((struct AppMessage *)(msg)) -> am_Type == AMTYPE_APPMENUITEM) &&
                (((struct AppMessage *)(msg)) -> am_ID == MAKE_ID('A','P','P','W')))
            {
               fev -> Class      = FF_EVENT_WBDROP;
               fev -> Key        = FV_KEY_NONE,
               fev -> MouseX     = ((struct AppMessage *)(msg)) -> am_MouseX;
               fev -> MouseY     = ((struct AppMessage *)(msg)) -> am_MouseY;
               fev -> Seconds    = ((struct AppMessage *)(msg)) -> am_Seconds;
               fev -> Micros     = ((struct AppMessage *)(msg)) -> am_Micros;
               fev -> Window     = (APTR)(((struct AppMessage *)(msg)) -> am_UserData);
            }
            else if (msg -> IDCMPWindow)
            {

/*** intuition events ******************************************************/
 
               fev -> Qualifier  = msg -> Qualifier;
               fev -> Key        = FV_KEY_NONE;
               fev -> MouseX     = msg -> MouseX;
               fev -> MouseY     = msg -> MouseY;
               fev -> Seconds    = msg -> Seconds;
               fev -> Micros     = msg -> Micros;
               fev -> Window     = (APTR) msg -> IDCMPWindow -> UserData;

               switch (msg -> Class)
               {

/*** event-tick ************************************************************/

                  case IDCMP_INTUITICKS:
                  {
                     fev -> Class = FF_EVENT_TICK;
                  }
                  break;

/*** event-key *************************************************************/

                  case IDCMP_RAWKEY:
                  {
                     
                     /* Mouse wheel events are received as keycodes because
                     intuition doesn't know how to decode them */
 
                     if (msg -> Code == 0x7A)
                     {
                        fev -> Class  = FF_EVENT_BUTTON;
                        fev -> Code   = FV_EVENT_BUTTON_WHEEL;
                     }
                     else if (msg -> Code == 0x7B)
                     {
                        fev -> Class  = FF_EVENT_BUTTON;
                        fev -> Code   = FV_EVENT_BUTTON_WHEEL;
                        fev -> Flags |= FF_EVENT_BUTTON_DOWN;
                     }
                     else
                     {
                        struct InputEvent ie;
                        ULONG i;

                        fev -> Class = FF_EVENT_KEY;

                        if (msg -> Code & IECODE_UP_PREFIX)          fev -> Flags |= FF_EVENT_KEY_UP;
                        if (msg -> Qualifier & IEQUALIFIER_REPEAT)   fev -> Flags |= FF_EVENT_KEY_REPEAT;

                        ie.ie_NextEvent      = NULL;
                        ie.ie_Class          = IECLASS_RAWKEY;
                        ie.ie_SubClass       = 0;
                        ie.ie_Code           = msg -> Code;
                        ie.ie_Qualifier      = msg -> Qualifier;
                        ie.ie_EventAddress   = (APTR *) *((ULONG *)(msg -> IAddress));

                        /* search a matching configurable key */

                        for (i = 1 ; i < FV_KEY_COUNT ; i += 1)
                        {
                           if (MatchIX(&ie,&LOD -> Keys[i - 1]))
                           {
                              fev -> Key = i; break;
                           }
                        }

                        /* try to decode the key as a char */

                        if (fev -> Key == FV_KEY_NONE)
                        {
                           UBYTE buf[4];

                           ie.ie_Code = msg -> Code & ~0x80;

                           if (MapRawKey(&ie,buf,3,0) == 1)
                           {
                              fev -> DecodedChar = buf[0];
                           }
                        }
                     }
                  }
                  break;

/*** event-button **********************************************************/

                  case IDCMP_MOUSEBUTTONS:
                  {
                     fev -> Class = FF_EVENT_BUTTON;

                     if (!(IECODE_UP_PREFIX & msg -> Code))
                     {
                        fev -> Flags |= FF_EVENT_BUTTON_DOWN;
                     }

                     switch (msg -> Code & ~IECODE_UP_PREFIX)
                     {
                        case IECODE_LBUTTON: fev -> Code = FV_EVENT_BUTTON_SELECT; break;
                        case IECODE_RBUTTON: fev -> Code = FV_EVENT_BUTTON_MENU; break;
                        case IECODE_MBUTTON: fev -> Code = FV_EVENT_BUTTON_MIDDLE; break;
                        default:             fev -> Code = 0xFFFF;
                     };
                     
                     if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                     {
                        if (LOD -> ev_Window == LOD -> ev_Window &&
                            LOD -> ev_Button == fev -> Code &&
                            LOD -> ev_MouseX == fev -> MouseX &&
                            LOD -> ev_MouseY == fev -> MouseY &&
                            DoubleClick(LOD -> ev_Secs,LOD -> ev_Micros,fev -> Seconds,fev -> Micros))
                        {
                           fev -> Flags |= FF_EVENT_BUTTON_DOUBLE;
                           LOD -> ev_Button = 0xFF;
                        }
                        else
                        {
                           LOD -> ev_Window = fev -> Window;
                           LOD -> ev_Button = fev -> Code;
                           LOD -> ev_MouseX = fev -> MouseX;
                           LOD -> ev_MouseY = fev -> MouseY;
                           LOD -> ev_Secs   = fev -> Seconds;
                           LOD -> ev_Micros = fev -> Micros;
                        }
                     }
                  }
                  break;

/*** event-motion **********************************************************/

                  case IDCMP_MOUSEMOVE:
                  {
                     fev -> Class = FF_EVENT_MOTION;
                  }
                  break;

/*** event-window **********************************************************/

                  case IDCMP_CLOSEWINDOW:
                  {
                     fev -> Class = FF_EVENT_WINDOW;
                     fev -> Code  = FV_EVENT_WINDOW_CLOSE;
                  }
                  break;

                  case IDCMP_ACTIVEWINDOW:
                  {
                     fev -> Class = FF_EVENT_WINDOW;
                     fev -> Code  = FV_EVENT_WINDOW_ACTIVE;
                  }
                  break;

                  case IDCMP_INACTIVEWINDOW:
                  {
                     fev -> Class = FF_EVENT_WINDOW;
                     fev -> Code  = FV_EVENT_WINDOW_INACTIVE;
                  }
                  break;

                  case IDCMP_NEWSIZE:
                  {
                     fev -> Class = FF_EVENT_WINDOW;
                     fev -> Code  = FV_EVENT_WINDOW_SIZE;
                  }
                  break;

                  case IDCMP_REFRESHWINDOW:
                  {
                     fev -> Class = FF_EVENT_WINDOW;
                     fev -> Code  = FV_EVENT_WINDOW_REFRESH;
                  }
                  break;

                  case IDCMP_CHANGEWINDOW:
                  {
                     fev -> Class = FF_EVENT_WINDOW;

                     if (msg -> Code) fev -> Code  = FV_EVENT_WINDOW_DEPTH;
                     else             fev -> Code  = FV_EVENT_WINDOW_CHANGE;
                  }
                  break;

/*** event-disk ************************************************************/

                  case IDCMP_DISKINSERTED:
                  {
                     fev -> Class = FF_EVENT_DISK;
                     fev -> Code  = FV_EVENT_DISK_LOAD;
                  }
                  break;

                  case IDCMP_DISKREMOVED:
                  {
                     fev -> Class = FF_EVENT_DISK;
                     fev -> Code  = FV_EVENT_DISK_EJECT;
                  }
                  break;
               }         
            }

///DB_EVENTS
            if (CUD -> db_Events)
            {
               switch (fev -> Class)
               {
                  case FF_EVENT_TICK:
                  {
                     F_Log(0,"event:tick (%06ld'%ld)",fev -> Seconds,fev -> Micros);
                  }
                  break;

                  case FF_EVENT_KEY:
                  {
                     F_Log(0,"event:key (code 0x%04lx, key %ld, char %lc, %s %s)",fev -> Code,fev -> Key,(fev -> DecodedChar) ? fev -> DecodedChar : '#',(FF_EVENT_KEY_UP & fev -> Flags) ? "up" : "down",(FF_EVENT_KEY_REPEAT & fev -> Flags) ? "repeat" : "");
                  }
                  break;

                  case FF_EVENT_BUTTON:
                  {
                     switch (fev -> Code)
                     {
                        case FV_EVENT_BUTTON_SELECT:  F_Log(0,"event:button-select-%s-%s)",(IECODE_UP_PREFIX & msg -> Code) ? "up" : "down",(FF_EVENT_BUTTON_DOUBLE & fev -> Flags) ? "double" : "first"); break;
                        case FV_EVENT_BUTTON_MENU:    F_Log(0,"event:button-menu-%s-%s)",(IECODE_UP_PREFIX & msg -> Code) ? "up" : "down",(FF_EVENT_BUTTON_DOUBLE & fev -> Flags) ? "double" : "first"); break;
                        case FV_EVENT_BUTTON_MIDDLE:  F_Log(0,"event:button-middle-%s-%s)",(IECODE_UP_PREFIX & msg -> Code) ? "up" : "down",(FF_EVENT_BUTTON_DOUBLE & fev -> Flags) ? "double" : "first"); break;
                        default:                      F_Log(0,"event:button-??-%s-%s)",(IECODE_UP_PREFIX & msg -> Code) ? "up" : "down",(FF_EVENT_BUTTON_DOUBLE & fev -> Flags) ? "double" : "first"); break;
                     }
                  }
                  break;

                  case FF_EVENT_MOTION:
                  {
                     F_Log(0,"event:motion (%04ld:%04ld)",fev -> MouseX,fev -> MouseY);
                  }
                  break;

                  case FF_EVENT_WINDOW:
                  {
                     switch (fev -> Code)
                     {
                        case FV_EVENT_WINDOW_CLOSE:      F_Log(0,"even:window-close)"); break;
                        case FV_EVENT_WINDOW_ACTIVE:     F_Log(0,"even:window-active)"); break;
                        case FV_EVENT_WINDOW_INACTIVE:   F_Log(0,"even:window-inactive)"); break;
                        case FV_EVENT_WINDOW_SIZE:       F_Log(0,"even:window-size)"); break;
                        case FV_EVENT_WINDOW_DEPTH:      F_Log(0,"even:window-depth)"); break;
                        case FV_EVENT_WINDOW_CHANGE:     F_Log(0,"even:window-change)"); break;
                        case FV_EVENT_WINDOW_REFRESH:    F_Log(0,"even:window-refresh)"); break;
                     }
                  }
                  break;

                  case FF_EVENT_DISK:
                  {
                     switch (fev -> Code)
                     {
                        case FV_EVENT_DISK_LOAD:   F_Log(0,"(event:disk-load)"); break;
                        case FV_EVENT_DISK_EJECT:  F_Log(0,"(event:disk-eject)"); break;
                     }
                     break;
                  }
                  break;

                  case FF_EVENT_WBDROP:
                  {
                     F_Log(0,"event:wbdrop (%04ld:%04ld)",fev -> MouseX,fev -> MouseY);
                  }
                  break;
               }
            }
//+

            F_Do(Obj,FM_Unlock);
            F_Do(fev -> Window,FM_Window_HandleEvent,fev);
            F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

            F_Dispose(fev);
         }
      
         ReplyMsg((struct Message *) msg);
         
         return TRUE;
      }
   }
   return FALSE;
}
//+
///app_check_broker_port
STATIC int32 app_check_broker_port(FClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> BrokerPort)
   {
      CxMsg *msg = (CxMsg *) GetMsg(LOD -> BrokerPort);

      if (msg)
      {
         uint32 id = CxMsgID(msg);
         uint32 type = CxMsgType(msg);

         ReplyMsg((struct Message *)(msg));

         switch (type)
         {
   //         case CXM_IEVENT:  F_DebugOut("App.Run - CX_IEvent\n"); break;
            case CXM_COMMAND: //F_DebugOut("App.Run - A command: ");
            {
               switch (id)
               {
                  case CXCMD_DISABLE:
   //                  F_DebugOut("CXCMD_DISABLE");
                     ActivateCxObj(LOD -> Broker,FALSE);
                  break;
                  case CXCMD_ENABLE:
   //                  F_DebugOut("CXCMD_ENABLE");
                     ActivateCxObj(LOD -> Broker,TRUE);
                  break;
                  case CXCMD_APPEAR:
   //                  F_DebugOut("CXCMD_APPEAR");
                     F_Set(Obj,FA_Application_Sleep,FALSE);
                  break;
                  case CXCMD_DISAPPEAR:
   //                  F_DebugOut("CXCMD_DISAPPEAR");
                     F_Set(Obj,FA_Application_Sleep,TRUE);
                  break;
                  case CXCMD_KILL:
   //                  F_DebugOut("CXCMD_KILL");
                     F_Do(Obj,FM_Application_Shutdown);
                  break;
               }
   //            F_DebugOut("\n");
            }
            break;
         }
         return TRUE;
      }
   }
   return FALSE;
}
//+
///app_find_timers
STATIC FTimeHeader * app_find_timers(struct LocalObjectData *LOD,FSignalHandler *Handler)
{
   FTimeHeader *time;

   for (time = (APTR)(LOD -> Timers.Head) ; time ; time = time -> Next)
   {
      if (time -> Handler == Handler)
      {
         return time;
      }
   }
   return NULL;
}
//+

/*** Methods ***************************************************************/

///App_Run
F_METHOD(void,App_Run)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    int8 read_again = FALSE;

    /* Messages are read one by one from each message port. A port receiving
    a   lot  of  messages  will  no  block  the  application.  The  variable
    'read_again' is set to TRUE each time a message is read. Thus if after a
    pass the variable 'read_again' is still FALSE the application can be set
    to wait for new signals.

    While the flag FF_Application_Run is set, the application runs. If  it's
    cleared the method exists. */

    LOD -> Flags |= FF_Application_Run;

    /* read objects data and preferences */

    F_Do(Obj,FM_Application_Load,FV_Application_ENV);

    if (!(LOD -> Flags & FF_APPLICATION_INHERITED_PREFS))
    {
       F_Do(LOD -> Preferences,F_IDO(FM_Preference_Read),FV_Preference_ENV);
    }

 /* FIXME: I should read some user  settings  to  know  if  windows  can  be
 opened, the user may want the application to start in sleep mode. */

    F_Set(Obj,FA_Application_Sleep,FALSE);

    app_collect_signals(Class,Obj);

    /* The application runs with its sempahore locked. It's unlock only when
    the main loop is left, when invoking objects. */

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    while (FF_Application_Run & LOD -> Flags)
    {
        struct Message *msg;
        bits32 sig;

        /* The FF_Application_Update flag is set when the application needs
        to  be  updated  because  preferences have been modified. A flag is
        used instead of a method because this is the easier  way  to  solve
        semaphore hanging troubles. Because application update always takes
        place from the main loop, it is totaly safe. */

        if (FF_Application_Update & LOD -> Flags)
        {
            LOD -> Flags &= ~FF_Application_Update;

            F_Do(Obj,FM_Application_Sleep);
            F_Do(Obj,FM_Application_Awake);
        }
    
/*** Waiting signals *******************************************************/
        
        if (read_again == FALSE)
        {
            F_Do(Obj,FM_Unlock);
           
            sig = Wait(LOD -> WaitSignals);
           
            F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
        }
        else
        {
            sig = 0; read_again = FALSE;
        }

/*** Signals ***************************************************************/

        if (LOD -> Signals.Head)
        {
            FSignalHandler *handler;
            APTR next = LOD -> Signals.Head;

            while ((handler = F_NextNode(&next)) != NULL)
            {
                if (sig & handler -> fsh_Signals)
                {
                    F_Do(Obj,FM_Unlock);
                    
                    F_Do(handler -> Object,handler -> Method);
                    
                    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
                }
            }
        }

        if (sig & SIGBREAKF_CTRL_C)
        {
            F_Do(Obj,FM_Application_Shutdown);
        }

/*** Pushed Methods ********************************************************/

        while (LOD -> PushedMethodList.Head)
        {
            FPush *push = (FPush *)(LOD -> PushedMethodList.Head);

            F_LinkRemove(&LOD -> PushedMethodList,(FNode *) push);

///DB_PUSH
         if (CUD -> db_Push)
         {
            F_Log(0,"PUSH: %s{%08lx}.0x%08lx { 0x%08lx 0x%08lx 0x%08lx 0x%08lx }",_classname(push -> Target),push -> Target,push -> Method,((ULONG *)(push -> Msg))[0],((ULONG *)(push -> Msg))[1],((ULONG *)(push -> Msg))[2],((ULONG *)(push -> Msg))[3]);
         }
//+

            F_Do(Obj,FM_Unlock);

            F_DoA(push -> Target,push -> Method,push -> Msg);
            
            F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

            F_Dispose(push);
        }

 /*** Rexx Port *************************************************************/

        msg = GetMsg(LOD -> AppPort);
       
        if (msg)
        {
            read_again = TRUE;

            if (IsRexxMsg((struct RexxMsg *)(msg)))
            {
                STRPTR cmd = ((struct RexxMsg *)(msg))  -> rm_Args[0];

                if (cmd)
                {
                    if (Stricmp(cmd,"QUIT") == 0)
                    {
                        F_Do(Obj,FM_Application_Shutdown);
                    }
                    else if (Stricmp(cmd,"HIDE") == 0)
                    {
                        F_Set(Obj,FA_Application_Sleep,TRUE);
                    }
                    else if (Stricmp(cmd,"SHOW") == 0)
                    {
                        F_Set(Obj,FA_Application_Sleep,FALSE);
                    }
                    else
                    {
                        ((struct RexxMsg *)(msg)) -> rm_Result1 = -3;
                    }
                }

                ReplyMsg(msg);
            }
        }

/*** Broker Port ***********************************************************/

        if (app_check_broker_port(Class,Obj))
        {
            read_again = TRUE;
        }

/*** Window Port ***********************************************************/

        if (app_check_window_port(Class,Obj))
        {
            read_again = TRUE;
        }

/*** Timer *****************************************************************/

        if (!IsMsgPortEmpty(LOD -> TimersPort))
        {
            msg = GetMsg(LOD -> TimersPort);
            
            if (msg)
            {
                FTimeNode *time = (APTR)((ULONG)(msg) - sizeof (FTimeHeader));
                struct timerequest *treq = (APTR)(msg);

                treq -> tr_node.io_Command = TR_ADDREQUEST;
                treq -> tr_time.tv_secs    = time -> Handler -> fsh_Secs;
                treq -> tr_time.tv_micro   = time -> Handler -> fsh_Micros;

                F_Do(Obj,FM_Unlock);
                
                if (F_Do(time -> Handler -> Object,time -> Handler -> Method))
                {
                    SendIO((struct IORequest *) treq);
                }
                
                F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
            }
            
            read_again = TRUE;
        }

        if (sig)
        {
            F_Do(Obj,FM_Unlock);
            
            F_Set(Obj,FA_Application_Signal,sig);
            
            F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
        }
    }

    F_Do(Obj,FM_Unlock);
}
//+
///App_Shutdown
F_METHOD(void,App_Shutdown)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> Flags &= ~FF_Application_Run;

    Signal(LOD -> AppPort -> mp_SigTask,1 << LOD -> AppPort -> mp_SigBit);
}
//+
///App_Awake
F_METHOD(void,App_Awake)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct Task *task = FindTask(NULL);
   BOOL err = FALSE;

   F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

   if (task != LOD -> AppPort -> mp_SigTask)
   {
      F_Log(0,"Method invoked by a foreign task (0x%08lx '%s'). AppTask 0x%08lx",task,task -> tc_Node.ln_Name,LOD -> AppPort -> mp_SigTask);
   }

//   F_DebugOut("App.Awake() - Task 0x%08lx ?= 0x%08lx\n",FindTask(NULL),LOD -> AppPort -> mp_SigTask);

   if (F_Do(Obj,FM_Application_Setup))
   {
      FFamilyNode *node;

      for (node = (FFamilyNode *) F_Get(Obj,FA_Family_Head) ; node ; node = node -> Next)
      {
         if (F_Get(node -> Object,FA_Window_Open))
         {
            if (!F_Do(node -> Object,FM_Window_Open))
            {
               F_Log(FV_LOG_USER,"Unable to Open Window 0x%08lx (ID %08lx)",node -> Object,F_Get(node -> Object,FA_ID));

               err = TRUE; break;
            }
         }
      }
   }
   else
   {
      F_Log(FV_LOG_USER,"Setup failed");

      err = TRUE;
   }

   if (err)
   {
      F_Do(Obj,FM_Application_Shutdown);
   }

   F_Do(Obj,FM_Unlock);
}
//+
///App_Sleep
F_METHOD(void,App_Sleep)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Task *task = FindTask(NULL);

   F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if (task != LOD -> AppPort -> mp_SigTask)
    {
        F_Log(0,"Method invoked from a foreign task (0x%08lx '%s'). AppTask 0x%08lx",task,task -> tc_Node.ln_Name,LOD -> AppPort -> mp_SigTask);
    }

    if (FF_Application_Setup & LOD -> Flags)
    {
        FFamilyNode *node;

        for (node = (FFamilyNode *) F_Get(Obj,FA_Family_Head) ; node ; node = node -> Next)
        {
            if (F_Get(node -> Object,FA_Window))
            {
                F_Do(node -> Object,FM_Window_Close);
            }
        }

        F_Do(Obj,FM_Application_Cleanup);
    }

    F_Do(Obj,FM_Unlock);
}
//+
///App_PushMethod
F_METHODM(LONG,App_PushMethod,FS_Application_PushMethod)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Target)
   {
      if (Msg -> Count < 256)
      {
         FPush *push = F_New(sizeof (FPush) + ((Msg -> Count + 1) * sizeof (ULONG)));

         if (push)
         {
            push -> Target = Msg -> Target;
            push -> Method = F_DynamicFindID((STRPTR)(Msg -> Method));
            push -> Msg    = (APTR)((ULONG)(push) + sizeof (FPush));

            CopyMem((APTR)((ULONG)(Msg) + sizeof (struct FS_Application_PushMethod)),push -> Msg,Msg -> Count * sizeof (ULONG));
            
            F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
            F_LinkTail(&LOD -> PushedMethodList,(FNode *) push);
            F_Do(Obj,FM_Unlock);

            Signal(LOD -> AppPort -> mp_SigTask,1 << LOD -> AppPort -> mp_SigBit);

            return TRUE;
         }
      }
      else
      {
         F_Log(FV_LOG_DEV,"Suspicious number of arguments (%ld) - Target %s{%08lx} - Method 0x%08lx",Msg -> Count,_classname(Msg -> Target),Msg -> Target,Msg -> Method);
      }
   }
   return FALSE;
}
//+
///App_AddSignal
F_METHODM(void,App_AddSignal,FS_Application_AddSignalHandler)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FSignalHandler *Handler = Msg -> Handler;

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if (FF_SignalHandler_Timer & Handler -> Flags)
    {
        if (!app_find_timers(LOD,Handler))
        {
            FTimeNode *time = F_New(sizeof (FTimeNode));
            
            if (time)
            {
                CopyMem(LOD -> TimeRequest,(APTR)(&time -> TReq),sizeof (struct timerequest));

                time -> Handler               = Handler;
                time -> TReq.tr_time.tv_secs  = Handler -> fsh_Secs;
                time -> TReq.tr_time.tv_micro = Handler -> fsh_Micros;

                F_LinkTail(&LOD -> Timers,(FNode *) time);

                SendIO((struct IORequest *)(&time -> TReq));

                Handler -> Flags |= FF_SignalHandler_Active;
            }
            else
            {
                F_Log(FV_LOG_USER,"Not enough memory");
            }
        }
        else
        {
            F_Log(FV_LOG_DEV,"Handler 0x%08lx already active",Handler);
        }
    }
    else
    {
        F_LinkTail(&LOD -> Signals,(FNode *) Handler);

        app_collect_signals(Class,Obj);
    }
    F_Do(Obj,FM_Unlock);
}
//+
///App_RemSignal
F_METHODM(void,App_RemSignal,FS_Application_RemSignalHandler)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FSignalHandler *Handler = Msg -> Handler;

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if (FF_SignalHandler_Timer & Handler -> Flags)
    {
        FTimeNode *time = (FTimeNode *) app_find_timers(LOD,Handler);

        if (time)
        {
            Handler -> Flags &= ~FF_SignalHandler_Active;

            if (CheckIO((struct IORequest *) &time -> TReq) == NULL)
            {
               AbortIO((struct IORequest *) &time -> TReq);
            }
            
            WaitIO((struct IORequest *) &time -> TReq);

            F_LinkRemove(&LOD -> Timers,(FNode *) time);

            F_Dispose(time);
        }
        else
        {
            F_Log(FV_LOG_DEV,"Unknow Handler 0x%08lx",Handler);
        }
    }
    else
    {
        F_LinkRemove(&LOD -> Signals,(FNode *) Handler);

        app_collect_signals(Class,Obj);
    }
    F_Do(Obj,FM_Unlock);
}
//+
///App_Update
F_METHOD(void,App_Update)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> Flags |= FF_Application_Update;

    Signal(LOD -> AppPort -> mp_SigTask,1 << LOD -> AppPort -> mp_SigBit);
}
//+
///App_ModifyEvents
F_METHODM(int32,App_ModifyEvents,FS_Application_ModifyEvents)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (LOD -> WindowPort)
    {
        struct Window *win = (struct Window *) F_Get(Msg -> Window,FA_Window);
        
        if (win)
        {
            bits32 idcmp = 0;
            
            if (FF_EVENT_TICK & Msg -> Events)
            {
                idcmp |= IDCMP_INTUITICKS;
            }
            if (FF_EVENT_KEY & Msg -> Events)
            {
                idcmp |= IDCMP_RAWKEY;
            }
            if (FF_EVENT_BUTTON & Msg -> Events)
            {
                idcmp |= IDCMP_MOUSEBUTTONS;
            }
            if (FF_EVENT_MOTION & Msg -> Events)
            {
                idcmp |= IDCMP_MOUSEMOVE;
            }
            if (FF_EVENT_WINDOW & Msg -> Events)
            {
                idcmp |= IDCMP_CLOSEWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_NEWSIZE | IDCMP_CHANGEWINDOW | IDCMP_REFRESHWINDOW;
            }
            if (FF_EVENT_DISK & Msg -> Events)
            {
                idcmp |= IDCMP_DISKINSERTED | IDCMP_DISKREMOVED;
            }

            if (CUD -> db_Events)
            {
                F_Log(0,"%s-%s-%s-%s-%s-%s",
                    (FF_EVENT_TICK & Msg -> Events) ? "tick" : "",
                    (FF_EVENT_KEY & Msg -> Events) ? "key" : "",
                    (FF_EVENT_BUTTON & Msg -> Events) ? "button" : "",
                    (FF_EVENT_MOTION & Msg -> Events) ? "motion" : "",
                    (FF_EVENT_WINDOW & Msg -> Events) ? "window" : "",
                    (FF_EVENT_DISK & Msg -> Events) ? "disk" : "");
            }
            
            if (idcmp)
            {
                win -> UserData = Msg -> Window;
                win -> UserPort = LOD -> WindowPort;
            }
            else
            {
                struct IntuiMessage *msg;
                struct Node *succ;
     
                win -> UserData = NULL;
                win -> UserPort = NULL;

                msg = (struct IntuiMessage *)(LOD -> WindowPort -> mp_MsgList.lh_Head);

                while ((succ = msg -> ExecMessage.mn_Node.ln_Succ) != NULL)
                {
                    if (msg -> IDCMPWindow == win)
                    {
                        Remove((struct Node *)(msg));
                        ReplyMsg((struct Message *)(msg));
                    }
                    msg = (struct IntuiMessage *)(succ);
                }
            }
            
            ModifyIDCMP(win,idcmp);
            
            return TRUE;
        }
    }
    return FALSE;
}
//+
