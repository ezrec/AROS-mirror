/*

$VER: 01.00 (2005/08/10)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Palette_Best);
F_METHOD_PROTO(void,Palette_ObtainPens);

F_METHOD_PROTO(void,Palette_New);
F_METHOD_PROTO(void,Palette_Dispose);
F_METHOD_PROTO(void,Palette_Get);
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
                F_ATTRIBUTES_ADD("Colors", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("NumColors", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("MaxColors", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Histogram", FV_TYPE_OBJECT),
                F_ATTRIBUTES_ADD("Resolution", FV_TYPE_INTEGER),
                
                F_ARRAY_END
            };
                
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Palette_Best, "Best"),
                F_METHODS_ADD(Palette_ObtainPens, "ObtainPens"),
                
                F_METHODS_ADD_STATIC(Palette_New, FM_New),
                F_METHODS_ADD_STATIC(Palette_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(Palette_Get, FM_Get),
 
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
