/*

 * 01.00 ** 2006/07/06 *
 
*/

#include "Private.h"

struct FeelinBase                  *FeelinBase = NULL;
struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,AppServer_New);
F_METHOD_PROTO(void,AppServer_Dispose);
F_METHOD_PROTO(void,AppServer_Get);
F_METHOD_PROTO(void,AppServer_RemMember);
F_METHOD_PROTO(void,AppServer_DnDHandle);
F_METHOD_PROTO(void,AppServer_HandleEvent);

F_THREAD_ENTRY_PROTO(Thread_Main);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      CUD -> Thread = ThreadObject,

         "FA_Thread_Entry", Thread_Main,
         "FA_Thread_Name",  "appserver.thread",

      End;
      
      if (CUD -> Thread)
      {
         CUD -> id_Send = F_DynamicFindID("FM_Thread_Send");
 
         return Obj;
      }
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DisposeObj(CUD -> Thread); CUD -> Thread = NULL;
   
   F_SUPERDO();
}
//+

F_QUERY()
{
   F_SAVE_BASE;

   switch (Which)
   {
///Meta
      case FV_Query_MetaTags:
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
         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD("Preference",  FV_OBJECT),
             
            F_ARRAY_END
         };

         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD(AppServer_DnDHandle,         "DnDHandle"),
            F_METHODS_ADD_BOTH(AppServer_HandleEvent,  "HandleEvent", FM_HandleEvent),

            F_METHODS_ADD_STATIC(AppServer_New,         FM_New),
            F_METHODS_ADD_STATIC(AppServer_Dispose,     FM_Dispose),
            F_METHODS_ADD_STATIC(AppServer_Get,         FM_Get),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Family),
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

