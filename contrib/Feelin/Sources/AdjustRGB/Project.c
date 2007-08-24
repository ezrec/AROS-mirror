/*

$VER: 02.00 (2005/08/10)
 
    Portability update.
    
    Added FC_String objects to directly edit RGB and HSV values. Also  added
    a  string to edit HTML (or hexadecimal) RGB values, and a button to pick
    color directly from the display.
 
$VER: 01.00 (2005/04/06)

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Adjust_New);
F_METHOD_PROTO(void,Adjust_HandleEvent);

F_METHOD_PROTO(void,Adjust_Query);
F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);

F_METHOD_PROTO(void,Adjust_PickBegin);
F_METHOD_PROTO(void,Adjust_PickEnd);
//+

F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD        (Adjust_Query,      "FM_Adjust_Query"),
                F_METHODS_ADD        (Adjust_ToString,   "FM_Adjust_ToString"),
                F_METHODS_ADD        (Adjust_ToObject,   "FM_Adjust_ToObject"),
                F_METHODS_ADD        (Adjust_PickBegin,  "Private1"),
                F_METHODS_ADD        (Adjust_PickEnd,    "Private2"),
                F_METHODS_ADD_STATIC (Adjust_New,         FM_New),
                F_METHODS_ADD_STATIC (Adjust_HandleEvent, FM_HandleEvent),

                F_ARRAY_END
            };
            
            STATIC F_AUTOS_ARRAY =
            {
                F_AUTOS_ADD("FA_Numeric_Value"),
                F_AUTOS_ADD("FA_String_Contents"),
                F_AUTOS_ADD("FA_String_Integer"),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Adjust),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_AUTOS,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_CATALOG,

                F_ARRAY_END
            };
            
            return F_TAGS;
        }
    }
    return NULL;
}

