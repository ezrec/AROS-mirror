/*

$VER: 03.00 (2005/08/10)
 
   Portability update.
   
   FM_Adjust_Query support.

$VER: 02.00 (2005/03/31)
 
   The class is now a subclass of FC_Adjust
    
*/

#include "Project.h"
#include "_locale/table.h"

///METHODS
F_METHOD_PROTO(void,Adjust_Query);
F_METHOD_PROTO(void,Adjust_ToString);
F_METHOD_PROTO(void,Adjust_ToObject);

F_METHOD_PROTO(void,Adjust_New);
F_METHOD_PROTO(void,Adjust_DnDQuery);
F_METHOD_PROTO(void,Adjust_DnDDrop);
//+

F_QUERY()
{
   switch (Which)
   {
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD        (Adjust_Query,    "FM_Adjust_Query"),
            F_METHODS_ADD        (Adjust_ToString, "FM_Adjust_ToString"),
            F_METHODS_ADD        (Adjust_ToObject, "FM_Adjust_ToObject"),
            F_METHODS_ADD_STATIC (Adjust_New,       FM_New),
            F_METHODS_ADD_STATIC (Adjust_DnDQuery,  FM_DnDQuery),
            F_METHODS_ADD_STATIC (Adjust_DnDDrop,   FM_DnDDrop),

            F_ARRAY_END
         };
         
         STATIC F_RESOLVES_ARRAY =
         {
            F_RESOLVES_ADD("FM_Adjust_ParseXML"),
            F_RESOLVES_ADD("FA_Adjust_Preview"),
            F_RESOLVES_ADD("FA_Adjust_Spec"),
            
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
   }
   return NULL;
}

