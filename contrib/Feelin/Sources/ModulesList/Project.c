/*

$VER: 01.00 (2005/08/12)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void, ML_New);
F_METHOD_PROTO(void, ML_Dispose);
F_METHOD_PROTO(void, ML_Get);
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
                F_ATTRIBUTES_ADD("Path", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Fast", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Classes", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Prefs", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("DecoratorPrefs", FV_TYPE_POINTER),
                
                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(ML_New, FM_New),
                F_METHODS_ADD_STATIC(ML_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(ML_Get, FM_Get),
                
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
    }
    return NULL;
}
