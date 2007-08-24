/*

$VER: 02.00 (2005/08/10)
 
   Portability update.
   
   Added the FM_Preview_Query method.
 
$VER: 01.00 (2005/04/06)
 
   Base class for preview objects

*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Preview_New);
F_METHOD_PROTO(void,Preview_Dispose);
F_METHOD_PROTO(void,Preview_Set);
F_METHOD_PROTO(void,Preview_Get);
F_METHOD_PROTO(void,Preview_Import);
F_METHOD_PROTO(void,Preview_Export);
F_METHOD_PROTO(void,Preview_Setup);
F_METHOD_PROTO(void,Preview_Cleanup);
F_METHOD_PROTO(void,Preview_Draw);
F_METHOD_PROTO(void,Preview_HandleEvent);
F_METHOD_PROTO(void,Preview_DnDQuery);
F_METHOD_PROTO(void,Preview_DnDDrop);

F_METHOD_PROTO(void,Preview_Query);
F_METHOD_PROTO(void,Preview_ParseXML);
F_METHOD_PROTO(void,Preview_ToString);
F_METHOD_PROTO(void,Preview_Adjust);
F_METHOD_PROTO(void,Preview_Update);

F_METHOD_PROTO(void,Prefs_New);
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
               F_ATTRIBUTES_ADD("Spec",   FV_TYPE_STRING),
               F_ATTRIBUTES_ADD("Alive",  FV_TYPE_BOOLEAN),
               F_ATTRIBUTES_ADD("Couple", FV_TYPE_BOOLEAN),

               F_ARRAY_END
            };
                                          
            STATIC F_METHODS_ARRAY =
            {
               F_METHODS_ADD        (Preview_Query,      "Query"),
               F_METHODS_ADD        (Preview_ParseXML,   "ParseXML"),
               F_METHODS_ADD        (Preview_ToString,   "ToString"),
               F_METHODS_ADD        (Preview_Adjust,     "Adjust"),
               F_METHODS_ADD        (Preview_Update,     "Private1"),
               F_METHODS_ADD_STATIC (Preview_New,         FM_New),
               F_METHODS_ADD_STATIC (Preview_Dispose,     FM_Dispose),
               F_METHODS_ADD_STATIC (Preview_Set,         FM_Set),
               F_METHODS_ADD_STATIC (Preview_Get,         FM_Get),
               F_METHODS_ADD_STATIC (Preview_Import,      FM_Import),
               F_METHODS_ADD_STATIC (Preview_Export,      FM_Export),
               F_METHODS_ADD_STATIC (Preview_Setup,       FM_Setup),
               F_METHODS_ADD_STATIC (Preview_Cleanup,     FM_Cleanup),
               F_METHODS_ADD_STATIC (Preview_Draw,        FM_Draw),
               F_METHODS_ADD_STATIC (Preview_HandleEvent, FM_HandleEvent),
               F_METHODS_ADD_STATIC (Preview_DnDQuery,    FM_DnDQuery),
               F_METHODS_ADD_STATIC (Preview_DnDDrop,     FM_DnDDrop),

               F_ARRAY_END
            };
       
            STATIC F_AUTOS_ARRAY =
            {
               F_AUTOS_ADD("FA_Document_Source"),
               F_AUTOS_ADD("FA_Document_SourceType"),
               F_AUTOS_ADD("FM_Document_AddIDs"),
               F_AUTOS_ADD("FM_Document_Resolve"),
               F_AUTOS_ADD("FA_XMLDocument_Markups"),

               F_ARRAY_END
            };
            STATIC F_TAGS_ARRAY =
            {
               F_TAGS_ADD_SUPER(Area),
               F_TAGS_ADD_LOD,
               F_TAGS_ADD_METHODS,
               F_TAGS_ADD_ATTRIBUTES,
               F_TAGS_ADD_AUTOS,
               F_TAGS_ADD_CATALOG,

               F_ARRAY_END
            };
    
            return F_TAGS;
        }
//+
///Prefs
        case FV_Query_PrefsTags:
        {
            STATIC F_METHODS_ARRAY =
            {
               F_METHODS_ADD_STATIC(Prefs_New, FM_New),

               F_ARRAY_END
            };
               
            STATIC F_TAGS_ARRAY =
            {
               F_TAGS_ADD_SUPER(PreferenceGroup),
               F_TAGS_ADD_METHODS,

               F_ARRAY_END
            };
    
            return F_TAGS;
        }
//+
    }
    return NULL;
}

