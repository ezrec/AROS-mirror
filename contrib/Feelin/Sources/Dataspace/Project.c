/*

$VER: 03.00 (2005/08/08)
 
   Portability update.
   
   Metaclass support.
 
$VER: 02.00 (2005/04/04)
 
   The parameters order of FM_Dataspace_Add has been changed.

*/

#include "Project.h"

struct Library                     *IFFParseBase;

///METHODS
F_METHOD_PROTO(void,DS_New);
F_METHOD_PROTO(void,DS_Dispose);
F_METHOD_PROTO(void,DS_Add);
F_METHOD_PROTO(void,DS_Remove);
F_METHOD_PROTO(void,DS_Find);
F_METHOD_PROTO(void,DS_Clear);
F_METHOD_PROTO(void,DS_WriteIFF);
F_METHOD_PROTO(void,DS_ReadIFF);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   if ((IFFParseBase = OpenLibrary("iffparse.library",36)))
   {
      return (FObject) F_SUPERDO();
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   if (IFFParseBase)
   {
      CloseLibrary(IFFParseBase); IFFParseBase = NULL;
   }

   F_SUPERDO();
}
//+

///QUERY
F_QUERY()
{
   switch (Which)
   {
///Meta
      case FV_Query_MetaClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Class_New,      FM_New),
            F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Class),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD("Pool",  FV_TYPE_POINTER),
             
            F_ARRAY_END
         };

         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD(DS_Add,       "Add"),
            F_METHODS_ADD(DS_Remove,    "Remove"),
            F_METHODS_ADD(DS_Find,      "Find"),
            F_METHODS_ADD(DS_Clear,     "Clear"),
            F_METHODS_ADD(DS_WriteIFF,  "WriteIFF"),
            F_METHODS_ADD(DS_ReadIFF,   "ReadIFF"),

            F_METHODS_ADD_STATIC(DS_New,       FM_New),
            F_METHODS_ADD_STATIC(DS_Dispose,   FM_Dispose),
             
            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
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
};
//+
