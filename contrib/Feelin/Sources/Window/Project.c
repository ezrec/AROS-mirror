/*

$VER: 11.00 (2005/08/19)
 
    New Feelin design.
   
    Use FC_Frame rendering instead of my own routines.
    
    New "damaged" technique used for rendering and  rethinking  (the  5th).
    This one is for sure the best. The FM_RethinkLayout is now deprecated.
    
    The FM_AskMinMax method is now invoked on the Decorator.

    [GROG] Added support for simple refresh windows.
 
$VER: 10.00 (2005/05/16)
 
    FC_Window is now a subclass of FC_Frame.

    New rethink layout system (the 4th). This one is the good one :-)

    Removed deprecated events  related  code,  which  are  now  handled  by
    FC_Application  (particuliarly  the  window  message  port  which is no
    longer handled by FC_Window).
   
    New context help support. Remove the FC_Area method FM_BuildContextHelp.
   
    Initial coordinates (left & top) and dimensions (width  &  height)  can
    now  be  defined  with FA_Left, FA_Top, FA_Width and FA_Height. You can
    supply a numeric value as well as strings. Using strings, values can be
    expressed in pixel or in percent (e.g. "120", "120px", "70%")
   
$VER: 09.00 (2005/02/04)

    Feelin events introduction.

*/

#include "Project.h"
#include "_locale/table.h"

struct Library                     *WorkbenchBase;

#ifdef __MORPHOS__
struct IntuitionBase               *IntuitionBase;
#endif

///METHODS
F_METHOD_PROTO(void,Window_New);
F_METHOD_PROTO(void,Window_Dispose);
F_METHOD_PROTO(void,Window_Get);
F_METHOD_PROTO(void,Window_Set);
F_METHOD_PROTO(void,Window_Import);
F_METHOD_PROTO(void,Window_Export);
F_METHOD_PROTO(void,Window_Connect);
F_METHOD_PROTO(void,Window_Disconnect);
F_METHOD_PROTO(void,Window_AddMember);
F_METHOD_PROTO(void,Window_RemMember);
F_METHOD_PROTO(void,Window_Setup);
F_METHOD_PROTO(void,Window_Cleanup);
F_METHOD_PROTO(void,Window_Open);
F_METHOD_PROTO(void,Window_Close);
F_METHOD_PROTO(void,Window_Layout);
F_METHOD_PROTO(void,Window_Draw);
F_METHOD_PROTO(void,Window_Erase);
F_METHOD_PROTO(void,Window_HandleEvent);
F_METHOD_PROTO(void,Window_AddEventHandler);
F_METHOD_PROTO(void,Window_RemEventHandler);
F_METHOD_PROTO(void,Window_ChainAdd);
F_METHOD_PROTO(void,Window_ChainRem);
F_METHOD_PROTO(void,Window_Zoom);
F_METHOD_PROTO(void,Window_Depth);
F_METHOD_PROTO(void,Window_Help);
F_METHOD_PROTO(void,Window_RequestRethink);
F_METHOD_PROTO(void,Prefs_New);
//+

