#include "Private.h"

//#define DB_HANDLEEVENT
//#define DB_ADDEVENTHANDLER
//#define DB_REMEVENTHANDLER

///window_refresh
static void window_refresh(FClass *Class, FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj, FM_Window_Draw);

    /* FIXME:? re-activate activeobject to draw its activate state */

    F_Set(Obj, FA_Window_ActiveObject, (uint32) LOD->ActiveObj);
}
//+

/*** Shared ****************************************************************/

///window_collect_events
uint32 window_collect_events(struct FeelinClass *Class,FObject Obj)
{
    struct LocalObjectData    *LOD = F_LOD(Class,Obj);
    struct FeelinEventHandler *handler;
    
    uint32 events = FF_EVENT_KEY | FF_EVENT_BUTTON | FF_EVENT_WINDOW;

    for (handler = (struct FeelinEventHandler *)(LOD -> EventHandlers.Head) ; handler ; handler = handler -> Next)
    {
        events |= handler -> Events;
    }

    return events;
}
//+

/*** Methods ***************************************************************/

///Window_HandleEvent
F_METHODM(void,Window_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;
    FEventHandler *node;
//   struct FS_BuildContextMenu    bcm;
    APTR                          next;

#ifdef DB_HANDLEEVENT
    F_Log(0,"Class 0x%08lx (%04lx - %04lx) - Key %02lx Decoded (%lc)",fev -> Class,fev -> Code,fev -> Qualifier,fev -> Key,(fev -> DecodedChar) ? fev -> DecodedChar : '#');
#endif
    
    if (fev -> Class == FF_EVENT_WINDOW)
    {
        switch (fev -> Code)
        {
            case FV_EVENT_WINDOW_SIZE:
            {
                
                /* In case FV_EVENT_WINDOW_SIZE occurs several time  before
                FV_EVENT_WINDOW_REFRESH occurs. */
                
                #if 1
                if (((FF_WINDOW_SYS_REFRESH_SIMPLE & LOD->SYSFlags) == 0) &&
                    ((FF_WINDOW_SYS_LAYOUT_EVENT & LOD->SYSFlags) == 0))
                {
                    F_Set(LOD->Render, FA_Render_Forbid, TRUE);
                    
                    LOD->SYSFlags |= FF_WINDOW_SYS_LAYOUT_EVENT;
                }
                #endif
 
//                F_Log(0,"event-window-layout >> BEGIN");

                F_Do(Obj,FM_Window_Layout);

                /* a FV_EVENT_WINDOW_REFRESH event must  come  right  after
                the FV_EVENT_WINDOW_SIZE. */
                
                #if 1
                
                if (FF_WINDOW_SYS_REFRESH_SIMPLE & LOD->SYSFlags)
                {
                    window_refresh(Class, Obj);
                    
                    LOD->SYSFlags |= FF_WINDOW_SYS_REFRESH_DONE;
                }
                #endif

//                F_Log(0,"event-window-layout >> DONE");
            }
            break;

            case FV_EVENT_WINDOW_REFRESH:
            {
//                F_Log(0,"event-window-refresh");
 
                if (FF_WINDOW_SYS_LAYOUT_EVENT & LOD->SYSFlags)
                {
                    LOD->SYSFlags &= ~FF_WINDOW_SYS_LAYOUT_EVENT;
                    
                    F_Set(LOD->Render, FA_Render_Forbid, FALSE);
                }
 
                if (FF_WINDOW_SYS_REFRESH_SIMPLE & LOD->SYSFlags)
                {
                    if ((FF_WINDOW_SYS_REFRESH_DONE & LOD->SYSFlags) == 0)
                    {
                        #if 1

                        BeginRefresh(LOD->Win);

                        window_refresh(Class, Obj);

                        EndRefresh(LOD->Win, FALSE);

                        #else
                        
                        window_refresh(Class, Obj);
                        
                        #endif
                    }
                    else
                    {
                        LOD->SYSFlags &= ~FF_WINDOW_SYS_REFRESH_DONE;
                    }
                }
                else
                {
                    F_Do(Obj, FM_Window_Draw);
                }
            }
            break;

            case FV_EVENT_WINDOW_CLOSE:
            {
                F_Do(LOD -> Application,FM_Application_PushMethod,Obj,FM_Set,2,FA_Window_CloseRequest,TRUE);
            }
            break;

            case FV_EVENT_WINDOW_ACTIVE:
            {
                LOD -> WINFlags |= FF_WINDOW_WIN_ACTIVE;
                F_SuperDo(Class,Obj,FM_Set,FA_Window_Active,TRUE, TAG_DONE);
            }
            break;

            case FV_EVENT_WINDOW_INACTIVE:
            {
                LOD -> WINFlags &= ~FF_WINDOW_WIN_ACTIVE;
                F_SuperDo(Class,Obj,FM_Set,FA_Window_Active,FALSE, TAG_DONE);
            }
            break;

            case FV_EVENT_WINDOW_CHANGE:
            {
                if (LOD -> Box.x != LOD -> Win -> LeftEdge &&
                     LOD -> Box.y != LOD -> Win -> TopEdge  &&
                     LOD -> Box.w != LOD -> Win -> Width    &&
                     LOD -> Box.h != LOD -> Win -> Height)
                {
                    F_Log(0,"ZOOM");
                }

                LOD -> Box.x = LOD -> Win -> LeftEdge; LOD -> Box.w = LOD -> Win -> Width;
                LOD -> Box.y = LOD -> Win -> TopEdge;  LOD -> Box.h = LOD -> Win -> Height;
            }
            break;
        }
    }
    
// First check ActiveObject

    if (LOD -> ActiveObj)
    {
        next = LOD -> EventHandlers.Head;

        while ((node = F_NextNode(&next)) != NULL)
        {
            if (node -> Object == LOD -> ActiveObj)
            {
                if (node -> Events & fev -> Class)
                {
                    if (FF_HandleEvent_Eat & F_ClassDoA((node -> Class) ? node -> Class : _class(node -> Object),node -> Object,FM_HandleEvent,Msg))
                    {
                        return;
                    }
                }
                break;
            }
        }
    }

    // Event Handlers

    next = LOD -> EventHandlers.Head;

    while ((node = F_NextNode(&next)) != NULL)
    {
/*
        {
            FAreaPublic *ad = (FAreaPublic *) F_Get(node -> Object,FA_AreaPublic);

            if (ad)
            {
                if (!(FF_Area_CanDraw & ad -> Flags))
                {
                    F_Log(0,"(db)%s{%08lx} still linked (0x%08lx)",_classname(node -> Object),node -> Object,node);

                    continue;
                }
            }
        }
*/
        //F_Log(0,"HANDLER 0x%08lx - %s{%08lx}",node,_classname(node -> Object),node -> Object);
        
        if (node -> Events & fev -> Class)
        {
            if (FF_HandleEvent_Eat & F_ClassDoA((node -> Class) ? node -> Class : _class(node -> Object),node -> Object,FM_HandleEvent,Msg))
            {
                return;
            }
        }
    }

/*** Nobody ate the event, we handle what we can ***************************/

    switch (fev -> Key)
    {
        case FV_KEY_NEXTOBJ:
        case FV_KEY_DOWN:
        case FV_KEY_RIGHT:
        {
            F_Set(Obj,FA_Window_ActiveObject,FV_Window_ActiveObject_Next);
        }
        break;

        case FV_KEY_PREVOBJ:
        case FV_KEY_UP:
        case FV_KEY_LEFT:
        {
            F_Set(Obj,FA_Window_ActiveObject,FV_Window_ActiveObject_Prev);
        }
        break;

        case FV_KEY_CLOSEWINDOW:
        {
            F_Do(LOD -> Application,FM_Application_PushMethod,Obj,FM_Set,2,FA_Window_CloseRequest,TRUE);
        }
        break;
    }
}
//+
///Window_AddEventHandler
F_METHODM(void,Window_AddEventHandler,FS_Window_AddEventHandler)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEventHandler *Handler = Msg -> Handler,*prev;

    if (F_LinkMember(&LOD -> EventHandlers,(FNode *) Handler))
    {
        F_Log(FV_LOG_DEV,"Handler 0x%08lx already added - %s{%08lx}",Handler,_classname(Handler -> Object),Handler -> Object);

        return;
    }
    else
    {
        #ifdef DB_ADDEVENTHANDLER
        F_Log(0,"Handler 0x%08lx - Event 0x%08lx - Obj %s{%08lx}",Handler,Handler -> Events,_classname(Handler -> Object),Handler -> Object);
        #endif

        for (prev = (APTR)(LOD -> EventHandlers.Tail) ; prev ; prev = prev -> Prev)
        {
            if (Handler -> Priority <= prev -> Priority) break;
        }

        F_LinkInsert(&LOD -> EventHandlers,(FNode *) Handler,(FNode *) prev);

        Handler -> Flags |= FF_EventHandler_Active;
    }

    if (LOD -> Win)
    {
        F_Do(LOD -> Application,FM_Application_ModifyEvents,window_collect_events(Class,Obj),Obj);
    }
}
//+
///Window_RemEventHandler
F_METHODM(void,Window_RemEventHandler,FS_Window_RemEventHandler)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_LinkMember(&LOD -> EventHandlers,(FNode *) Msg -> Handler))
    {
        #ifdef DB_REMEVENTHANDLER
        F_Log(0,"Handler 0x%08lx - Object %s{%08lx} (%s)",Msg -> Handler,_classname(Msg -> Handler -> Object),Msg -> Handler -> Object,(Msg -> Handler -> Class) ? Msg -> Handler -> Class -> Name : (STRPTR)("self"));
        #endif

        F_LinkRemove(&LOD -> EventHandlers,(FNode *) Msg -> Handler);

        if (LOD -> Win)
        {
            F_Do(LOD -> Application,FM_Application_ModifyEvents,window_collect_events(Class,Obj),Obj);
        }

        Msg -> Handler -> Flags &= ~FF_EventHandler_Active;
    }
    else
    {
        F_Log(FV_LOG_DEV,"Handler 0x%08lx is not linked - %s{%08lx}",Msg -> Handler,_classname(Msg -> Handler -> Object),Msg -> Handler -> Object);
    }
}
//+

