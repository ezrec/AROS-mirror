/*

$VER: Application 10.02 (2005/08/08)
 
    Portability update.
   
    Modify resources creation order.

*/

#include "Project.h"

struct ClassUserData                *CUD;

///METHODS
F_METHOD_PROTO(void,App_New);
F_METHOD_PROTO(void,App_Dispose);
F_METHOD_PROTO(void,App_Set);
F_METHOD_PROTO(void,App_Get);
F_METHOD_PROTO(void,App_AddMember);
F_METHOD_PROTO(void,App_RemMember);
F_METHOD_PROTO(void,App_Connect);
F_METHOD_PROTO(void,App_Disconnect);

F_METHOD_PROTO(void,App_Run);
F_METHOD_PROTO(void,App_Shutdown);
F_METHOD_PROTO(void,App_Awake);
F_METHOD_PROTO(void,App_Sleep);
F_METHOD_PROTO(void,App_PushMethod);
F_METHOD_PROTO(void,App_Setup);
F_METHOD_PROTO(void,App_Cleanup);
F_METHOD_PROTO(void,App_Load);
F_METHOD_PROTO(void,App_Save);
F_METHOD_PROTO(void,App_Resolve);
F_METHOD_PROTO(void,App_ResolveInt);
F_METHOD_PROTO(void,App_OpenFont);
F_METHOD_PROTO(void,App_AddSignal);
F_METHOD_PROTO(void,App_RemSignal);
F_METHOD_PROTO(void,App_Update);
F_METHOD_PROTO(void,App_ModifyEvents);

/* Preferences */

F_METHOD_PROTO(void,Prefs_New);
//+

///update_dbg

struct FS_DOSNotify_Notify                      { STRPTR FileName; };

F_HOOKM(void,update_dbg,FS_DOSNotify_Notify)
{
   STRPTR buf = F_New(1024);

   CUD -> db_Push   = FALSE;
   CUD -> db_Events = FALSE;

   if (buf)
   {
      BPTR file = Open(Msg -> FileName,MODE_OLDFILE);

      if (file)
      {
         if (FGets(file,buf,1024))
         {
            struct RDArgs *rda = (struct RDArgs *) AllocDosObject(DOS_RDARGS,NULL);

            if (rda)
            {
               rda -> RDA_Source.CS_Buffer = buf;
               rda -> RDA_Source.CS_Length = F_StrLen(buf);

               if (ReadArgs(TEMPLATE,&CUD -> db_Array,rda))
               {
                  FreeArgs(rda);
               }
               FreeDosObject(DOS_RDARGS,rda);
            }
         }
         Close(file);
      }
      F_Dispose(buf);
   }
}
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   CUD -> db_Notify = DOSNotifyObject,

      "FA_DOSNotify_Name",    "ENV:Feelin/DBG_APPLICATION",

      End;

   F_Do(CUD -> db_Notify,FM_Notify,"FA_DOSNotify_Update",FV_Notify_Always,
        FV_Notify_Self,FM_CallHookEntry,2,update_dbg,FV_Notify_Value);

   F_Set(CUD -> db_Notify,(uint32) "FA_DOSNotify_Update",
                          (uint32) "ENV:Feelin/DBG_APPLICATION");

   /*** open libraries ***/
 
   RexxSysBase = (struct RxsLib *) OpenLibrary("rexxsyslib.library",36);
   CxBase = OpenLibrary("commodities.library",39);
   IFFParseBase = OpenLibrary("iffparse.library",36);
   DiskfontBase = OpenLibrary("diskfont.library",39);
   KeymapBase = OpenLibrary("keymap.library",37);

   if (RexxSysBase && CxBase && IFFParseBase && DiskfontBase && KeymapBase)
   {
      return (FObject) F_SUPERDO();
   }

   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DisposeObj(CUD -> db_Notify); CUD -> db_Notify = NULL;

   CloseLibrary(KeymapBase); KeymapBase = NULL;
   CloseLibrary(DiskfontBase); DiskfontBase = NULL;
   CloseLibrary(IFFParseBase); IFFParseBase = NULL;
   CloseLibrary((struct Library *) RexxSysBase); RexxSysBase = NULL;
   CloseLibrary(CxBase); CxBase = NULL;
   
   F_SUPERDO();
}
//+

