/*

$VER: 09.00 (2005/08/09)
 
    Portability update.
   
    Removed a longstanding bug in FM_HandleEvent. The FEventHandler was not
    added the first time it was created, but only when modified.
   
    Metaclass support.
   
    Added FF_Hidden_Check and FF_Hidden_NoNesting to set FA_Hidden.
    
    Added FF_Disabled_Check and FF_Disabled_NoNesting to set FA_Disabled.
    
    A new technique is used to limit redraws to damaged objects. An  object
    is  damaged  when  at  least  one of its coordinate was modified or its
    visual state as been modified. A damaged  object  is  marked  with  the
    FF_Area_Damaged  flag. This flag is then used by Feelin to decied if an
    object should be redrawn or not. This new technique is used wisely  and
    particuliarly  when  rethinking  layout. The FM_RethinkLayout method is
    now deprecated.
 
$VER: 08.00 (2005/05/04)
   
    Remove some bugs in event handling for dragable objects.
   
    FM_DnDQuery returns FALSE if the object is disabled.
   
    Member Help removed from FS_BuildContextHelp
   
    The attribute FA_ImageDisplay_Origin is set to the FBox of the object.
   
    FA_Bufferize
   
    Improved  rendering.  Since  FC_Frame  clears   and   draw   everything
    necessary, only the area part is now cleared.
   
    FA_InLeft, FA_InRight, FA_InTop and FA_InBottom are no  longer  handled
    by  FC_Area.  Since  the erasing has been splited between FC_Frame (for
    the space between the border and the inner region) and FC_Area (special
    erasing).
   
    FA_Back is not longer handled by FC_Area but FC_Frame.
 
    Renamed FS_HandleEvent member FEv as Event
   
    Add the FF_Erase_Box flag to the FM_Erase method, to use a FBox instead
    of a FRect.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Area_New);
F_METHOD_PROTO(void,Area_Get);
F_METHOD_PROTO(void,Area_Set);
F_METHOD_PROTO(void,Area_Connect);
F_METHOD_PROTO(void,Area_Disconnect);
F_METHOD_PROTO(void,Area_Import);
F_METHOD_PROTO(void,Area_Export);
F_METHOD_PROTO(void,Area_Setup);
F_METHOD_PROTO(void,Area_Cleanup);
F_METHOD_PROTO(void,Area_Show);
F_METHOD_PROTO(void,Area_Hide);
F_METHOD_PROTO(void,Area_AskMinMax);
F_METHOD_PROTO(void,Area_Draw);
F_METHOD_PROTO(void,Area_Erase);
F_METHOD_PROTO(void,Area_GoActive);
F_METHOD_PROTO(void,Area_GoInactive);
F_METHOD_PROTO(void,Area_GoEnabled);
F_METHOD_PROTO(void,Area_GoDisabled);
F_METHOD_PROTO(void,Area_HandleEvent);
F_METHOD_PROTO(void,Area_ModifyHandler);
F_METHOD_PROTO(void,Area_WhichObject);
F_METHOD_PROTO(void,Area_BuildContextMenu);
F_METHOD_PROTO(void,Area_BuildContextHelp);
F_METHOD_PROTO(void,Area_DnDDo);
F_METHOD_PROTO(void,Area_DnDQuery);
F_METHOD_PROTO(void,Area_DnDBegin);
F_METHOD_PROTO(void,Area_DnDFinish);
F_METHOD_PROTO(void,Area_DnDReport);

#if F_CODE_DEPRECATED
F_METHOD_PROTO(void,Area_RethinkLayout);
#endif

F_METHOD_PROTO(void,Prefs_New);
//+

///DUMMY
F_METHOD(uint32,Area_Dummy)
{
   return 0;
}
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(SetMinMax) =
            {
                F_VALUES_ADD("None",     FV_SetNone),
                F_VALUES_ADD("Both",     FV_SetBoth),
                F_VALUES_ADD("Width",    FV_SetWidth),
                F_VALUES_ADD("Height",   FV_SetHeight),

                F_ARRAY_END
            };

            STATIC F_VALUES_ARRAY(InputMode) =
            {
                F_VALUES_ADD("None",        FV_InputMode_None),
                F_VALUES_ADD("Immediate",   FV_InputMode_Immediate),
                F_VALUES_ADD("Release",     FV_InputMode_Release),
                F_VALUES_ADD("Toggle",      FV_InputMode_Toggle),

                F_ARRAY_END
            };

            STATIC F_VALUES_ARRAY(Font) =
            {
                F_VALUES_ADD("Inherit",     FV_Font_Inherit),

                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_BOTH("PublicData", FV_TYPE_POINTER, FA_Area_PublicData),
                F_ATTRIBUTES_ADD_BOTH("Left", FV_TYPE_INTEGER, FA_Left),
                F_ATTRIBUTES_ADD_BOTH("Top", FV_TYPE_INTEGER, FA_Top),
                F_ATTRIBUTES_ADD_BOTH("Width", FV_TYPE_INTEGER, FA_Width),
                F_ATTRIBUTES_ADD_BOTH("Height", FV_TYPE_INTEGER, FA_Height),
                F_ATTRIBUTES_ADD_BOTH("Right", FV_TYPE_INTEGER, FA_Right),
                F_ATTRIBUTES_ADD_BOTH("Bottom", FV_TYPE_INTEGER, FA_Bottom),
                F_ATTRIBUTES_ADD_BOTH("MinWidth", FV_TYPE_INTEGER, FA_MinWidth),
                F_ATTRIBUTES_ADD_BOTH("MinHeight", FV_TYPE_INTEGER, FA_MinHeight),
                F_ATTRIBUTES_ADD_BOTH("MaxWidth", FV_TYPE_INTEGER, FA_MaxWidth),
                F_ATTRIBUTES_ADD_BOTH("MaxHeight", FV_TYPE_INTEGER, FA_MaxHeight),
                F_ATTRIBUTES_ADD_BOTH("FixWidth", FV_TYPE_INTEGER, FA_FixWidth),
                F_ATTRIBUTES_ADD_BOTH("FixHeight", FV_TYPE_INTEGER, FA_FixHeight),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("SetMin", FV_TYPE_INTEGER, FA_SetMin,        SetMinMax),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("SetMax", FV_TYPE_INTEGER, FA_SetMax,        SetMinMax),
                F_ATTRIBUTES_ADD_BOTH("Active", FV_TYPE_BOOLEAN, FA_Active),
                F_ATTRIBUTES_ADD_BOTH("Selected", FV_TYPE_BOOLEAN, FA_Selected),
                F_ATTRIBUTES_ADD_BOTH("Pressed", FV_TYPE_BOOLEAN, FA_Pressed),
                F_ATTRIBUTES_ADD_BOTH("Hidden", FV_TYPE_BOOLEAN, FA_Hidden),
                F_ATTRIBUTES_ADD_BOTH("Disabled", FV_TYPE_BOOLEAN, FA_Disabled),
                F_ATTRIBUTES_ADD_BOTH("Draggable", FV_TYPE_BOOLEAN, FA_Draggable),
                F_ATTRIBUTES_ADD_BOTH("Dropable", FV_TYPE_BOOLEAN, FA_Dropable),
                F_ATTRIBUTES_ADD_BOTH("Horizontal", FV_TYPE_BOOLEAN, FA_Horizontal),
                F_ATTRIBUTES_ADD_BOTH("Timer", FV_TYPE_INTEGER, FA_Timer),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("Font", FV_TYPE_STRING,  FA_Font,          Font),
                F_ATTRIBUTES_ADD_BOTH("Weight", FV_TYPE_INTEGER, FA_Weight),
                F_ATTRIBUTES_ADD_BOTH("NoFill", FV_TYPE_BOOLEAN, FA_NoFill),
                F_ATTRIBUTES_ADD_BOTH_WITH_VALUES("InputMode", FV_TYPE_INTEGER, FA_InputMode,     InputMode),
                F_ATTRIBUTES_ADD_BOTH("ControlChar", FV_TYPE_INTEGER, FA_ControlChar),
                F_ATTRIBUTES_ADD_BOTH("ChainToCycle", FV_TYPE_BOOLEAN, FA_ChainToCycle),
                F_ATTRIBUTES_ADD_BOTH("WindowObject", FV_TYPE_OBJECT,  FA_WindowObject),

                /* General attributes */

                F_ATTRIBUTES_ADD_BOTH("ColorScheme", FV_TYPE_STRING, FA_ColorScheme),
                F_ATTRIBUTES_ADD_BOTH("DisabledColorScheme", FV_TYPE_STRING, FA_DisabledColorScheme),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Area_New,           FM_New),
                F_METHODS_ADD_STATIC(Area_Get,           FM_Get),
                F_METHODS_ADD_STATIC(Area_Set,           FM_Set),
                F_METHODS_ADD_STATIC(Area_Connect,       FM_Connect),
                F_METHODS_ADD_STATIC(Area_Disconnect,    FM_Disconnect),
                F_METHODS_ADD_STATIC(Area_Import,        FM_Import),
                F_METHODS_ADD_STATIC(Area_Export,        FM_Export),

                F_METHODS_ADD_BOTH(Area_Setup,            "Setup",             FM_Setup),
                F_METHODS_ADD_BOTH(Area_Cleanup,          "Cleanup",           FM_Cleanup),
                F_METHODS_ADD_BOTH(Area_Show,             "Show",              FM_Show),
                F_METHODS_ADD_BOTH(Area_Hide,             "Hide",              FM_Hide),
                F_METHODS_ADD_BOTH(Area_AskMinMax,        "AskMinMax",         FM_AskMinMax),
                F_METHODS_ADD_BOTH(Area_Dummy,            "Layout",            FM_Layout),
                
                #if F_CODE_DEPRECATED
                F_METHODS_ADD_BOTH(Area_RethinkLayout,    "RethinkLayout",     FM_RethinkLayout),
                #endif
                 
                F_METHODS_ADD_BOTH(Area_Draw,             "Draw",              FM_Draw),
                F_METHODS_ADD_BOTH(Area_Erase,            "Erase",             FM_Erase),
                F_METHODS_ADD_BOTH(Area_GoActive,         "GoActive",          FM_GoActive),
                F_METHODS_ADD_BOTH(Area_GoInactive,       "GoInactive",        FM_GoInactive),
                F_METHODS_ADD_BOTH(Area_GoEnabled,        "GoEnabled",         FM_GoEnabled),
                F_METHODS_ADD_BOTH(Area_GoDisabled,       "GoDisabled",        FM_GoDisabled),
                F_METHODS_ADD_BOTH(Area_HandleEvent,      "HandleEvent",       FM_HandleEvent),
                F_METHODS_ADD_BOTH(Area_ModifyHandler,    "ModifyHandler",     FM_ModifyHandler),
                F_METHODS_ADD_BOTH(Area_BuildContextMenu, "BuildContextMenu",  FM_BuildContextMenu),
                F_METHODS_ADD_BOTH(Area_BuildContextHelp, "BuildContextHelp",  FM_BuildContextHelp),
                F_METHODS_ADD_BOTH(Area_DnDDo,            "DnDDo",             FM_DnDDo),
                F_METHODS_ADD_BOTH(Area_DnDQuery,         "DnDQuery",          FM_DnDQuery),
                F_METHODS_ADD_BOTH(Area_DnDBegin,         "DnDBegin",          FM_DnDBegin),
                F_METHODS_ADD_BOTH(Area_DnDFinish,        "DnDFinish",         FM_DnDFinish),
                F_METHODS_ADD_BOTH(Area_DnDReport,        "DnDReport",         FM_DnDReport),
                F_METHODS_ADD_BOTH(Area_Dummy,            "DnDDrop",           FM_DnDDrop),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Frame),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

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
