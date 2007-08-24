#include "Private.h"

#define DELAY_INITIAL_SECS                      2
#define DELAY_INITIAL_MICRO                     0
#define DELAY_NEXT_SECS                         0
#define DELAY_NEXT_MICRO                        10000

//#define DB_HANDLER_DISABLED

#ifndef DB_HANDLER_DISABLED
///input_handler
#ifndef __MORPHOS__
ASM(struct InputEvent *) input_handler
(
    REG(a0, struct InputEvent *ie),
    REG(a1, struct LocalObjectData *LOD)
)
{
#else
struct InputEvent * input_handler(void)
{
    struct InputEvent *ie = (struct InputEvent *) REG_A0;
    struct LocalObjectData *LOD = (struct LocalObjectData *) REG_A1;
#endif /*! __MORPHOS__ */

    struct FeelinBase *FeelinBase = LOD -> Feelin;
 
    if (ie -> ie_Class == IECLASS_TIMER)
    {
        if (LOD -> tv.tv_secs == 0)
        {
            LOD -> tv.tv_secs = ie -> ie_TimeStamp.tv_secs + DELAY_INITIAL_SECS;
            LOD -> tv.tv_micro = ie -> ie_TimeStamp.tv_micro + DELAY_INITIAL_MICRO;
        }
        else if (ie -> ie_TimeStamp.tv_secs >= LOD -> tv.tv_secs)
        {
            LOD -> tv.tv_secs = ie -> ie_TimeStamp.tv_secs + DELAY_NEXT_SECS;
            LOD -> tv.tv_micro = ie -> ie_TimeStamp.tv_micro + DELAY_NEXT_MICRO;
            
            if (!LOD -> help_window && !(FF_APPSERVER_DRAGGING & LOD -> Flags))
            {
                if (LOD -> help_screen != IntuitionBase -> ActiveScreen ||
                    LOD -> help_mousex != IntuitionBase -> ActiveScreen -> MouseX ||
                    LOD -> help_mousey != IntuitionBase -> ActiveScreen -> MouseY)
                {
                    LOD -> help_screen = IntuitionBase -> ActiveScreen;
                    LOD -> help_mousex = IntuitionBase -> ActiveScreen -> MouseX;
                    LOD -> help_mousey = IntuitionBase -> ActiveScreen -> MouseY;

//                    F_Log(0,"F_Do(%s{%08lx},0x%08lx,%ld",_classname(LOD -> Thread),LOD -> Thread,LOD -> id_Send,FV_Thread_Help_Show);
                    F_Do(LOD -> Thread,LOD -> id_Send,FV_Thread_Help_Show);
                }
            }

            LOD -> help_screen = IntuitionBase -> ActiveScreen;
            LOD -> help_mousex = IntuitionBase -> ActiveScreen -> MouseX;
            LOD -> help_mousey = IntuitionBase -> ActiveScreen -> MouseY;
        }
    }
#ifdef __AROS__
    else if (ie -> ie_Class != IECLASS_NULL)
#else
    else
#endif
    {
        LOD -> tv.tv_secs = ie -> ie_TimeStamp.tv_secs + DELAY_INITIAL_SECS;
        LOD -> tv.tv_micro = ie -> ie_TimeStamp.tv_micro + DELAY_INITIAL_MICRO;

        LOD -> help_screen = NULL;
        LOD -> help_mousex = 0;
        LOD -> help_mousey = 0;

        if (LOD -> help_window)
        {
//            F_Log(0,"F_Do(%s{%08lx},0x%08lx,%ld",_classname(LOD -> Thread),LOD -> Thread,LOD -> id_Send,FV_Thread_Help_Hide);
            F_Do(LOD -> Thread,LOD -> id_Send,FV_Thread_Help_Hide);
        }
   }

   return ie;
}

#ifdef __MORPHOS__
_MAKEGATE(input_handler, input_handler);
#endif

//+

///thread_init
uint32 thread_init(struct LocalObjectData *LOD,struct FeelinBase *FeelinBase)
{
    LOD -> Flags &= ~FF_APPSERVER_INITIATED;
  
    if ((LOD -> input_port = CreateMsgPort()))
    {
        if ((LOD -> input_request = CreateIORequest(LOD -> input_port,sizeof (struct IOStdReq))))
        {
            if (!OpenDevice("input.device",0,(struct IORequest *) LOD -> input_request,0))
            {
                LOD -> input_request -> io_Data = (APTR) &LOD -> input_handler;
                LOD -> input_request -> io_Command = IND_ADDHANDLER;
                DoIO((struct IORequest *) LOD -> input_request);

                LOD -> Flags |= FF_APPSERVER_INITIATED;
                
                return TRUE;
            }
            DeleteIORequest(LOD -> input_request); LOD -> input_request = NULL;
        }
        DeleteMsgPort(LOD -> input_port); LOD -> input_port = NULL;
    }
    return FALSE;
}
//+
///thread_exit
void thread_exit(struct LocalObjectData *LOD,struct FeelinBase *FeelinBase)
{
    if (FF_APPSERVER_INITIATED & LOD -> Flags)
    {
        LOD -> Flags &= ~FF_APPSERVER_INITIATED;
 
        LOD -> input_request -> io_Data = (APTR) &LOD -> input_handler;
        LOD -> input_request -> io_Command = IND_REMHANDLER;
        DoIO((struct IORequest *) LOD -> input_request);
            
        CloseDevice((struct IORequest *) LOD -> input_request);

        DeleteIORequest(LOD -> input_request); LOD -> input_request = NULL;
        
        DeleteMsgPort(LOD -> input_port); LOD -> input_port = NULL;
    }
}
//+
#endif

///Thread_Main
F_THREAD_ENTRY(Thread_Main)
{
    struct LocalObjectData *LOD = UserData;

    LOD -> Feelin = FeelinBase;
    
    LOD -> process = (struct Process *) FindTask(NULL);
    LOD -> signals = (1 << LOD -> process -> pr_MsgPort.mp_SigBit);

    #ifndef DB_HANDLER_DISABLED
 
    LOD -> input_handler.is_Code = (void (* )()) F_FUNCTION_GATE(input_handler);
    LOD -> input_handler.is_Data = LOD;
    LOD -> input_handler.is_Node.ln_Pri = 100;
    LOD -> input_handler.is_Node.ln_Name = "appserver.handler";
    
    LOD -> id_Send = F_DynamicFindID("FM_Thread_Send");
    
    #endif

    for (;;)
    {
        FThreadMsg *msg = (FThreadMsg *) GetMsg(&LOD -> process -> pr_MsgPort);

        if (msg)
        {
            switch (msg -> Action)
            {
                case FV_Thread_Hello:
                {
                    #ifndef DB_HANDLER_DISABLED
                     
                    msg -> Return = thread_init(LOD,FeelinBase);

                    #else

                    msg->Return = TRUE;
 
                    #endif
                }
                break;

                case FV_Thread_Bye:
                {
                    #ifndef DB_HANDLER_DISABLED
 
                    thread_exit(LOD,FeelinBase);
                    
                    #endif

                    msg -> Return = TRUE;
                    
                    ReplyMsg((struct Message *) msg);
                    
                    return;
                }
                break;
                        
                #ifndef DB_HANDLER_DISABLED
 
                case FV_Thread_Help_Show:
                {
                    FFamilyNode *app_node;

                    ReplyMsg((struct Message *) msg); msg = NULL;
      
                    F_Do(LOD -> Server,FM_Lock,FF_Lock_Shared);

                    for (app_node = (FFamilyNode *) F_Get(LOD -> Server,FA_Family_Head) ; app_node ; app_node = app_node -> Next)
                    {
                        FFamilyNode *win_node;

                        for (win_node = (FFamilyNode *) F_Get(app_node -> Object,FA_Family_Head) ; win_node ; win_node = win_node -> Next)
                        {
                            struct Window *win;

                            if ((win = (struct Window *) F_Get(win_node -> Object,FA_Window)) != NULL)
                            {
                                if (win -> WScreen == LOD -> help_screen)
                                {
                                    struct Layer *layer;

                                    if ((layer = WhichLayer(&LOD -> help_screen -> LayerInfo,LOD -> help_mousex,LOD -> help_mousey)))
                                    {
                                       if (layer -> Window == win)
                                       {
                                          F_Do(app_node -> Object,FM_Application_PushMethod,win_node -> Object,FM_Window_Help,3,FV_Window_Help_Show,LOD -> help_mousex,LOD -> help_mousey);

                                          LOD -> help_window = win_node -> Object;
                                       }
                                    }
                                }
                            }
                        }
                    }

                    F_Do(LOD -> Server,FM_Unlock);
                }
                break;
              
                case FV_Thread_Help_Hide:
                {
                    ReplyMsg((struct Message *) msg); msg = NULL;
                
                    if (LOD -> help_window)
                    {
                        F_Do((FObject) F_Get(LOD -> help_window,FA_Parent),FM_Application_PushMethod,LOD -> help_window,FM_Window_Help,1,FV_Window_Help_Hide);

                        LOD -> help_window = NULL;
                    }
                }
                break;
            
                #endif
            }
          
            if (msg)
            {
                ReplyMsg((struct Message *) msg);
            }
        }

        Wait(LOD -> signals);
    }
}
//+
