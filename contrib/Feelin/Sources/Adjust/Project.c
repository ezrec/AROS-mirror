/*

$VER: 02.00 (2005/08/08)
 
    Protabilty update.
   
    Added the  FM_Adjust_Query  method,  which  *must*  be  implemented  by
    subclass.  This  new method is used to ask a class if a spec is handled
    and valid. This method is now invoked *before*  the  FM_Adjust_ToString
    method,  if  the  result  is  FALSE,  the  FM_Adjust_ToString method is
    invoked with NULL as spec. The method is also used during  FM_DnDQuery,
    thus subclasses don't need to implement their own FM_DnDQuery method.
   
    Added the FM_Adjust_ParseXML method.
   
    Added 'XMLDocument' member in FS_Adjust_ToObject method message.
   
    The FA_Adjust_Preview attribute can now be got and give the address  of
    the  preview  object,  which may be NULL if not preview object has been
    created.
   
    Use the new FF_Disabled_Check flag.
 
$VER: 01.00 (2005/04/06)

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Adjust_New);
F_METHOD_PROTO(void,Adjust_Dispose);
F_METHOD_PROTO(void,Adjust_Get);
F_METHOD_PROTO(void,Adjust_Set);
F_METHOD_PROTO(void,Adjust_DnDQuery);
F_METHOD_PROTO(void,Adjust_DnDDrop);
F_METHOD_PROTO(void,Adjust_DnDDiffer);

F_METHOD_PROTO(void,Adjust_Query);
F_METHOD_PROTO(void,Adjust_ParseXML);
F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);
F_METHOD_PROTO(void,Adjust_TogglePreview);
//+

F_QUERY()
{
    switch (Which)
    {
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD("Spec", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("Couple", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("Separator", FV_TYPE_INTEGER),
                F_ATTRIBUTES_ADD("Preview", FV_TYPE_BOOLEAN),
                F_ATTRIBUTES_ADD("PreviewTitle", FV_TYPE_STRING),
                F_ATTRIBUTES_ADD("PreviewClass", FV_TYPE_STRING),
                
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD(Adjust_Query,           "Query"),
                F_METHODS_ADD(Adjust_ParseXML,        "ParseXML"),
                F_METHODS_ADD(Adjust_ToString,        "ToString"),
                F_METHODS_ADD(Adjust_ToObject,        "ToObject"),
                F_METHODS_ADD(Adjust_TogglePreview,   "Private1"),

                F_METHODS_ADD_STATIC(Adjust_New,       FM_New),
                F_METHODS_ADD_STATIC(Adjust_Dispose,   FM_Dispose),
                F_METHODS_ADD_STATIC(Adjust_Get,       FM_Get),
                F_METHODS_ADD_STATIC(Adjust_Set,       FM_Set),

                F_METHODS_ADD_STATIC(Adjust_DnDQuery,  FM_DnDQuery),
                F_METHODS_ADD_STATIC(Adjust_DnDDiffer, FM_DnDBegin),
                F_METHODS_ADD_STATIC(Adjust_DnDDiffer, FM_DnDReport),
                F_METHODS_ADD_STATIC(Adjust_DnDDrop,   FM_DnDDrop),
                F_METHODS_ADD_STATIC(Adjust_DnDDiffer, FM_DnDFinish),
                
                F_ARRAY_END
            };

            STATIC F_AUTOS_ARRAY =
            {
                F_AUTOS_ADD("FA_Preview_Spec"),
                 
                F_AUTOS_ADD("FA_Document_Source"),
                F_AUTOS_ADD("FA_Document_SourceType"),
                F_AUTOS_ADD("FM_Document_AddIDs"),
                F_AUTOS_ADD("FM_Document_Resolve"),
                
                F_AUTOS_ADD("FA_XMLDocument_Markups"),

                F_ARRAY_END
            };
             
            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Group),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                F_TAGS_ADD_AUTOS,
                F_TAGS_ADD_CATALOG,
                
                F_ARRAY_END
            };
    
            return F_TAGS;
        }
    }
    return NULL;
}

