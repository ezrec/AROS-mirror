#include "Private.h"

/*** Methods ***************************************************************/

///AppServer_New
F_METHOD(FObject,AppServer_New)
{
   if (!CUD -> Server)
   {
      if (F_SUPERDO())
      {
         CUD -> Preference = PreferenceObject,

            "FA_Preference_Name","«Global»",

         End;
         
         if (CUD -> Preference)
         {
            F_Do(CUD -> Preference,F_DynamicFindID("FM_Preference_Read"),FV_Preference_ENV);

            CUD -> Server = Obj;

            return Obj;
         }
      }
   }
   return NULL;
}
//+
///AppServer_Dispose
F_METHOD(void,AppServer_Dispose)
{
   F_SUPERDO();

   F_DisposeObj(CUD -> Preference); CUD -> Preference = NULL;

   CUD -> Server = NULL;
}
//+
///AppServer_Get
F_METHOD(void,AppServer_Get)
{
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_AppServer_Preference:
      {
         F_STORE(CUD -> Preference);
      }
      break;
   }

   F_SUPERDO();
}
//+

