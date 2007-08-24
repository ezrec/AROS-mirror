/*

$VER: AppServer 05.00 (2005/08/10)
 
   Portability upgrade.
   
   Help bubble are no longer opened during a DnD process.
 
$VER: AppServer 04.00 (2005/02/06)
 
   DnD handling rewritten completely (again). Instead of reading  intuition
   messages  on  the  AppServer  thread, I add a FEventHandler, with a very
   high priority, to the window where the DnD begins. Thus I  use  the  new
   event system and receive them through the FM_HandleEvent method.
   
   Bob routines have been rewritten too. Because  I  was  replacing  screen
   GelsInfo, after a DnD icons operations crashed the system. This is fixed
   now, hopefully :-)
 
$VER AppServer 03.00 (2004/08/13)

   AppServer is now a shared object, because servers are deprecated since v8
   of feelin.library. AppServer is now a subclass of FC_Family.

   AppServer now handles PopHelp and the root FC_Preference object.

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,AppServer_New);
F_METHOD_PROTO(void,AppServer_Dispose);
F_METHOD_PROTO(void,AppServer_Get);
F_METHOD_PROTO(void,AppServer_RemMember);
F_METHOD_PROTO(void,AppServer_DnDHandle);
F_METHOD_PROTO(void,AppServer_HandleEvent);
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
            F_ATTRIBUTES_ADD("Preference",  FV_TYPE_OBJECT),
             
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
 
