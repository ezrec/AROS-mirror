/*

$VER: 01.02 (2005/08/10)
 
    Portability update
 
$VER: 01.00 (2005/05/11)

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,FontDialog_New);
F_METHOD_PROTO(void,FontDialog_Get);
F_METHOD_PROTO(void,FontDialog_Set);
F_METHOD_PROTO(void,FontDialog_Choosed);
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
                F_ATTRIBUTES_ADD("Spec", FV_TYPE_STRING),

                F_ARRAY_END
            };
             
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(FontDialog_Choosed, "Private1"),
                
                F_METHODS_ADD_STATIC(FontDialog_New, FM_New),
                F_METHODS_ADD_STATIC(FontDialog_Get, FM_Get),
                F_METHODS_ADD_STATIC(FontDialog_Set, FM_Set),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Dialog),
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
