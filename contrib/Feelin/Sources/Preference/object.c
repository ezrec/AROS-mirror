#include "Private.h"

/*** Private ***************************************************************/

///prefs_add
STATIC FFamilyNode * prefs_add(FObject Obj)
{
   FFamilyNode *node;

   F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Exclusive);

   if ((node = F_New(sizeof (FFamilyNode))) != NULL)
   {
      node -> Object = Obj;

      F_LinkTail(&CUD -> PreferenceList,(FNode *) node);
   }

   F_Do(CUD -> ListSemaphore,FM_Unlock);

   return node;
}
//+
///prefs_rem
STATIC void prefs_rem(FObject Obj)
{
   FFamilyNode *node;

   F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Exclusive);

   for (node = (FFamilyNode *)(CUD -> PreferenceList.Head) ; node ; node = node -> Next)
   {
      if (node -> Object == Obj)
      {
         F_LinkRemove(&CUD -> PreferenceList,(FNode *) node);
         F_Dispose(node);

         break;
      }
   }

   F_Do(CUD -> ListSemaphore,FM_Unlock);
}
//+

///p_resolve_name
STRPTR p_resolve_name(FClass *Class,FObject Obj,STRPTR Name)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   STRPTR path;

   if ((path = F_New(256)) != NULL)
   {
      switch ((ULONG)(Name))
      {
         case FV_Preference_ENV:
         {
            if (LOD -> Name)
            {
               CopyMem("ENV:Feelin",path,11);
               AddPart(path,LOD -> Name,256);
            }
         }
         break;

         case FV_Preference_ENVARC:
         {
            if (LOD -> Name)
            {
               CopyMem("ENVARC:Feelin",path,14);
               AddPart(path,LOD -> Name,256);
            }
         }
         break;

         case FV_Preference_BOTH:
         {
            F_Log(FV_LOG_DEV,"FV_Preference_BOTH not valid");
         }
         break;

         default:
         {
            AddPart(path,Name,256);
         }
         break;
      }

      if (*path)
      {
         CopyMem(".gui",path + F_StrLen(path),5);
      }
      else
      {
         F_Dispose(path); path = NULL;
      }
   }
   return path;
}
//+

/*** Methods ***************************************************************/

///Prefs_New
F_METHOD(FObject,Prefs_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Preference_Name:
      {
         LOD -> Name = (STRPTR)(item.ti_Data);
      }
      break;

      case FA_Preference_Reference:
      {
         LOD -> Reference = (FObject)(item.ti_Data);
      }
      break;
   }

   if (F_SUPERDO())
   {
      if (LOD -> Name)
      {
         if ((LOD -> FileName = p_resolve_name(Class,Obj,(STRPTR)(FV_Preference_ENV))) != NULL)
         {
            LOD -> Notify = DOSNotifyObject,"FA_DOSNotify_Name",LOD -> FileName,End;
         }

         if (LOD -> Notify)
         {
            F_Do(LOD -> Notify,FM_Notify,"FA_DOSNotify_Update",FV_Notify_Always,
                 Obj,FM_Set,2,F_IDA(FA_Preference_Update),TRUE);
         }
         else
         {
            F_Log(FV_LOG_USER,"Unable to create notify on '%s' (%s)",LOD -> FileName,LOD -> Name);

            return NULL;
         }
      }

      if (prefs_add(Obj))
      {
          return Obj;
      }
   }
   return NULL;
}
//+
///Prefs_Dispose
F_METHOD(void,Prefs_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   prefs_rem(Obj);

   F_DisposeObj(LOD -> Notify);  LOD -> Notify     = NULL;
   F_Dispose(LOD -> FileName);   LOD -> FileName   = NULL;
   F_DeletePool(LOD -> Pool);    LOD -> Pool       = NULL;
//   F_HashDelete(LOD -> Table);   LOD -> Table  = NULL;

   F_SUPERDO();
}
//+
///Prefs_Set
F_METHOD(void,Prefs_Set)
{
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Preference_Update:
      {
         FFamilyNode *node;

         F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Shared);

         for (node = (FFamilyNode *)(CUD -> PreferenceList.Head) ; node ; node = node -> Next)
         {
            struct LocalObjectData *node_lod = F_LOD(Class,node -> Object);

            if (node_lod -> Reference == Obj)
            {
               F_Set(node -> Object,F_IDA(FA_Preference_Update),item.ti_Data);
            }
         }

         F_Do(CUD -> ListSemaphore,FM_Unlock);
      }
      break;
   }

   F_SUPERDO();
}
//+
///Prefs_Get
F_METHOD(void,Prefs_Get)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg,item;

   F_SUPERDO();

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Preference_Name:        F_STORE(LOD -> Name);          break;
      case FA_Preference_Reference:   F_STORE(LOD -> Reference);     break;
   }
}
//+