F_QUERY()
{
   switch (Which)
   {
///Meta
      case FV_Query_MetaClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Class_New,      FM_New),
            F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Class),
            F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD_BOTH("Application",    FV_TYPE_OBJECT,  FA_Application),
            F_ATTRIBUTES_ADD_BOTH("Title",          FV_TYPE_STRING,  FA_Application_Title),
            F_ATTRIBUTES_ADD_BOTH("Version",        FV_TYPE_STRING,  FA_Application_Version),
            F_ATTRIBUTES_ADD_BOTH("Copyright",      FV_TYPE_STRING,  FA_Application_Copyright),
            F_ATTRIBUTES_ADD_BOTH("Author",         FV_TYPE_STRING,  FA_Application_Author),
            F_ATTRIBUTES_ADD_BOTH("Description",    FV_TYPE_STRING,  FA_Application_Description),
            F_ATTRIBUTES_ADD_BOTH("Base",           FV_TYPE_STRING,  FA_Application_Base),
            F_ATTRIBUTES_ADD_BOTH("Unique",         FV_TYPE_BOOLEAN, FA_Application_Unique),
            F_ATTRIBUTES_ADD_BOTH("Signal",         FV_TYPE_INTEGER, FA_Application_Signal),
            F_ATTRIBUTES_ADD_BOTH("UserSignals",    FV_TYPE_INTEGER, FA_Application_UserSignals),
            F_ATTRIBUTES_ADD_BOTH("WindowPort",     FV_TYPE_POINTER, FA_Application_WindowPort),
            F_ATTRIBUTES_ADD_BOTH("BrokerPort",     FV_TYPE_POINTER, FA_Application_BrokerPort),
            F_ATTRIBUTES_ADD_BOTH("BrokerHook",     FV_TYPE_POINTER, FA_Application_BrokerHook),
            F_ATTRIBUTES_ADD_BOTH("BrokerPri",      FV_TYPE_INTEGER, FA_Application_BrokerPri),
            F_ATTRIBUTES_ADD_BOTH("Broker",         FV_TYPE_POINTER, FA_Application_Broker),
            F_ATTRIBUTES_ADD_BOTH("Display",        FV_TYPE_OBJECT,  FA_Application_Display),
            F_ATTRIBUTES_ADD_BOTH("Dataspace",      FV_TYPE_OBJECT,  FA_Application_Dataspace),
            F_ATTRIBUTES_ADD_BOTH("Preference",     FV_TYPE_OBJECT,  FA_Application_Preference),
            F_ATTRIBUTES_ADD_BOTH("Sleep",          FV_TYPE_BOOLEAN, FA_Application_Sleep),
            F_ATTRIBUTES_ADD_BOTH("ResolveMapping", FV_TYPE_POINTER, FA_Application_ResolveMapping),
            F_ATTRIBUTES_ADD_BOTH("ColorScheme",    FV_TYPE_STRING,  FA_ColorScheme),

            F_ARRAY_END
         };

         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(App_New,          FM_New),
            F_METHODS_ADD_STATIC(App_Dispose,      FM_Dispose),
            F_METHODS_ADD_STATIC(App_Get,          FM_Get),
            F_METHODS_ADD_STATIC(App_Set,          FM_Set),
            F_METHODS_ADD_STATIC(App_Connect,      FM_Connect),
            F_METHODS_ADD_STATIC(App_Disconnect,   FM_Disconnect),
            F_METHODS_ADD_STATIC(App_AddMember,    FM_AddMember),
            F_METHODS_ADD_STATIC(App_RemMember,    FM_RemMember),

            F_METHODS_ADD_BOTH(App_Run,          "Run",              FM_Application_Run),
            F_METHODS_ADD_BOTH(App_Shutdown,     "Shutdown",         FM_Application_Shutdown),
            F_METHODS_ADD_BOTH(App_Sleep,        "Sleep",            FM_Application_Sleep),
            F_METHODS_ADD_BOTH(App_Awake,        "Awake",            FM_Application_Awake),
            F_METHODS_ADD_BOTH(App_PushMethod,   "PushMethod",       FM_Application_PushMethod),
            F_METHODS_ADD_BOTH(App_Setup,        "Setup",            FM_Application_Setup),
            F_METHODS_ADD_BOTH(App_Cleanup,      "Cleanup",          FM_Application_Cleanup),
            F_METHODS_ADD_BOTH(App_Load,         "Load",             FM_Application_Load),
            F_METHODS_ADD_BOTH(App_Save,         "Save",             FM_Application_Save),
            F_METHODS_ADD_BOTH(App_Resolve,      "Resolve",          FM_Application_Resolve),
            F_METHODS_ADD_BOTH(App_ResolveInt,   "ResolveInt",       FM_Application_ResolveInt),
            F_METHODS_ADD_BOTH(App_OpenFont,     "OpenFont",         FM_Application_OpenFont),
            F_METHODS_ADD_BOTH(App_AddSignal,    "AddSignalHandler", FM_Application_AddSignalHandler),
            F_METHODS_ADD_BOTH(App_RemSignal,    "RemSignalHandler", FM_Application_RemSignalHandler),
            F_METHODS_ADD_BOTH(App_Update,       "Update",           FM_Application_Update),
            F_METHODS_ADD_BOTH(App_ModifyEvents, "ModifyEvents",     FM_Application_ModifyEvents),

            F_ARRAY_END
         };

         STATIC F_AUTOS_ARRAY =
         {
            F_AUTOS_ADD("FM_Dataspace_Add"),
            F_AUTOS_ADD("FM_Dataspace_Find"),
            F_AUTOS_ADD("FM_Dataspace_Clear"),
            F_AUTOS_ADD("FM_Dataspace_ReadIFF"),
            F_AUTOS_ADD("FM_Dataspace_WriteIFF"),
            F_AUTOS_ADD("FM_Preference_Read"),
            F_AUTOS_ADD("FM_Preference_Resolve"),
            F_AUTOS_ADD("FM_Preference_ResolveInt"),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Family),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_ATTRIBUTES,
            F_TAGS_ADD_AUTOS,
            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Prefs
      case FV_Query_PrefsTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Prefs_New, FM_New),
             
            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(PreferenceGroup),
            F_TAGS_ADD_METHODS,
            
            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
   }
   return NULL;
}

