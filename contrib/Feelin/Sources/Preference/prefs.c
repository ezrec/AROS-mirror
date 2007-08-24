#include "Private.h"
//#define DB_ADD

#define BIG_ENDIAN_MACHINE 1

#ifdef __AROS__
 #include <aros/macros.h>
 #if !AROS_BIG_ENDIAN
  #undef BIG_ENDIAN_MACHINE
  #define BIG_ENDIAN_MACHINE 0
 #endif
#endif

#define FV_PREFERENCE_TABLE_SIZE                FV_HASH_NORMAL

#warning "stegerg CHECKME CHECKME CHECKME CHECKME: endianness stuff for integer items!!"

/***************************************************************************/
/*** Private ***************************************************************/
/***************************************************************************/

///p_obtain_storage
APTR p_obtain_storage(FClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (!LOD -> Pool)
   {
      F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Exclusive);

      if ((LOD -> Pool = F_CreatePool(1024,FA_Pool_Items,1,FA_Pool_Name,"Preference.Chunks",TAG_DONE)) != NULL)
      {
         if ((LOD -> Table = F_NewP(LOD -> Pool,sizeof (FHashTable) + sizeof (APTR) * FV_PREFERENCE_TABLE_SIZE)) != NULL)
         {
            LOD -> Table -> Size = FV_PREFERENCE_TABLE_SIZE;
            LOD -> Table -> Entries = (APTR)((uint32)(LOD -> Table) + sizeof (FHashTable));
         }
         else
         {
            F_DeletePool(LOD -> Pool); LOD -> Pool = NULL;
         }
      }
   
      F_Do(CUD -> ListSemaphore,FM_Unlock);
   }
   return LOD -> Pool;
}
//+
///p_remove_item
int32 p_remove_item(FClass *Class,FObject Obj,FPItem *Item,uint32 Hash)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   FHashLink *node,*prev = NULL;

   for (node = LOD -> Table -> Entries[Hash] ; node ; node = node -> Next)
   {
      if ((uint32)(node) == (uint32)(Item))
      {
         if (prev)
         {
            prev -> Next = node -> Next;
         }
         else
         {
            LOD -> Table -> Entries[Hash] = node -> Next;
         }
      
         F_Dispose(node);

         return TRUE;
      }

      prev = node;
   }
   return FALSE;
}
//+
 
/***************************************************************************/
/*** Methods ***************************************************************/
/***************************************************************************/

