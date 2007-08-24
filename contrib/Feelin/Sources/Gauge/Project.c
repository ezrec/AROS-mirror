/*

$VER: 04.02 (2005/08/10)
 
    Portability update

$VER: 04.00 (2005/04/06)
 
    Uses an XMLObject for preferences GUI.
    
    Added $gauge-back-vertical preference item, which should be used for
    background of vertical objects. Default is $gauge-back.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Gauge_New);
F_METHOD_PROTO(void,Gauge_Set);
F_METHOD_PROTO(void,Gauge_Get);
F_METHOD_PROTO(void,Gauge_AskMinMax);
F_METHOD_PROTO(void,Gauge_Draw);

/*** Preferences ***********************************************************/

F_METHOD_PROTO(void,Prefs_New);
F_METHOD_PROTO(void,Prefs_Show);
F_METHOD_PROTO(void,Prefs_Hide);
F_METHOD_PROTO(void,Prefs_Update);
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
                F_ATTRIBUTES_ADD("Simple", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Info", FV_TYPE_STRING),
                
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Gauge_New,FM_New),
                F_METHODS_ADD_STATIC(Gauge_Get,FM_Get),
                F_METHODS_ADD_STATIC(Gauge_Set,FM_Set),
                F_METHODS_ADD_STATIC(Gauge_AskMinMax,FM_AskMinMax),
                F_METHODS_ADD_STATIC(Gauge_Draw,FM_Draw),
                
                F_ARRAY_END
            };
             
            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FA_Numeric_Value"),
                F_RESOLVES_ADD("FA_Numeric_Min"),
                F_RESOLVES_ADD("FA_Numeric_Max"),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Numeric),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_RESOLVES,
                
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
                F_METHODS_ADD_STATIC(Prefs_New,FM_New),
                F_METHODS_ADD_STATIC(Prefs_Show,FM_Show),
                F_METHODS_ADD_STATIC(Prefs_Hide,FM_Hide),
                F_METHODS_ADD_STATIC(Prefs_Update,FM_Gauge_Update),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(PreferenceGroup),
                F_TAGS_ADD(LODSize,  sizeof (struct p_LocalObjectData)),
                F_TAGS_ADD_METHODS,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}

