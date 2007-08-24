/*

$VER: 04.00 (2005/08/10)
 
    Portability update.
   
    Metaclass support.
   
    Some rendering methods introduced.
   
    FM_Render_Fill

$VER: 03.01 (2005/01/11)

   Removed useless private memory pool, traces are now allocated  from  the
   default memory pool.

*/

#include "Project.h"

struct Library                     *CyberGfxBase;
struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,Render_New);
F_METHOD_PROTO(void,Render_Get);
F_METHOD_PROTO(void,Render_Set);
F_METHOD_PROTO(void,Render_AddClip);
F_METHOD_PROTO(void,Render_RemClip);
F_METHOD_PROTO(void,Render_AddClipRegion);
F_METHOD_PROTO(void,Render_CreateBuffer);
F_METHOD_PROTO(void,Render_DeleteBuffer);

F_METHOD_PROTO(void,Render_DrawGradient);
F_METHOD_PROTO(void,Render_Fill);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFX_INCLUDE_VERSION);
   
   if ((CUD -> Arbitrer = F_NewObj(FC_Object, TAG_DONE)))
   {
      return (FObject) F_SUPERDO();
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DisposeObj(CUD -> Arbitrer); CUD -> Arbitrer = NULL;

   if (CyberGfxBase)
   {
      CloseLibrary(CyberGfxBase); CyberGfxBase = NULL;
   }

   F_SUPERDO();
}
//+

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
            F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_BOTH(Render_AddClip,        "AddClip",        FM_Render_AddClip),
            F_METHODS_ADD_BOTH(Render_RemClip,        "RemClip",        FM_Render_RemClip),
            F_METHODS_ADD_BOTH(Render_AddClipRegion,  "AddClipRegion",  FM_Render_AddClipRegion),
            F_METHODS_ADD_BOTH(Render_CreateBuffer,   "CreateBuffer",   FM_Render_CreateBuffer),
            F_METHODS_ADD_BOTH(Render_DeleteBuffer,   "DeleteBuffer",   FM_Render_DeleteBuffer),
            F_METHODS_ADD     (Render_DrawGradient,   "DrawGradient"),
            F_METHODS_ADD(Render_Fill, "Fill"),

            F_METHODS_ADD_STATIC(Render_New,       FM_New),
            F_METHODS_ADD_STATIC(Render_Get,       FM_Get),
            F_METHODS_ADD_STATIC(Render_Set,       FM_Set),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
   }
   return NULL;
}
