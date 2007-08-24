/*

$VER: 02.00 (2005/08/14)
 
    Portability update.
   
    Gradient specification is now defined in XML format.
   
    Graidents can now be built from 2, 3 or 4 colors (e.g. aqua)
 
$VER: 01.00 (2005/04/05)
  
*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Adjust_New);
F_METHOD_PROTO(void,Adjust_Query);
F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Adjust_Query, "FM_Adjust_Query"),
                F_METHODS_ADD(Adjust_ToString, "FM_Adjust_ToString"),
                F_METHODS_ADD(Adjust_ToObject, "FM_Adjust_ToObject"),
                F_METHODS_ADD_STATIC(Adjust_New, FM_New),
               
                F_ARRAY_END
            };

            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FM_Adjust_ParseXML"),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Adjust),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_RESOLVES,
                F_TAGS_ADD_CATALOG,
             
                F_ARRAY_END
            };
    
            return F_TAGS;
        }
//+
    }
    return NULL;
}