///Window_ChainAdd
F_METHODM(LONG,Window_ChainAdd,FS_Window_ChainAdd)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Object)
    {
        FAreaPublic *ad = (FAreaPublic *) F_Get(Msg -> Object,FA_Area_PublicData);

        if (ad)
        {
            FCycleNode *node;

            for (node = (FCycleNode *) LOD -> CycleChain.Head ; node ; node = node -> Next)
            {
                if (node -> Object == Msg -> Object) break;
            }

            if (node)
            {
                F_Log(FV_LOG_DEV,"%s{%08lx} already member of the cycle chain",_classname(Msg -> Object),Msg -> Object);
            }
            else if ((node = F_NewP(LOD -> CyclePool,sizeof (FCycleNode))) != NULL)
            {
                node -> Object = Msg -> Object;
                node -> AreaPublic = ad;

                F_LinkTail(&LOD -> CycleChain,(FNode *) node);

                return TRUE;
            }
        }
    }
    return FALSE;
}
//+
///Window_ChainRem
F_METHODM(LONG,Window_ChainRem,FS_Window_ChainRem)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FCycleNode *node;

    for (node = (FCycleNode *) LOD -> CycleChain.Head ; node ; node = node -> Next)
    {
        if (node -> Object == Msg -> Object)
        {
            F_LinkRemove(&LOD -> CycleChain,(FNode *) node);
            
            F_Dispose(node);

            return TRUE;
        }
    }
    return FALSE;
}
//+

