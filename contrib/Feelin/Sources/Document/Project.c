/*

$VER: 02.02 (2005/08/22)

    DuLockFromFH()   doesn't   work    with    WinUAE    vitual    volumes,
    FM_Document_Read has been modified and use Lock() instead.

$VER: 02.00 (2005/08/10)
 
    Portability update.
    
    FM_Adjust_Resolve gives the kind of data resolved instead of TRUE.
 
$VER: 01.00 (2005/05/01)
 
    This is the base class of document classes.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Document_New);
F_METHOD_PROTO(void,Document_Dispose);
F_METHOD_PROTO(void,Document_Get);
F_METHOD_PROTO(void,Document_Set);
F_METHOD_PROTO(void,Document_Read);
F_METHOD_PROTO(void,Document_Parse);
F_METHOD_PROTO(void,Document_AddIDs);
F_METHOD_PROTO(void,Document_Resolve);
F_METHOD_PROTO(void,Document_FindName);
F_METHOD_PROTO(void,Document_ObtainName);
F_METHOD_PROTO(void,Document_Log);
F_METHOD_PROTO(void,Document_Clear);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_VALUES_ARRAY(SourceType) =
            {
                F_VALUES_ADD("File", FV_Document_SourceType_File),
                F_VALUES_ADD("Memory", FV_Document_SourceType_Memory),
             
                F_ARRAY_END
            };

            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Pool", FV_TYPE_POINTER),
                F_ATTRIBUTES_ADD("Source", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD_WITH_VALUES("SourceType", FV_TYPE_INTEGER, SourceType),
                F_ATTRIBUTES_ADD("Version", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Revision", FV_TYPE_INTEGER),

                F_ARRAY_END
            };
 
            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD       (Document_Read,         "Read"),
                F_METHODS_ADD       (Document_Parse,        "Parse"),
                F_METHODS_ADD       (Document_AddIDs,       "AddIDs"),
                F_METHODS_ADD       (Document_Resolve,      "Resolve"),
                F_METHODS_ADD       (Document_FindName,     "FindName"),
                F_METHODS_ADD       (Document_ObtainName,   "ObtainName"),
                F_METHODS_ADD       (Document_Log,          "Log"),
                F_METHODS_ADD       (Document_Clear,        "Clear"),
                F_METHODS_ADD_STATIC(Document_New,           FM_New),
                F_METHODS_ADD_STATIC(Document_Dispose,       FM_Dispose),
                F_METHODS_ADD_STATIC(Document_Get,           FM_Get),
                F_METHODS_ADD_STATIC(Document_Set,           FM_Set),

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
