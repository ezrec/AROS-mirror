#include "Private.h"

#define BIG_ENDIAN_MACHINE 1

#ifdef __AROS__
 #include <aros/macros.h>
 #if !AROS_BIG_ENDIAN
  #undef BIG_ENDIAN_MACHINE
  #define BIG_ENDIAN_MACHINE 0
 #endif
#endif

#define ID_FP_X                  MAKE_ID('[','F','P',']')
/*
///prefs_strip
void prefs_strip(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Reference)
   {
      struct LocalObjectData *rLOD = F_LOD(Class,LOD -> Reference);

      ULONG i;

      F_OPool(LOD -> Pool);
      F_OPool(rLOD -> Pool);

      for (i = 0 ; i < LOD -> HashTable -> Size ; i++)
      {
         FPBase *base,*next;

         for (base = (FPBase *)(LOD -> HashTable -> Entries[i]) ; base ; base = next)
         {
            FPBase *refb;

            if (refb = (FPBase *) F_HashFind(rLOD -> HashTable,base -> Hash.Key,base -> Hash.KeyLength,NULL))
            {
               APTR next = base -> ItemList.Head;
               FPItem *item;

//               F_Log(0,"SAME_BASE : '%s'",refb -> Hash.Key);

               while (item = F_NextNode(&next))
               {
                  FPItem *refi;

                  if (refi = prefs_find_item(refb,item -> Name))
                  {
//                     F_Log(0,"SAME_ITEM : '%s'",refi -> Name);

                     if (item -> DataSize == refi -> DataSize)
                     {

                        Data is long word aligned, so long word comparaison
                        can be used for speed

                        ULONG * idata = (ULONG *)(item -> Data);
                        ULONG * rdata = (ULONG *)(refi -> Data);
                        ULONG i;

                        for (i = 0 ; i < item -> DataSize ; i += 4)
                        {
                           if (*idata++ != *rdata++) break;
                        }

//                        F_Log(0,"i %ld - datasize %ld",i,item -> DataSize);

                        if (i == item -> DataSize)
                        {
//                           F_Log(0,"SAME_ITEM_DATA");

                           prefs_remove_item(LOD -> Pool,base,item -> Name);
                        }
                     }
                  }
               }
            }

            next = (FPBase *)(base -> Hash.Next);

            if (!base -> ItemList.Head)
            {
//               F_Log(0,"base '%s' is empty",base -> Hash.Key);

               F_HashRemLink(LOD -> HashTable,&base -> Hash);
               F_Dispose(base);
            }
         }
      }

      F_RPool(rLOD -> Pool);
      F_RPool(LOD -> Pool);
   }
}
//+
*/
/*** Methods ***************************************************************/

/*

   IFF NODE:
   
   4 bytes - long name length
   ? bytes - name
   4 bytes - long data length
   ? bytes - data

*/
 
