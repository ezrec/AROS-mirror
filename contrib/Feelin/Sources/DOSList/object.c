#include "Private.h"

struct FS_HOOK_ACTIVE                           { FClass *Class; STRPTR Active; };

struct FS_DOSList_Filter                        { STRPTR Path; struct FileInfoBlock *fib; };
 
/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///code_list_active
F_HOOKM(void,code_list_active,FS_HOOK_ACTIVE)
{
    struct FeelinClass *Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct ListEntry *le = (APTR) F_Do(Obj,F_IDR(FM_List_GetEntry),Msg -> Active);

    if (le)
    {
        uint32 activation = F_Get(Obj,(uint32)"Activation");

        if (activation == FV_List_Activation_Click)
        {
            switch (le -> Type)
            {
                case FV_ENTRY_VOLUME:
                case FV_ENTRY_ASSIGN:
                {
                    F_Set(Obj,F_IDA(FA_DOSList_Path),(uint32) le -> Name);
                }
                break;

                case FV_ENTRY_DIRECTORY:
                {
                    STRPTR buf = F_New(1024);

                    if (buf)
                    {
                        BPTR lock;

                        AddPart(buf,(STRPTR) LOD -> path,1024);
                        AddPart(buf,(STRPTR) le -> Name,1024);
                        
                        lock = Lock(buf,SHARED_LOCK);

                        if (lock)
                        {
                            NameFromLock(lock,buf,1024);

                            UnLock(lock);
                        }
                    }

                    F_Set(Obj,F_IDA(FA_DOSList_Path),(uint32) buf);

                    F_Dispose(buf);
                }
                break;

                case FV_ENTRY_FILE:
                {
                   F_Set(Obj,F_IDA(FA_DOSList_File),(uint32) le -> Name);
                }
                break;

                case FV_ENTRY_VOLUME_SEPARATOR:
                case FV_ENTRY_DIRECTORY_SEPARATOR:
                {
                   F_Do(Obj,FM_Set,FA_NoNotify,TRUE,"FA_List_Active",FV_List_Active_None,TAG_DONE);
                }
                break;
            }
        }
        else if (activation == FV_List_Activation_Key)
        {
            F_Dispose(LOD -> file); LOD -> file = NULL;
            
            if (le -> Type != FV_ENTRY_VOLUME_SEPARATOR && le -> Type != FV_ENTRY_DIRECTORY_SEPARATOR)
            {
                LOD -> file = F_StrNew(NULL,"%s",le -> Name);
            } 
        }
    }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/
 
///DOSList_New
F_METHOD(FObject,DOSList_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   F_AREA_SAVE_PUBLIC;
 
   if (F_SuperDo(Class,Obj,Method,
         
      "FA_List_ConstructHook",   &CUD -> construct_hook,
      "FA_List_DestructHook",    &CUD -> destruct_hook,
      "FA_List_CompareHook",     &CUD -> compare_hook,
      "FA_List_DisplayHook",     &CUD -> display_hook,

      /***/
      
//      F_IDA(FA_DOSList_Path),         FV_DOSList_Path_Work,
      F_IDA(FA_DOSList_FilterIcons),  TRUE,
      F_IDA(FA_DOSList_FilterFiles),  FALSE,
      F_IDA(FA_DOSList_Pattern),      "#?",
 
   TAG_MORE,Msg))
   {
      F_Do(Obj,FM_Notify,"FA_List_Active",FV_Notify_Always,Obj,FM_CallHookEntry,3,code_list_active,Class,FV_Notify_Value);

      return Obj;
   }
   return NULL;
}
//+
///DOSList_Dispose
F_METHOD(void,DOSList_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Dispose(LOD -> path);    LOD -> path    = NULL;
   F_Dispose(LOD -> file);    LOD -> file    = NULL;
   F_Dispose(LOD -> pattern); LOD -> pattern = NULL;
   F_Dispose(LOD -> tokens);  LOD -> tokens  = NULL;
   
   F_SUPERDO();
}
//+
///DOSList_Get
F_METHOD(void,DOSList_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_DOSList_Path:      F_STORE(LOD -> path); break;
      case FA_DOSList_File:      F_STORE(LOD -> file); break;
      case FA_DOSList_Pattern:   F_STORE(LOD -> pattern); break;
   }

   F_SUPERDO();
}
//+
///DOSList_Set

#define FF_UPDATE_PATH                          (1 << 0)

