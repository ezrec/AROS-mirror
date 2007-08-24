/*

$VER: 02.00 (2005/08/03)
 
    Portability update.
    
    String contents modification by the user did not trigger  notification.
    fixed.
 
$VER: 01.00 (2005/04/29)

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,PopFile_New);
F_METHOD_PROTO(void,PopFile_Dispose);
F_METHOD_PROTO(void,PopFile_Get);
F_METHOD_PROTO(void,PopFile_Set);
F_METHOD_PROTO(void,PopFile_DnDQuery);
F_METHOD_PROTO(void,PopFile_DnDDrop);
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
                F_METHODS_ADD_STATIC(PopFile_New,        FM_New),
                F_METHODS_ADD_STATIC(PopFile_Dispose,    FM_New),
                F_METHODS_ADD_STATIC(PopFile_Get,        FM_Get),
                F_METHODS_ADD_STATIC(PopFile_Set,        FM_Set),
                F_METHODS_ADD_STATIC(PopFile_DnDQuery,   FM_DnDQuery),
                F_METHODS_ADD_STATIC(PopFile_DnDDrop,    FM_DnDDrop),
                
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
