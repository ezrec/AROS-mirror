/*

$VER: 02.10 (2005/08/10)
 
    Portability update

$VER: 02.00 (2005/03/15)

    Subclass of FC_XMLObject

    Custom classes can now be used within XML files. All you have to  do  is
    define  your  custom  class in FXMLDefinition, then use the same name in
    the XML file.
 
$VER: 01.10 (2005/02/02)
 
    Improved object collection management. F_MakeObj() is now used  as  much
    as possible.
 
*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,XMLApplication_New);
F_METHOD_PROTO(void,XMLApplication_Dispose);
F_METHOD_PROTO(void,XMLApplication_Build);
F_METHOD_PROTO(void,XMLApplication_CreateNotify);
F_METHOD_PROTO(void,XMLApplication_Run);

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
                F_METHODS_ADD(XMLApplication_CreateNotify, "CreateNotify"),
                F_METHODS_ADD(XMLApplication_Run, "Run"),
                F_METHODS_ADD(XMLApplication_Build, "FM_XMLObject_Build"),

                F_METHODS_ADD_STATIC(XMLApplication_New, FM_New),
                F_METHODS_ADD_STATIC(XMLApplication_Dispose, FM_Dispose),

                F_ARRAY_END
            };

            STATIC F_RESOLVES_ARRAY =
            {
                F_RESOLVES_ADD("FM_Document_Resolve"),
                F_RESOLVES_ADD("FM_Document_Log"),
                F_RESOLVES_ADD("FM_XMLObject_Find"),

                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(XMLObject),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_RESOLVES,

                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