/*

$VER: Application 10.10 (2005/04/29)

    The semaphore is now unlocked when  waiting  signals.  Waiting  signals
    under  the  protected state of the semaphore locking, was locking other
    tasks pushing methods.

    The flag FF_EVENT_BUTTON_DOUBLE is  now  set  with  mouse  button  down
    (FF_EVENT_BUTTON_DOWN), instead of mouse button up.

    Replaced event class codes FV_EVENT_BUTTON_UP and  FF_EVENT_BUTTON_DOWN
    by FV_EVENT_BUTTON_WHEEL. The flag FF_EVENT_BUTTON_DOWN indicates wheel
    direction.

$VER: Application 10.00 (2005/02/04)

    Added    preference     resolving     mapping.     With     the     new
    FA_Application_ResolveMapping  attribute,  the developer can now easily
    override any user preference item. This was  a  problem  with  previous
    version  because  some  preference  items  cannot  be  overridden  e.g.
    "FP_Decorator_AScheme". It's so easy now, it's nearly ridiculous :-)

    Own event system, IDCMP is deprecated. Icon drop support.

$VER: Application 09.00 (2004/12/18)

    The class is now a subclass of FC_Family.

    The variable DBG_APPLICATION can be used to toggle verbose output.  The
    variable is read with ReadArgs(). Thus, many switches can be defined in
    a single file.

    The method FM_Application_Run has been completely  rewritten.  It  runs
    locked  and  only  unlocks when exiting the loop (e.g. when invoking an
    object). The implementation of the  FM_Application_Run  method  is  far
    more  secure than before. More over, the application runs faster and is
    more responsive.

    New preference update mechanism.

    The      flag      FF_SignalHandler_Active      is      set      during
    FM_Application_AddSignalHandler, to indicate that the handler is linked
    and active, and cleared  during  FM_Application_RemSignalHandler.  This
    flag  is  not  used by FC_Application, but can be used by developers to
    check if a handler is active before removing  it,  although  trying  to
    remove  a  not  active  (linked) handler causes no harms, but a message
    logging.

    ** PRIVATE **

    The method FM_Application_Shutdown does no longer send a message (which
    could fails to allocate). Instead, the flag FF_Application_Run is used.
    When  the   flag   is   cleared   the   application   exit   from   the
    FM_Application_Run.  The  FF_Application_Update  flag  is  also  use to
    indicate that the application should update  because  preferences  have
    changed.

*/