///Prefs_Read
F_METHODM(void,Prefs_Read,FS_Preference_Read)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct IFFHandle *iff;
   STRPTR name;

   if (!(name = p_resolve_name(Class,Obj,Msg -> Name))) return;

   if (p_obtain_storage(Class,Obj) && (iff = AllocIFF()))
   {
      if ((iff -> iff_Stream = Open(name,MODE_OLDFILE)) != NULL)
      {
         InitIFFasDOS(iff);

         if (!OpenIFF(iff,IFFF_READ))
         {
            StopChunk(iff,MAKE_ID('P','R','E','F'),ID_FP_X);

            F_OPool(LOD -> Pool);

            while (!ParseIFF(iff,IFFPARSE_SCAN))
            {
               UWORD long_name_len=0;
               UWORD long_data_len=0;
 
               ReadChunkBytes(iff,&long_name_len,2);
               ReadChunkBytes(iff,&long_data_len,2);
               
    	    #if !BIG_ENDIAN_MACHINE
	       long_name_len = AROS_BE2WORD(long_name_len);
	       long_data_len = AROS_BE2WORD(long_data_len);	       
	    #endif
	    	       
               if (long_name_len && long_data_len)
               {
                  FPItem *item;
 
                  if ((item = F_NewP(LOD -> Pool,sizeof (FPItem) + long_name_len + long_data_len)) != NULL)
                  {
                     FPItem *same;
                     ULONG hash;
 
                     item -> Key = (STRPTR)((ULONG)(item) + sizeof (FPItem));
                     item -> Data = (APTR)((ULONG)(item) + sizeof (FPItem) + long_name_len);

                     item -> NameSize = long_name_len;
                     item -> DataSize = long_data_len;

                     ReadChunkBytes(iff,item -> Key,long_name_len);
                     ReadChunkBytes(iff,item -> Data,long_data_len);
                     
                     item -> KeyLength = F_StrLen(item -> Key);

                     if ((same = (FPItem *) F_HashFind(LOD -> Table,item -> Key,item -> KeyLength,&hash)) != NULL)
                     {
                        p_remove_item(Class,Obj,same,hash);
                     }
                  
//                     F_Log(0,"%04lx%04lx[%s][%s]",item -> NameSize,item -> DataSize,item -> Key,item -> Data);

                     item -> Next = (FPItem *) LOD -> Table -> Entries[hash];
                     LOD -> Table -> Entries[hash] = (FHashLink *) item;
                  }
               }
            }

///DB_READ
            if (CUD -> db_Read)
            {
               ULONG i;
 
               F_Log(0,"FILE: '%s'",name);

               for (i = 0 ; i < LOD -> Table -> Size ; i++)
               {
                  FPItem *item;

                  for (item = (FPItem *)(LOD -> Table -> Entries[i]) ; item ; item = (FPItem *)(item -> Next))
                  {
                     ULONG *data = item -> Data;
                     
                     if (((UBYTE *)(item -> Data))[0] > '!' && ((UBYTE *)(item -> Data))[1] < '~')
                     {
                        FPrintf(FeelinBase -> Console,"$[%32.32s] [%04ld.%04ld] ($%s)\n",item -> Data,item -> NameSize,item -> DataSize,item -> Key);
                     }
                     else
                     {
                        FPrintf(FeelinBase -> Console,"x[%08lx%08lx%08lx%08lx] [%04ld.%04ld] ($%s)\n",data[0],data[1],data[2],data[3],item -> NameSize,item -> DataSize,item -> Key);
                     }
                  }
               }
               FPrintf(FeelinBase -> Console,"\n");
            }
//+

            F_RPool(LOD -> Pool);

            CloseIFF(iff);
         }
         Close(iff -> iff_Stream);
      }
      FreeIFF(iff);
   }
   F_Dispose(name);
}
//+
///Prefs_Write
F_METHODM(void,Prefs_Write,FS_Preference_Write)
{
   struct LocalObjectData       *LOD = F_LOD(Class,Obj);
   struct IFFHandle             *iff;
   static ULONG                  version[] = { 0,1 };
   STRPTR                        name = Msg -> Name;

   if (!LOD -> Table)
   {
      return;
   }
 
//   prefs_strip(Class,Obj);

   if ((ULONG)(name) == FV_Preference_BOTH)
   {
      F_Do(Obj,Method,FV_Preference_ENVARC);

      name = (STRPTR)(FV_Preference_ENV);
   }

   if (!(name = p_resolve_name(Class,Obj,name))) return;

///DB_WRITE
   if (CUD -> db_Write)
   {
      ULONG i;

      F_Log(0,"FILE: '%s'",name);

      for (i = 0 ; i < LOD -> Table -> Size ; i++)
      {
         FPItem *item;

         for (item = (FPItem *)(LOD -> Table -> Entries[i]) ; item ; item = (FPItem *)(item -> Next))
         {
            ULONG *data = item -> Data;
 
            if (((UBYTE *)(item -> Data))[0] > '!' && ((UBYTE *)(item -> Data))[1] < '~')
            {
               FPrintf(FeelinBase -> Console,"$[%32.32s] [%04ld.%04ld] ($%s)\n",item -> Data,item -> NameSize,item -> DataSize,item -> Key);
            }
            else
            {
               FPrintf(FeelinBase -> Console,"x[%08lx%08lx%08lx%08lx] [%04ld.%04ld] ($%s)\n",data[0],data[1],data[2],data[3],item -> NameSize,item -> DataSize,item -> Key);
            }
         }
      }
      FPrintf(FeelinBase -> Console,"\n");
   }
//+

   if ((iff = AllocIFF()) != NULL)
   {
      if ((iff -> iff_Stream = Open(name,MODE_NEWFILE)) != NULL)
      {
         InitIFFasDOS(iff);

         if (!OpenIFF(iff,IFFF_WRITE))
         {
            ULONG i;

            PushChunk(iff,MAKE_ID('P','R','E','F'),MAKE_ID('F','O','R','M'),IFFSIZE_UNKNOWN);
            PushChunk(iff,MAKE_ID('P','R','E','F'),MAKE_ID('P','R','H','D'),8);
            WriteChunkBytes(iff,&version,8);
            PopChunk(iff);

            for (i = 0 ; i < LOD -> Table -> Size ; i++)
            {
               FPItem *item;

               for (item = (FPItem *) LOD -> Table -> Entries[i] ; item ; item = item -> Next)
               {
                  PushChunk(iff,MAKE_ID('P','R','E','F'),ID_FP_X,IFFSIZE_UNKNOWN);
                  
	       #if BIG_ENDIAN_MACHINE
                  WriteChunkBytes(iff,&item -> NameSize,sizeof (UWORD));
                  WriteChunkBytes(iff,&item -> DataSize,sizeof (UWORD));
               #else
	          {
		     UWORD namesize = AROS_WORD2BE(item->NameSize);
		     UWORD datasize = AROS_WORD2BE(item->DataSize);
		     
                     WriteChunkBytes(iff,&namesize,sizeof (UWORD));
                     WriteChunkBytes(iff,&datasize,sizeof (UWORD));
	          }
	       #endif
	       
                  WriteChunkBytes(iff,item -> Key,item -> NameSize);
                  WriteChunkBytes(iff,item -> Data,item -> DataSize);
                  
                  PopChunk(iff);
               }
            }
            PopChunk(iff);
            CloseIFF(iff);
         }
         Close(iff -> iff_Stream);
      }
      else
      {
         F_Log(FV_LOG_USER,"Unable to open file '%s'",name);
      }
      FreeIFF(iff);
   }

   F_Dispose(name);
}
//+