///Prefs_Find
F_METHODM(FPItem *,Prefs_Find,FS_Preference_Find)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Table && Msg -> Name)
   {
      uint32 hash;
      FPItem *item;

//      F_Log(0,"%s",Msg -> Name + 1);
 
      if ((item = (FPItem *) F_HashFind(LOD -> Table,Msg -> Name + 1,F_StrLen(Msg -> Name) - 1,&hash)) != NULL)
      {
//         F_Log(0,"FOUND (%s)(%s)",item -> Key,item -> Data);
 
         return item -> Data;
      }
/*
      else
      {
         for (item = (FPItem *) LOD -> Table -> Entries[hash] ; item ; item = item -> Next)
         {
            F_Log(0,"HASH (0x%08lx) (%s)(%s)",hash,item -> Key,item -> Data);
         }
      }
*/
   }
   return NULL;
}
//+
///Prefs_Add
F_METHODM(APTR,Prefs_Add,FS_Preference_Add)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   uint32 len = F_StrLen(Msg -> Name);

   if (len)
   {
      uint32 hash;
      FPItem *item;

      if ((item = (FPItem *) F_HashFind(LOD -> Table,Msg -> Name + 1,len - 1,&hash)) != NULL)
      {
         p_remove_item(Class,Obj,item,hash);
      }

      if (Msg -> Data && Msg -> Size && p_obtain_storage(Class,Obj))
      {
         uint32 long_len = ((len + 3) >> 2) << 2;
         uint32 long_data = ((Msg -> Size + 3) >> 2) << 2;
         
         len--;
         
         F_OPool(LOD -> Pool);
         
         /* I don't add a byte to the name length because item have a  leading
         dollar sign ($) that is striped here */
    
         if ((item = F_NewP(LOD -> Pool,sizeof (FPItem) + long_len + long_data)) != NULL)
         {
            item -> Key = (STRPTR)((uint32)(item) + sizeof (FPItem));
            item -> KeyLength = len;
            item -> Data = (APTR)((uint32)(item) + sizeof (FPItem) + long_len);
            
            item -> NameSize = long_len;
            item -> DataSize = long_data;

            CopyMem(Msg -> Name + 1,item -> Key,len);
            CopyMem(Msg -> Data,item -> Data,Msg -> Size);
            
            item -> Next = (FPItem *) LOD -> Table -> Entries[hash];
            LOD -> Table -> Entries[hash] = (FHashLink *) item;
/*
            {
               uint32 data;
               
               if (data = F_DoA(Obj,F_IDM(FM_Preference_Find),Msg))
               {
                  F_Log(0,"check (%s)",data);
               }
               else
               {
                  F_Log(0,"FAILED!!! (%s)",item -> Data);
               }
                
            }
*/
//            F_Log(0,"item (0x%08lx) Key (%s)(%ld)(%s)",item,item -> Key,item -> KeyLength,item -> Data);
         }
         
         F_RPool(LOD -> Pool);
         
         return item;
      }
   }

   return NULL;
}
//+
///Prefs_AddLong
F_METHODM(uint32,Prefs_AddLong,FS_Preference_AddLong)
{
#if BIG_ENDIAN
   return F_Do(Obj,F_IDM(FM_Preference_Add),Msg -> Name,&Msg -> Data,4);
#else
   ULONG data = AROS_LONG2BE(Msg -> Data);

   return F_Do(Obj,F_IDM(FM_Preference_Add),Msg -> Name,&data,4);   
#endif   
}
//+
///Prefs_AddString
F_METHODM(uint32,Prefs_AddString,FS_Preference_AddString)
{
   uint32 len = F_StrLen(Msg -> Data);

//   F_Log(0,"ADD %s - CONTENTS %s",Msg -> Name,Msg -> Data);

   return F_Do(Obj,F_IDM(FM_Preference_Add),Msg -> Name,Msg -> Data,(len) ? len + 1 : 0);
}
//+
///Prefs_Remove
F_METHODM(int32,Prefs_Remove,FS_Preference_Remove)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   int32 rc=FALSE;

   if (LOD -> Pool && Msg -> Name)
   {
      uint32 hash;
      FPItem *item;

      F_OPool(LOD -> Pool);
      
      if ((item = (FPItem *) F_HashFind(LOD -> Table,Msg -> Name + 1,F_StrLen(Msg -> Name + 1),&hash)) != NULL)
      {
         rc = p_remove_item(Class,Obj,item,hash);
      }
      
      F_RPool(LOD -> Pool);
   }
   
   return rc;
}
//+
///Prefs_Clear
F_METHOD(void,Prefs_Clear)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Exclusive);

   F_DeletePool(LOD -> Pool); LOD -> Pool = NULL; LOD -> Table = NULL;

   F_Do(CUD -> ListSemaphore,FM_Unlock);
}
//+
///Prefs_Resolve
F_METHODM(APTR,Prefs_Resolve,FS_Preference_Resolve)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
/*
   if (Msg -> Name[0] == 'F' && Msg -> Name[1] == 'P' && Msg -> Name[2] == '_')
   {
      F_Log(0,"OBSOLETE (%s)",Msg -> Name);
      
      return Msg -> Default;
   }
*/
   if (!LOD -> Table)
   {
      return Msg -> Default;
   }
   else if (((uint32)(Msg -> Name) > 0xFFFF) && (*Msg -> Name == '$'))
   {
      APTR data;
      
      F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Shared);
      
      data = (APTR) F_DoA(Obj,F_IDM(FM_Preference_Find),Msg);
      if (!data) data = (APTR) F_DoA(LOD -> Reference,F_IDM(FM_Preference_Find),Msg);

///DB_RESOLVE
         if (CUD -> db_Resolve)
         {
            if (!data)
            {
               F_Log(0,"not defined '%s'",Msg -> Name);
            }
         }
//+
      
      F_Do(CUD -> ListSemaphore,FM_Unlock);

      if (!data) data = Msg -> Default;
         
      return data;
   }
   return Msg -> Name;
}
//+
///Prefs_ResolveInt
F_METHODM(uint32,Prefs_ResolveInt,FS_Preference_ResolveInt)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
/*
   if (Msg -> Name[0] == 'F' && Msg -> Name[1] == 'P' && Msg -> Name[2] == '_')
   {
      F_Log(0,"OBSOLETE (%s)",Msg -> Name);

      return Msg -> Default;
   }
*/
   if (!LOD -> Table)
   {
      return Msg -> Default;
   }
   else if (((uint32)(Msg -> Name) > 0xFFFF) && (*Msg -> Name == '$'))
   {
      APTR data;

      F_Do(CUD -> ListSemaphore,FM_Lock,FF_Lock_Shared);

      data = (APTR) F_DoA(Obj,F_IDM(FM_Preference_Find),Msg);
      if (!data) data = (APTR) F_DoA(LOD -> Reference,F_IDM(FM_Preference_Find),Msg);

      F_Do(CUD -> ListSemaphore,FM_Unlock);

      if (data)
      {
      #if BIG_ENDIAN_MACHINE
         return *((uint32 *)(data));
      #else
         return AROS_BE2LONG(*((uint32 *)(data)));
      #endif
      }
      else
      {

///DB_RESOLVE
            if (CUD -> db_Resolve)
            {
               if (!data)
               {
                  F_Log(0,"not defined '%s'",Msg -> Name);
               }
            }
//+

         return Msg -> Default;
      }
   }
   return (uint32)(Msg -> Name);
}
//+