///Class_New
F_METHOD(uint32,Class_New)
{
    #ifdef __MORPHOS__
    IntuitionBase = FeelinBase->Intuition;
    #endif
  
    WorkbenchBase = OpenLibrary("workbench.library",39);

    if (WorkbenchBase != NULL)
    {
       return F_SUPERDO();
    }
    else
    {
       F_Log(FV_LOG_USER,"Unable to open %s v%ld","workbench.library",39);
    }
    return 0;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
    if (WorkbenchBase)
    {
        CloseLibrary(WorkbenchBase); WorkbenchBase = NULL;
    }
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
                F_TAGS_ADD_METHODS,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(ActiveObject) =
            {
                F_VALUES_ADD("Prev",  FV_Window_ActiveObject_Prev),
                F_VALUES_ADD("Next",  FV_Window_ActiveObject_Next),
                F_VALUES_ADD("None",  FV_Window_ActiveObject_None),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_BOTH("Window",         FV_TYPE_POINTER, FA_Window),
                F_ATTRIBUTES_ADD_BOTH("Title",          FV_TYPE_STRING,  FA_Window_Title),
                F_ATTRIBUTES_ADD_BOTH("ScreenTitle",    FV_TYPE_STRING,  FA_Window_ScreenTitle),
                F_ATTRIBUTES_ADD_BOTH("Open",           FV_TYPE_BOOLEAN, FA_Window_Open),
                F_ATTRIBUTES_ADD_BOTH("CloseRequest",   FV_TYPE_BOOLEAN, FA_Window_CloseRequest),
                F_ATTRIBUTES_ADD_BOTH("Active",         FV_TYPE_BOOLEAN, FA_Window_Active),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("ActiveObject",   FV_TYPE_OBJECT,  FA_Window_ActiveObject, ActiveObject),
                F_ATTRIBUTES_ADD_BOTH("Backdrop",       FV_TYPE_OBJECT,  FA_Window_Backdrop),
                F_ATTRIBUTES_ADD_BOTH("Borderless",     FV_TYPE_BOOLEAN, FA_Window_Borderless),
                F_ATTRIBUTES_ADD_BOTH("Resizable",      FV_TYPE_BOOLEAN, FA_Window_Resizable),
                F_ATTRIBUTES_ADD_BOTH("GadNone",        FV_TYPE_BOOLEAN, FA_Window_GadNone),
                F_ATTRIBUTES_ADD_BOTH("GadDragbar",     FV_TYPE_BOOLEAN, FA_Window_GadDragbar),
                F_ATTRIBUTES_ADD_BOTH("GadClose",       FV_TYPE_BOOLEAN, FA_Window_GadClose),
                F_ATTRIBUTES_ADD_BOTH("GadDepth",       FV_TYPE_BOOLEAN, FA_Window_GadDepth),
                F_ATTRIBUTES_ADD_BOTH("GadIconify",     FV_TYPE_BOOLEAN, FA_Window_GadIconify),
                F_ATTRIBUTES_ADD_BOTH("Decorator",      FV_TYPE_STRING,  FA_Window_Decorator),
                F_ATTRIBUTES_ADD_BOTH("Left",           FV_TYPE_STRING,  FA_Left),
                F_ATTRIBUTES_ADD_BOTH("Top",            FV_TYPE_STRING,  FA_Top),
                F_ATTRIBUTES_ADD_BOTH("Width",          FV_TYPE_STRING,  FA_Width),
                F_ATTRIBUTES_ADD_BOTH("Height",         FV_TYPE_STRING,  FA_Height),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Window_New,               FM_New),
                F_METHODS_ADD_STATIC(Window_Dispose,           FM_Dispose),
                F_METHODS_ADD_STATIC(Window_Get,               FM_Get),
                F_METHODS_ADD_STATIC(Window_Set,               FM_Set),
                F_METHODS_ADD_STATIC(Window_Import,            FM_Import),
                F_METHODS_ADD_STATIC(Window_Export,            FM_Export),
                F_METHODS_ADD_STATIC(Window_Connect,           FM_Connect),
                F_METHODS_ADD_STATIC(Window_Disconnect,        FM_Disconnect),
                F_METHODS_ADD_STATIC(Window_AddMember,         FM_AddMember),
                F_METHODS_ADD_STATIC(Window_RemMember,         FM_RemMember),

                F_METHODS_ADD_BOTH(Window_Erase,             "Erase",             FM_Erase),
                F_METHODS_ADD_BOTH(Window_Setup,             "Setup",             FM_Window_Setup),
                F_METHODS_ADD_BOTH(Window_Cleanup,           "Cleanup",           FM_Window_Cleanup),
                F_METHODS_ADD_BOTH(Window_Open,              "Open",              FM_Window_Open),
                F_METHODS_ADD_BOTH(Window_Close,             "Close",             FM_Window_Close),
                F_METHODS_ADD_BOTH(Window_Layout,            "Layout",            FM_Window_Layout),
                F_METHODS_ADD_BOTH(Window_Draw,              "Draw",              FM_Window_Draw),
                F_METHODS_ADD_BOTH(Window_HandleEvent,       "HandleEvent",       FM_Window_HandleEvent),
                F_METHODS_ADD_BOTH(Window_AddEventHandler,   "AddEventHandler",   FM_Window_AddEventHandler),
                F_METHODS_ADD_BOTH(Window_RemEventHandler,   "RemEventHandler",   FM_Window_RemEventHandler),
                F_METHODS_ADD_BOTH(Window_ChainAdd,          "ChainAdd",          FM_Window_ChainAdd),
                F_METHODS_ADD_BOTH(Window_ChainRem,          "ChainRem",          FM_Window_ChainRem),
                F_METHODS_ADD_BOTH(Window_Zoom,              "Zoom",              FM_Window_Zoom),
                F_METHODS_ADD_BOTH(Window_Depth,             "Depth",             FM_Window_Depth),
                F_METHODS_ADD_BOTH(Window_Help,              "Help",              FM_Window_Help),
                F_METHODS_ADD_BOTH(Window_RequestRethink,    "RequestRethink",    FM_Window_RequestRethink),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Frame),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_CATALOG,
                
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
};
