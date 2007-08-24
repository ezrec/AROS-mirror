/*

$VER: 02.00 (2005/08/10)
 
   Portability update.
   
   Support FM_Document_Resolve new types.
   
   If FA_XMLObject_References was NULL, objects generated with a name  were
   causing a hit.
 
$VER: 01.00 (2005/04/30)

   This class is used to generate  one  or  several  objects,  as  well  as
   objects  trees.  This  class  is  the  link  between  FC_XMLDocument and
   FC_XMLApplication.

   Rename   FM_XMLObject_Substitute    as    FM_XMLObject_Resolve    (since
   FML_XMLDocument_Resolve as been renamed as FM_XMLDocument_AddIDs).
   
   Since  FC_XMLDocument  implements  the  FM_XMLDocument_Resolve   method,
   FM_XMLObject_Resolve  method  is  used to add object and name resolving,
   the rest (boolean, integers, strings) is left to FC_XMLDocument.
 
*/

#include "Project.h"

///METHODS
F_METHOD(void,XMLObject_New);
F_METHOD(void,XMLObject_Get);
F_METHOD(void,XMLObject_Read);
F_METHOD(void,XMLObject_Find);
F_METHOD(void,XMLObject_Build);
F_METHOD(void,XMLObject_Create);
F_METHOD(void,XMLObject_Resolve);
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
                F_ATTRIBUTES_ADD("Definitions", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("References", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Tags", FV_TYPE_POINTER),

                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(XMLObject_Read, "FM_Document_Read"),
                F_METHODS_ADD(XMLObject_Resolve, "FM_Document_Resolve"),
     
                F_METHODS_ADD(XMLObject_Find, "Find"),
                F_METHODS_ADD(XMLObject_Build, "Build"),
                F_METHODS_ADD(XMLObject_Create, "Create"),

                F_METHODS_ADD_STATIC(XMLObject_New, FM_New),
                F_METHODS_ADD_STATIC(XMLObject_Get, FM_Get),

                F_ARRAY_END
            };

            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FM_Document_AddIDs"),
                F_RESOLVES_ADD("FM_Document_Log"),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(XMLDocument),
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
