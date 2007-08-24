/*

$VER: 01.02 (2005/08/10)
 
    Portability update
 
$VER: 01.00 (2004/11/16)

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,PopFont_New);
F_METHOD_PROTO(void,PopFont_Get);
F_METHOD_PROTO(void,PopFont_Set);
F_METHOD_PROTO(void,PopFont_DnDQuery);
F_METHOD_PROTO(void,PopFont_DnDDrop);
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
                F_ATTRIBUTES_ADD("Contents", FV_TYPE_STRING),

                F_ARRAY_END
            };
             
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(PopFont_New,        FM_New),
                F_METHODS_ADD_STATIC(PopFont_Get,        FM_Get),
                F_METHODS_ADD_STATIC(PopFont_Set,        FM_Set),
                F_METHODS_ADD_STATIC(PopFont_DnDQuery,   FM_DnDQuery),
                F_METHODS_ADD_STATIC(PopFont_DnDDrop,    FM_DnDDrop),

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
//+
    }
    return NULL;
}