F_METHOD(void,DOSList_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;
    
    uint32 update=0;
    
    while  (tag = F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
///FA_DOSList_Path
        case FA_DOSList_Path:
        {
            STRPTR path=LOD -> path;

            if (item.ti_Data == FV_DOSList_Path_Volumes)
            {
                path = NULL;
            }
            else if (item.ti_Data == FV_DOSList_Path_Parent)
            {
                BPTR lock = Lock(LOD -> path,SHARED_LOCK);
                
                path = NULL;

                if (lock)
                {
                    BPTR pl = ParentDir(lock);

                    if (pl)
                    {
                        path = F_New(1024);
      
                        if (path)
                        {
                            NameFromLock(pl,path,1024);
                        }
                       
                        UnLock(pl);
                    }
                }
                UnLock(lock);
            }
            else if (item.ti_Data == FV_DOSList_Path_Prog)
            {
                BPTR lock = GetProgramDir();
                
                if (lock)
                {
                    path = F_New(1024);
                   
                    if (path)
                    {
                        NameFromLock(lock,path,1024);
                    }
                }
            }
            else if (item.ti_Data == FV_DOSList_Path_Work)
            {
                path = F_New(1024);
 
                if (path)
                {
                    GetCurrentDirName(path,1024);
                }
            }
            else
            {
                path = F_StrNew(NULL,"%s",item.ti_Data);
            }
      
      #ifdef __AROS__
      #warning "AROS FIXME: doslist lock stuff"
      #else
            if ((path != LOD -> path) && (path != NULL) && (LOD -> path != NULL))
            {
                struct FileLock *lock1 = BADDR(Lock(LOD -> path,SHARED_LOCK));

                if (lock1)
                {
                    struct FileLock *lock2 = BADDR(Lock(path,SHARED_LOCK));
                    
                    if (lock2)
                    {
                        if (lock1 -> fl_Key == lock2 -> fl_Key)
                        {
                           F_Dispose(path); path = LOD -> path;
                        }
                
                        UnLock(MKBADDR(lock2));
                    }

                    UnLock(MKBADDR(lock1));
                }
            }
    	#endif
	
            if (path != LOD -> path)
            {
                F_Dispose(LOD -> path);
                
                LOD -> path = path;

                update |= FF_UPDATE_PATH;
            }

            tag -> ti_Data = (uint32) LOD -> path;
            
   //         F_Log(0,">> path (0x%08lx)(%s)(%ld)",tag -> ti_Data,tag -> ti_Data,update);
        }
        break;
//+
         
        case FA_DOSList_File:
        {
            if (item.ti_Data)
            {
                STRPTR file = LOD -> file;
                STRPTR buf = F_New(1024);
                
//                F_Log(0,"file (0x%08lx)(%s)",item.ti_Data,item.ti_Data);

                if (buf)
                {
                    BPTR lock;
         
	    	    /* stegerg: CHECKME/FIXME LOD -> path can be NULL.
		                Therefore I added if (LOD -> path) check */
				
                    if (LOD -> path) AddPart(buf,(STRPTR) LOD -> path,1024);
                    AddPart(buf,(STRPTR) item.ti_Data,1024);
                    
//                    F_Log(0,"complete (%s)",buf);
        
                    lock = Lock(buf,SHARED_LOCK);
                    
                    if (lock)
                    {
                        struct FileInfoBlock *fib = AllocDosObject(DOS_FIB,TAG_DONE);
          
                        NameFromLock(lock,buf,1024);
                        
//                        F_Log(0,"real (%s)",buf);
                        
                        if (fib)
                        {
                            Examine(lock,fib);
                            
                            if (fib -> fib_DirEntryType > 0)
                            {
                                F_Set(Obj,F_IDA(FA_DOSList_Path),(uint32) buf);
                            }
                            else
                            {
                                STRPTR part = FilePart(buf);

                                file = F_StrNew(NULL,"%s",part);
                                
                                if (part > buf)
                                {
                                   *part = 0;
                                   
                                   F_Set(Obj,F_IDA(FA_DOSList_Path),(uint32) buf);
                                }
                            }

                            FreeDosObject(DOS_FIB,fib);
                        }
                         
                        UnLock(lock);
                    }
                    else
                    {
                        STRPTR part = FilePart(buf);
                     
                        file = F_StrNew(NULL,"%s",part);
          
                        if (part > buf && *part)
                        {
                            *part = 0;

                            F_Set(Obj,F_IDA(FA_DOSList_Path), (uint32) buf);
                        }
                        else
                        {
                            F_Set(Obj,F_IDA(FA_DOSList_Path), FV_DOSList_Path_Volumes);
                        }
                    }
                    F_Dispose(buf);
                }
             
                if (file != LOD -> file)
                {
                    F_Dispose(LOD -> file); LOD -> file = file;
                }
                
                tag -> ti_Data = (uint32) LOD -> file;
            }
            else
            {
                F_Dispose(LOD->file); LOD->file = NULL;
            }
            
   //         F_Log(0,">> file (0x%08lx)(%s)",tag -> ti_Data,tag -> ti_Data);
        }
        break;
     
        case FA_DOSList_FilterIcons:
        {
            if (item.ti_Data) LOD -> Flags |= FF_DOSLIST_FILTERICONS;
            else LOD -> Flags &= ~FF_DOSLIST_FILTERICONS;

            update |= FF_UPDATE_PATH;
        }
        break;
     
        case FA_DOSList_FilterFiles:
        {
            if (item.ti_Data)
            {
                LOD -> Flags |= FF_DOSLIST_FILTERFILES;

                if (LOD -> tokens)
                {
                    struct ListEntry *le;
                    uint32 i=0;
                  
                    F_Set(Obj,F_IDR(FA_List_Quiet),TRUE);
       
                    while (le = (struct ListEntry *) F_Do(Obj,F_IDR(FM_List_GetEntry),i))
                    {
                        if (le -> Type == FV_ENTRY_FILE && le->Name)
                        {
                            if (!MatchPatternNoCase(LOD -> tokens,le->Name))
                            {
                                F_Do(Obj,F_IDR(FM_List_Remove),i);
                            }
                            else i++;
                        }
                        else i++;
                    }
                
                    i = F_Get(Obj,(uint32) "FA_List_Entries");
               
                    if (i)
                    {
                        le = (struct ListEntry *) F_Do(Obj,F_IDR(FM_List_GetEntry),i - 1);

                        if (le)
                        {
                            if (le -> Type == FV_ENTRY_DIRECTORY_SEPARATOR)
                            {
                                F_Do(Obj,F_IDR(FM_List_Remove),i - 1);
                            }
                        }
                     }
               
                    F_Set(Obj,F_IDR(FA_List_Quiet),FALSE);
                }
            }
            else
            {
                LOD -> Flags &= ~FF_DOSLIST_FILTERFILES;
               
                update |= FF_UPDATE_PATH;
            }
        }
        break;
     
        case FA_DOSList_Pattern:
        {
            uint32 len;
    
            F_Dispose(LOD -> pattern);
            F_Dispose(LOD -> tokens);
            
            LOD -> pattern = F_StrNew(&len,"%s",item.ti_Data);
            LOD -> tokens = F_New(len * 3);
            
            if (LOD->pattern && LOD -> tokens)
            {
               ParsePatternNoCase(LOD -> pattern,LOD -> tokens,len * 3);
            }
    
            if (FF_DOSLIST_FILTERFILES & LOD -> Flags)
            {
               update |= FF_UPDATE_PATH;
            }
         
            tag -> ti_Data = (uint32) LOD -> pattern;
            
   //         F_Log(0,"wild (0x%08lx)(%s)",LOD -> pattern,LOD -> pattern);
        }
        break;
    }

    F_SUPERDO();

    /* path must be changed *after* passing the method  to  our  superclass,
    because FA_List_ConstructHook must be set *before* the path is read */
  
    if (FF_UPDATE_PATH & update)
    {
        F_Do(Obj,F_IDM(FM_DOSList_ChangePath),LOD -> path);
    }
}
//+