///Window_Zoom
F_METHODM(void,Window_Zoom,FS_Window_Zoom)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Win)
    {
        switch (Msg -> Zoom)
        {
            case FV_Window_Zoom_Toggle:
            {
//            F_Log(0,"DIM %ld:%ld %ldx%ld - ZOOM %ld:%ld %ldx%ld\n",LOD -> Box.x,LOD -> Box.y,LOD -> Box.w,LOD -> Box.h,LOD -> Zoom.x,LOD -> Zoom.y,LOD -> Zoom.w,LOD -> Zoom.h);

                ZipWindow(LOD -> Win);
            }
            break;

            case FV_Window_Zoom_Max:
            {
                ZipWindow(LOD -> Win);
            }
            break;

            case FV_Window_Zoom_Full:
            {
                ZipWindow(LOD -> Win);
            }
            break;
        }
    }
}
//+
///Window_Depth
F_METHODM(void,Window_Depth,FS_Window_Depth)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Win)
    {
        switch (Msg -> Depth)
        {
            case FV_Window_Depth_Toggle:
            {
                if (LOD -> Win -> RPort -> Layer -> front)
                {
                    WindowToFront(LOD -> Win);
                }
                else
                {
                    WindowToBack(LOD -> Win);
                }
            }
            break;

            case FV_Window_Depth_Front:
            {
                WindowToFront(LOD -> Win);
            }
            break;

            case FV_Window_Depth_Back:
            {
                WindowToBack(LOD -> Win);
            }
            break;
        }
    }
}
//+
///Window_Help
F_METHODM(void,Window_Help,FS_Window_Help)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Win)
    {
        if (Msg -> Action == FV_Window_Help_Show)
        {
            if (!LOD -> PopHelp)
            {
                STRPTR help = (STRPTR) F_Do(LOD -> Decorator,FM_BuildContextHelp,Msg -> MouseX - LOD -> Box.x,Msg -> MouseY - LOD -> Box.y);
                if (!help) help = (STRPTR) F_Do(LOD -> Root,FM_BuildContextHelp,Msg -> MouseX - LOD -> Box.x,Msg -> MouseY - LOD -> Box.y);

                if (help)
                {
                    LOD -> PopHelp = PopHelpObject,

                        "FA_PopHelp_Text",     help,
                        "FA_PopHelp_Window",   Obj,
                        "FA_PopHelp_Open",     TRUE,

                    End;
                }
            }
        }
        else if (Msg -> Action == FV_Window_Help_Hide)
        {
            F_DisposeObj(LOD -> PopHelp); LOD -> PopHelp = NULL;
        }
    }
}
//+

