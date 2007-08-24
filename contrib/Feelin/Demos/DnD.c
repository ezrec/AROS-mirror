/*
   DnD Demo

   2000-2005 © Olivier LAVIALE <gofromiel@gofromiel.com>

*/

#include <intuition/intuition.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/feelin.h>

struct  FeelinBase    *FeelinBase;

#ifdef __VBCC__
#undef GfxBase
struct GfxBase *GfxBase;
#endif

// This is the instance data for our custom class.

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
};
 
///mNew
F_METHOD(uint32,mNew)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;

   return F_SuperDo(Class,Obj,Method,
   
       FA_Frame,         "$text-frame",
       FA_Back,          "$text-back",
       FA_ColorScheme,   "$text-scheme",
       FA_Font,          "$font-big",
       FA_Text_VCenter,  TRUE,
       FA_Text_HCenter,  TRUE,
       FA_Dropable,      TRUE,
       
       TAG_MORE,Msg);
}
//+
/// mSetup
/*

   FM_ModifyHandler creates a FEventHandler on the  fly  depending  on  the
   resulting  modified  events. Using FM_ModifyHandler it's a piece of cake
   ot request events.

*/
F_METHOD(uint32,mSetup)
{
    F_Do(Obj,FM_ModifyHandler,FF_EVENT_WBDROP,0);

    return F_SUPERDO();
}
//+
/// mCleanup
F_METHOD(uint32,mCleanup)
{
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_WBDROP);

    return F_SUPERDO();
}
//+
///mHandleEvent
/*

in FM_Setup we said that we want get a message if an object was droped from
the workbench, so we have to define the event-handler.

*/

F_METHODM(uint32,mHandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;
  
    #define _between(a,x,b) ((x) >= (a) && (x) <= (b))
    #define _isinobject(x,y) (_between(_ix,(x),_ix2) && _between(_iy,(y),_iy2))

    if (fev -> Class == FF_EVENT_WBDROP)
    {
        if (_isinobject(fev -> MouseX,fev -> MouseY))
        {
            uint32 narg = ((struct AppMessage *)(fev -> IMsg)) -> am_NumArgs;
            struct WBArg *arg = ((struct AppMessage *)(fev -> IMsg)) -> am_ArgList;
            STRPTR buf;
            
            if ((buf = F_New(1024)) != NULL)
            {
                while (narg--)
                {
                    NameFromLock(arg -> wa_Lock,buf,1023);
                    AddPart(buf,arg -> wa_Name,1023);
      
                    F_Log(0,"WBDrop (%s)(0x%08lx)",buf,arg -> wa_Lock);
                 
                    arg++;
                }

                F_Dispose(buf);
            }

            return FF_HandleEvent_Eat;
        }
    }
    return F_SUPERDO();
}
//+
///mDnDDrop
F_METHODM(uint32,mDnDDrop,FS_DnDDrop)
{
    F_Log(0,"DnDDrop: Source %s{0x%08lx}",_classname(Msg -> Source), Msg->Source);
   
    return F_SUPERDO();
}
//+

/// Main
int main()
{
    FClass *cc;
    FObject app,win,myobj;

    STATIC F_METHODS_ARRAY =
    {
        F_METHODS_ADD_STATIC(mNew,         FM_New),
        F_METHODS_ADD_STATIC(mSetup,       FM_Setup),
        F_METHODS_ADD_STATIC(mCleanup,     FM_Cleanup),
        F_METHODS_ADD_STATIC(mHandleEvent, FM_HandleEvent),
        F_METHODS_ADD_STATIC(mDnDDrop,     FM_DnDDrop),

        F_ARRAY_END
    };

    if (F_FEELIN_OPEN)
    {
        #ifdef __VBCC__
        GfxBase = FeelinBase -> Graphics;
        #endif

        /* Create the new custom class with a call to F_CreateClassA().

        This function returns a FClass. You must use Class -> Name to  create
        instance  of  your  custom  class.  This  Name  is unique and made by
        F_CreateClassA(). */

        cc = F_CreateClass(NULL,

            FA_Class_Super,    FC_Text,
            FA_Class_LODSize,  sizeof (struct LocalObjectData),
            FA_Class_Methods,  F_METHODS,
            
            TAG_DONE);
        
        if (cc)
        {
           app = AppObject,
              Child, win = WindowObject,
                 FA_ID,            MAKE_ID('M','A','I','N'),
                 FA_Window_Title, "Feelin : DnD Demo",
                 FA_Window_Open,   TRUE,

                 Child, VGroup,
                    Child, TextObject, FA_SetMax,FV_SetHeight, DontChain, TextFrame, TextBack, FA_Text, "<align=center><pens style=glow>Drag'n'Drop demo</pens><br>You can also drop workbench objects !", End,
                    
                    Child, HGroup,
                       Child, F_MakeObj(FV_MakeObj_Button,"Drag Me",FA_Draggable,TRUE,FA_SetMax,FV_SetNone,FA_Text_VCenter,TRUE,FA_Font,"$font-big",TAG_DONE),
                       Child, myobj = F_NewObj(cc -> Name,FA_Text,"Drop on Me",TAG_DONE),
                    End,
                 End,
              End,
           End;

           if (app)
           {
              F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE, app,FM_Application_Shutdown,0);
              F_Do(app,FM_Application_Run);
              F_DisposeObj(app);
           }

           F_DeleteClass(cc);
        }
        else
        {
           Printf("Could not create custom class.\n");
        }
        
        F_FEELIN_CLOSE;
    }
    else
    {
        Printf("Failed to open feelin.library\n");
    }
    return 0;
}
//+