///DOSList_HandleEvent
F_METHODM(uint32,DOSList_HandleEvent,FS_HandleEvent)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (FF_EVENT_BUTTON == Msg -> Event -> Class &&
       FF_EVENT_BUTTON_DOWN & Msg -> Event -> Flags &&
       (FV_EVENT_BUTTON_MENU == Msg -> Event -> Code || FV_EVENT_BUTTON_MIDDLE == Msg -> Event -> Code) &&
      _inside(Msg -> Event -> MouseX,_x,_x2) &&
      _inside(Msg -> Event -> MouseY,_y,_y2))
   {
      if (LOD -> path)
      {
         if (Msg -> Event -> Code == FV_EVENT_BUTTON_MENU)
         {
            F_Set(Obj,F_IDA(FA_DOSList_Path),FV_DOSList_Path_Parent);
         }
         else if (Msg -> Event -> Code == FV_EVENT_BUTTON_MIDDLE)
         {
            F_Set(Obj,F_IDA(FA_DOSList_Path),FV_DOSList_Path_Volumes);
         }
      }
      else
      {
         F_Set(Obj,F_IDA(FA_DOSList_Path),FV_DOSList_Path_Work);
      }
      return FF_HandleEvent_Eat;
   }
   return F_SUPERDO();
}
//+
///DOSList_ChangePath
F_METHODM(void,DOSList_ChangePath,FS_DOSList_ChangePath)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct ListEntryRaw ler;
   
   F_Set(Obj,F_IDR(FA_List_Quiet),TRUE);

   F_Do(Obj,F_IDR(FM_List_Remove),FV_List_Remove_All);

   if (!Msg -> Path || (Msg -> Path && !*Msg -> Path))
   {
      struct DosList *dl = LockDosList(LDF_READ | LDF_VOLUMES | LDF_ASSIGNS);
      struct DosList *cl;

      F_Set(Obj,F_IDR(FA_List_Format),(uint32) "<col fixed='true'/><col /><col fixed='true' />");
    
      cl = dl; ler.Type = FV_ENTRY_VOLUME;
    
      while (cl = NextDosEntry(cl,LDF_VOLUMES))
      {
         ler.Data = cl; F_Do(Obj,F_IDR(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
      }

      ler.Type = FV_ENTRY_VOLUME_SEPARATOR; F_Do(Obj,F_IDR(FM_List_InsertSingle),&ler,FV_List_Insert_Bottom);

      cl = dl; ler.Type = FV_ENTRY_ASSIGN;

      while (cl = NextDosEntry(cl,LDF_ASSIGNS))
      {
         ler.Data = cl; F_Do(Obj,F_IDR(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
      }

      UnLockDosList(LDF_READ | LDF_VOLUMES | LDF_ASSIGNS);
   }
   else
   {
      struct FileInfoBlock *fib;
      BPTR lock;

      F_Set(Obj,F_IDR(FA_List_Format),(uint32) "<col /><col fixed='true' />");

      if (fib = AllocDosObject(DOS_FIB, NULL))
      {
         if (lock = Lock(Msg -> Path, SHARED_LOCK))
         {
            BOOL success = Examine(lock, fib);
            BOOL isdir = (fib -> fib_DirEntryType > 0);

            if (success && isdir)
            {
               uint32 ndirs = 0;
               uint32 nfiles = 0;

               while (success = ExNext(lock, fib))
               {
                  if (isdir = (fib -> fib_DirEntryType > 0))
                  {
                     ndirs++; ler.Type = FV_ENTRY_DIRECTORY;
                  }
                  else
                  {
                     if (FF_DOSLIST_FILTERICONS & LOD -> Flags)
                     {
                        WORD len = F_StrLen(fib -> fib_FileName);

                        if (len >= 5)
                        {
                           if (Stricmp(fib -> fib_FileName + len - 5, ".info") == 0) continue;
                        }
                     }
                  
                     if (LOD -> filter_hook)
                     {
                        struct FS_DOSList_Filter msg;
                        
                        msg.Path = Msg -> Path;
                        msg.fib  = fib;
                        
                        if (!CallHookPkt(LOD -> filter_hook,Obj,&msg)) continue;
                     }
                  
                     if (FF_DOSLIST_FILTERFILES & LOD -> Flags)
                     {
                        if (!MatchPatternNoCase(LOD -> tokens,fib -> fib_FileName)) continue;
                     }
                     
                     nfiles++; ler.Type = FV_ENTRY_FILE;
                  }
               
                  ler.Data = fib;
               
                  F_Do(Obj,F_IDR(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
               }
            
               if (ndirs && nfiles)
               {
                  ler.Type = FV_ENTRY_DIRECTORY_SEPARATOR; F_Do(Obj,F_IDR(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
               }
            }
            UnLock(lock);
         }
         FreeDosObject(DOS_FIB,fib);
      }
   }

   F_Set(Obj,F_IDR(FA_List_Quiet),FALSE);
}
//+
