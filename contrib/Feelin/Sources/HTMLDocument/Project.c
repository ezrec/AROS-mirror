/*

$VER: 01.00 (2005/08/10)
 
    Portability update
 
$VER: 01.00 (2005/05/01)
 
    This new class should be used to parse HTML documents. It  is  based  on
    FC_XMLApplication  code  and  behaviour, both class are very similar and
    work in the same ways.
 
*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,HTMLDocument_Get);
F_METHOD_PROTO(void,HTMLDocument_Parse);
F_METHOD_PROTO(void,HTMLDocument_Clear);
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
                F_ATTRIBUTES_ADD("Nodes", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Attributes", FV_TYPE_POINTER),

                F_ARRAY_END
            };
                            
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD       (HTMLDocument_Parse,    "FM_Document_Parse"),
                F_METHODS_ADD       (HTMLDocument_Clear,    "FM_Document_Clear"),
                F_METHODS_ADD_STATIC(HTMLDocument_Get,       FM_Get),

                F_ARRAY_END
            };

            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FM_Document_Read"),
                F_RESOLVES_ADD("FM_Document_Resolve"),
                F_RESOLVES_ADD("FM_Document_ObtainName"),
                F_RESOLVES_ADD("FM_Document_Log"),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Document),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_RESOLVES,
                
                F_ARRAY_END
            };
 
            return F_TAGS;
        }
//+
    }
    return NULL;
}
