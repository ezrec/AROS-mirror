/*

$VER: 05.02 (2005/08/10)
 
    Portability update.
    
    Added FM_GoEnabled and GM_GoDisabled support.
 
$VER: 05.00 (2005/04/29)
 
    FM_Prop_Decrease and FM_Prop_Decrease  no  longer  modify  FA_Prop_First
    uselessly.
    
    The attribute FA_Prop_Useless is set to TRUE when there is no  entry  or
    when  all  entries  are  visible; and is set to FALSE when there is some
    entries, which are no all visible.
    
    The event class code FV_EVENT_BUTTON_WHEEL is now handled by the class.
 
$VER: 04.00 (2005/02/01)

    Added color scheme support.
    Added new events support.
 
$VER: 03.00 (2003/09/22)

    Added preference editor support

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Prop_New);
F_METHOD_PROTO(void,Prop_Dispose);
F_METHOD_PROTO(void,Prop_Get);
F_METHOD_PROTO(void,Prop_Set);
F_METHOD_PROTO(void,Prop_Setup);
F_METHOD_PROTO(void,Prop_Cleanup);
F_METHOD_PROTO(void,Prop_Show);
F_METHOD_PROTO(void,Prop_Hide);
F_METHOD_PROTO(void,Prop_AskMinMax);
F_METHOD_PROTO(void,Prop_Layout);
F_METHOD_PROTO(void,Prop_Draw);
F_METHOD_PROTO(void,Prop_HandleEvent);
F_METHOD_PROTO(void,Prop_GoActive);
F_METHOD_PROTO(void,Prop_GoInactive);
F_METHOD_PROTO(void,Prop_GoEnabled);
F_METHOD_PROTO(void,Prop_GoDisabled);
F_METHOD_PROTO(void,Prop_Decrease);
F_METHOD_PROTO(void,Prop_Increase);
F_METHOD_PROTO(void,Prefs_New);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Entries",  FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Visible",  FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("First",    FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Step",     FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Knob",     FV_TYPE_OBJECT),
                F_ATTRIBUTES_ADD("Useless",  FV_TYPE_BOOLEAN),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Prop_Decrease,      "Decrease"),
                F_METHODS_ADD(Prop_Increase,      "Increase"),

                F_METHODS_ADD_STATIC(Prop_New,          FM_New),
                F_METHODS_ADD_STATIC(Prop_Dispose,      FM_Dispose),
                F_METHODS_ADD_STATIC(Prop_Get,          FM_Get),
                F_METHODS_ADD_STATIC(Prop_Set,          FM_Set),
                F_METHODS_ADD_STATIC(Prop_Setup,        FM_Setup),
                F_METHODS_ADD_STATIC(Prop_Cleanup,      FM_Cleanup),
                F_METHODS_ADD_STATIC(Prop_Show,         FM_Show),
                F_METHODS_ADD_STATIC(Prop_Hide,         FM_Hide),
                F_METHODS_ADD_STATIC(Prop_AskMinMax,    FM_AskMinMax),
                F_METHODS_ADD_STATIC(Prop_Layout,       FM_Layout),
                F_METHODS_ADD_STATIC(Prop_Draw,         FM_Draw),
                F_METHODS_ADD_STATIC(Prop_GoActive,     FM_GoActive),
                F_METHODS_ADD_STATIC(Prop_GoInactive,   FM_GoInactive),
                F_METHODS_ADD_STATIC(Prop_GoEnabled,    FM_GoEnabled),
                F_METHODS_ADD_STATIC(Prop_GoDisabled,   FM_GoDisabled),
                F_METHODS_ADD_STATIC(Prop_HandleEvent,  FM_HandleEvent),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
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
}

