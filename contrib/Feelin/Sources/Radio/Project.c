/*

$VER: 01.02 (2005/08/10)
 
    Portability update
 
$VER: 01.00 ( 2004/12/03)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Radio_New);
F_METHOD_PROTO(void,Radio_Dispose);
F_METHOD_PROTO(void,Radio_Set);
F_METHOD_PROTO(void,Radio_Get);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(EntriesType) =
            {
                F_VALUES_ADD("Strings",FV_Radio_EntriesType_Strings),
                F_VALUES_ADD("Objects",FV_Radio_EntriesType_Objects),

               F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Active", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Entries", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("PreParse", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD_WITH_VALUES("EntriesType", FV_TYPE_INTEGER, EntriesType),

               F_ARRAY_END
            };
             
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Radio_New,     FM_New),
                F_METHODS_ADD_STATIC(Radio_Dispose, FM_Dispose),
                F_METHODS_ADD_STATIC(Radio_Set,     FM_Set),
                F_METHODS_ADD_STATIC(Radio_Get,     FM_Get),

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
//+
