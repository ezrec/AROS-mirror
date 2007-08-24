#include "Private.h"

/*** Methods ***************************************************************/

///AppServer_New
F_METHOD(FObject,AppServer_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    LOD -> Server = Obj;

    LOD -> Thread = ThreadObject,

        "FA_Thread_Entry",    Thread_Main,
        "FA_Thread_Name",     "appserver.thread",
        "FA_Thread_UserData", LOD,

    End;
    
    if (LOD -> Thread)
    {
        if (F_SUPERDO())
        {
            LOD -> Preference = PreferenceObject,

                "FA_Preference_Name","«Global»",

            End;
            
            if (LOD -> Preference)
            {
                F_Do(LOD -> Preference,F_DynamicFindID("FM_Preference_Read"),FV_Preference_ENV);

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
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    F_DisposeObj(LOD -> Thread); LOD -> Thread = NULL;

    F_SUPERDO();

    F_DisposeObj(LOD -> Preference); LOD -> Preference = NULL;
}
//+
///AppServer_Get
F_METHOD(void,AppServer_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_AppServer_Preference:
      {
         F_STORE(LOD -> Preference);
      }
      break;
   }

   F_SUPERDO();
}
//+

