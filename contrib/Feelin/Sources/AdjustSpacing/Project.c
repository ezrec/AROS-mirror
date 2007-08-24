/*

$VER: 01.10 (2005/08/10)
 
    Portability update
    
$VER: 01.00 (2004/10/05)
 
*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,AdjustSpacing_New);
F_METHOD_PROTO(void,AdjustSpacing_Get);
F_METHOD_PROTO(void,AdjustSpacing_Set);
//+

///QUERY
F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Spec", FV_TYPE_STRING),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(AdjustSpacing_New,  FM_New),
                F_METHODS_ADD_STATIC(AdjustSpacing_Set,  FM_Set),
                F_METHODS_ADD_STATIC(AdjustSpacing_Get,  FM_Get),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_CATALOG,

                F_ARRAY_END
            };

            return F_TAGS;
        }
    }
    return NULL;
}
//+
