/*

$VER: 03.02 (2005/08/10)
 
    Portability update
 
$VER: 03.00 (2004/09/06)

    PopHelp is now handled by the shared object AppServer.  PopHelp  doesn't
    handle events on its own. It's now a 'stupid' class.

*/ 
 
#include "Project.h"
 
///METHOD 
F_METHOD_PROTO(void,PH_New);
F_METHOD_PROTO(void,PH_Dispose);
F_METHOD_PROTO(void,PH_Get);
F_METHOD_PROTO(void,PH_Set);

//F_METHOD(void,Prefs_New);
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
                F_ATTRIBUTES_ADD("Text",     FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Font",     FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Open",     FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Window",   FV_TYPE_OBJECT),
                F_ATTRIBUTES_ADD("Scheme",   FV_TYPE_STRING),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(PH_New,       FM_New),
                F_METHODS_ADD_STATIC(PH_Dispose,   FM_Dispose),
                F_METHODS_ADD_STATIC(PH_Get,       FM_Get),
                F_METHODS_ADD_STATIC(PH_Set,       FM_Set),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Object),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
/*
        case FV_Query_PrefsTags:
        {
            F_METHODS_INIT
            F_METHODS_ADD_STATIC(Prefs_New, FM_New)
            F_METHODS_DONE
            
            F_TAGS_INIT
            F_TAGS_ADD(Super,    FC_PreferenceGroup)
            F_TAGS_ADD(Methods,  F_METHODS)
            F_TAGS_DONE
             
            
            return F_TAGS;
        }
*/
    }
    return NULL;
}; 
//+ 
