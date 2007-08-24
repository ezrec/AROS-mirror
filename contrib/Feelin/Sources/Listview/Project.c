/*

$VER: 01.02 (2005/08/10)
 
    Portability update
 
$VER: 01.00 (2004/12/18)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,LV_New);
F_METHOD_PROTO(void,LV_GoActive);
F_METHOD_PROTO(void,LV_GoInactive);
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
                F_ATTRIBUTES_ADD("List", FV_TYPE_OBJECT),
            
                F_ARRAY_END
            };
            
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(LV_New,          FM_New),
                F_METHODS_ADD_STATIC(LV_GoActive,     FM_GoActive),
                F_METHODS_ADD_STATIC(LV_GoInactive,   FM_GoInactive),
            
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
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
