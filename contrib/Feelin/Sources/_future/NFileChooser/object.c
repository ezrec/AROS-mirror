#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/
/*
///filechooser_select_disabled
void filechooser_select_disabled(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (!F_Get(LOD -> Path,(ULONG) "Contents"))
    {
        if (!F_Get(LOD -> icons,FA_Disabled))
        {
            F_Do(Obj,FM_MultiSet,FA_Disabled,TRUE,LOD -> icons,LOD -> volumes,LOD -> parent,LOD -> match,NULL);
        }
    }
    else
    {   
        if (F_Get(LOD -> icons,FA_Disabled))
        {
            F_Do(Obj,FM_MultiSet,FA_Disabled,FALSE,LOD -> icons,LOD -> volumes,LOD -> parent,LOD -> match,NULL);
        }
    }
}
//+
*/

///filechooser_create_volumes_list
void filechooser_create_volumes_list(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct DosList *dl = LockDosList(LDF_READ | LDF_VOLUMES | LDF_ASSIGNS);
    struct DosList *cl;
    struct ListEntryRaw ler;

    F_Set(LOD -> list_volumes,F_IDO(FA_List_Quiet),TRUE);
    F_Do(LOD -> list_volumes,F_IDO(FM_List_Remove),FV_List_Remove_All);

    cl = dl; ler.Type = FV_ENTRY_VOLUME;

    while (cl = NextDosEntry(cl,LDF_VOLUMES))
    {
        ler.Data = cl; F_Do(LOD -> list_volumes,F_IDO(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
    }

    ler.Type = FV_ENTRY_VOLUME_SEPARATOR; F_Do(LOD -> list_volumes,F_IDO(FM_List_InsertSingle),&ler,FV_List_Insert_Bottom);

    cl = dl; ler.Type = FV_ENTRY_ASSIGN;

    while (cl = NextDosEntry(cl,LDF_ASSIGNS))
    {
        ler.Data = cl; F_Do(LOD -> list_volumes,F_IDO(FM_List_InsertSingle),&ler,FV_List_Insert_Sorted);
    }

    UnLockDosList(LDF_READ | LDF_VOLUMES | LDF_ASSIGNS);

    F_Set(LOD -> list_volumes,F_IDO(FA_List_Quiet),FALSE);
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///FileChooser_New
F_METHOD(FObject,FileChooser_New)
{
    struct LocalObjectData * LOD = F_LOD(Class,Obj);

    if (F_SuperDo(Class,Obj,Method,
    
        FA_Horizontal, TRUE,

        Child, ListviewObject, FA_ChainToCycle,FALSE, FA_Weight,25, /*FA_MinHeight,100,*/
            "FA_Listview_List", LOD -> list_volumes = ListObject, FA_SetMin,FV_SetWidth, FA_SetMax,FV_SetWidth,
//                "FA_List_Format", "<col fixed='true'/><col /><col fixed='true' />",
                "FA_List_ConstructHook", &CUD -> hook_volumes_construct,
                "FA_List_DestructHook", &CUD -> hook_destruct,
                "FA_List_CompareHook", &CUD -> hook_compare,
                "FA_List_DisplayHook", &CUD -> hook_volumes_display,
            End,
        End,

//        Child, BalanceObject, End,

        Child, VGroup,
            Child, HGroup,
                Child, ImageObject, "FA_Image_Spec",DEF_FILECHOOSER_IMAGE_HOME, FA_SetMax,FV_SetBoth, End,
                Child, ImageObject, "FA_Image_Spec",DEF_FILECHOOSER_IMAGE_BACK, FA_SetMax,FV_SetBoth, End,
                Child, ImageObject, "FA_Image_Spec",DEF_FILECHOOSER_IMAGE_FORWARD, FA_SetMax,FV_SetBoth, End,
                Child, AreaObject, FA_MaxHeight,2, End,
            End,
            
            Child, ListviewObject, FA_ChainToCycle,FALSE, FA_MinHeight,100,
                "FA_Listview_List", LOD -> list_files = ListObject, End,
            End,
/*
            Child, HGroup, FA_SetMax,FV_SetHeight,
                Child, LOD -> file = StringObject, "AdvanceOnCR",FALSE, FA_Weight,80, End,
                Child, LOD -> balance = BalanceObject, End,
                Child, LOD -> pattern = StringObject, "AdvanceOnCR",FALSE, FA_Weight,20, End,
                Child, LOD -> match = F_MakeObj(FV_MakeObj_Button,"?", FA_ChainToCycle,FALSE, FA_InputMode,FV_InputMode_Toggle, FA_SetMax,FV_SetBoth, TAG_DONE),
                Child, LOD -> icons = F_MakeObj(FV_MakeObj_Button,"i", FA_ChainToCycle,FALSE, FA_InputMode,FV_InputMode_Toggle, FA_SetMax,FV_SetBoth, TAG_DONE),
            End,
*/
        End,

        TAG_MORE, Msg))
    {
        filechooser_create_volumes_list(Class,Obj);

        F_Do(LOD -> list_volumes,FM_Notify,"FA_List_Active",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Activate),2, FV_FILECHOOSER_LIST_VOLUMES,FV_Notify_Value);
 
        return Obj;
    }
    return NULL;



/*
    struct TagItem *Tags = Msg,*tag,item;

    STRPTR path="Prog",file=NULL;
    
    while  (tag = F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path: path = (STRPTR) item.ti_Data; tag -> ti_Tag = TAG_IGNORE; break;
        case FA_FileChooser_File: file = (STRPTR) item.ti_Data; tag -> ti_Tag = TAG_IGNORE; break;
    }

    if (F_SuperDo(Class,Obj,Method,
            
        F_IDA(FA_FileChooser_FilterIcons),  TRUE,
        F_IDA(FA_FileChooser_FilterFiles),  FALSE,
        F_IDA(FA_FileChooser_Pattern),      "#?",
        
        Child, ListviewObject, FA_ChainToCycle,FALSE, FA_MinHeight,100, "FA_Listview_List",
            LOD -> list = DOSListObject, End,
        End,
        
        Child, HGroup,
            Child, LOD -> volumes = F_MakeObj(FV_MakeObj_Button,":",FA_ChainToCycle,FALSE,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD -> parent = F_MakeObj(FV_MakeObj_Button,"/",FA_ChainToCycle,FALSE,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD -> Path = StringObject, "AdvanceOnCR",FALSE, End,
        End,

        Child, HGroup, FA_SetMax,FV_SetHeight,
            Child, LOD -> File = StringObject, "AdvanceOnCR",FALSE, FA_Weight,80, End,
            Child, LOD -> balance = BalanceObject, End,
            Child, LOD -> pattern = StringObject, "AdvanceOnCR",FALSE, FA_Weight,20, End,
            Child, LOD -> match = F_MakeObj(FV_MakeObj_Button,"?",FA_ChainToCycle,FALSE,FA_InputMode,FV_InputMode_Toggle,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD -> icons = F_MakeObj(FV_MakeObj_Button,"i",FA_ChainToCycle,FALSE,FA_InputMode,FV_InputMode_Toggle,FA_SetMax,FV_SetBoth,TAG_DONE),
        End,  

    TAG_MORE,Msg))
    {
        F_Set(LOD -> File,(ULONG) "AttachedList",(ULONG) LOD -> list);

        F_Do(LOD -> Path,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_PATH,FV_Notify_Value);
        F_Do(LOD -> File,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_FILE,FV_Notify_Value);
        F_Do(LOD -> list,FM_Notify, "FA_DOSList_Path",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_LIST_PATH,FV_Notify_Value);
        F_Do(LOD -> list,FM_Notify, "FA_DOSList_File",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_LIST_FILE,FV_Notify_Value);
        F_Do(LOD -> list,FM_Notify, "FA_List_Activation",FV_List_Activation_DoubleClick,Obj,FM_Set,2,F_IDA(FA_FileChooser_Choosed),TRUE);
        F_Do(LOD -> list,FM_Notify, "FA_List_Activation",FV_List_Activation_Key,Obj,F_IDM(FM_FileChooser_Activate),0);
        
        F_Do(LOD -> pattern,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,FM_Set,2,F_IDA(FA_FileChooser_Pattern),FV_Notify_Value);

        F_Do(LOD -> volumes,FM_Notify,FA_Pressed,FALSE,LOD -> list,FM_Set,2,"FA_DOSList_Path","Volumes");
        F_Do(LOD -> parent,FM_Notify,FA_Pressed,FALSE,LOD -> list,FM_Set,2,"FA_DOSList_Path","Parent");
        F_Do(LOD -> match,FM_Notify,FA_Selected,FV_Notify_Always,Obj,FM_Set,2,F_IDA(FA_FileChooser_FilterFiles),FV_Notify_Value);
        F_Do(LOD -> icons,FM_Notify,FA_Selected,FV_Notify_Always,Obj,FM_Set,2,F_IDA(FA_FileChooser_FilterIcons),FV_Notify_Toggle);

        F_Do(Obj,FM_Set,
        
            F_IDA(FA_FileChooser_Path), path,
            F_IDA(FA_FileChooser_File), file,
            
            TAG_DONE);
 
        return Obj;
    }
    return NULL;
*/
}
//+
///FileChooser_Get
F_METHOD(void,FileChooser_Get)
{
/*
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path:     F_STORE(F_Get(LOD -> Path,(ULONG) "Contents")); break;
        case FA_FileChooser_File:     F_STORE(F_Get(LOD -> file,(ULONG) "Contents")); break;
        case FA_FileChooser_Pattern:  F_STORE(F_Get(LOD -> pattern,(ULONG) "Contents")); break;
    }
*/
    F_SUPERDO();
}
//+
///FileChooser_Set
F_METHOD(void,FileChooser_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    while  (tag = F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path:
        {
            if (!(FF_FILECHOOSER_PATH_NOTIFIED & LOD -> flags))
            {
                F_Log(0,"should update volumes list according to FA_FileChooser_Path (%s)",item.ti_Data);
                //F_Do(LOD -> list_volumes, FM_Set, FA_NoNotify,TRUE, "Path",item.ti_Data, TAG_DONE);
            }
            
            tag -> ti_Data = F_Get(LOD -> list,(ULONG) "Path");

            F_Do(LOD -> Path,FM_Set,FA_NoNotify,TRUE,"Contents",tag -> ti_Data,TAG_DONE);

//            filechooser_select_disabled(Class,Obj,FeelinBase);
      
//         F_Log(0,">> path (0x%08lx)(%s)",tag -> ti_Data,tag -> ti_Data);
        }
        break;
/*
        case FA_FileChooser_File:
        {
//         F_Log(0,"file (0x%08lx)(%s)",item.ti_Data,item.ti_Data);

            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"File",item.ti_Data,TAG_DONE);
            
            tag -> ti_Data = F_Get(LOD -> list,(ULONG) "File");
            
            F_Do(LOD -> File,FM_Set,FA_NoNotify,TRUE,"Contents",tag -> ti_Data,TAG_DONE);
            
//         F_Log(0,">> file (0x%08lx)(%s)",tag -> ti_Data,tag -> ti_Data);
        }
        break;
    
        case FA_FileChooser_FilterIcons:
        {
            F_Do(LOD -> icons,FM_Set,FA_NoNotify,TRUE,FA_Selected,!item.ti_Data,TAG_DONE);
            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"FilterIcons",item.ti_Data,TAG_DONE);
        }
        break;
    
        case FA_FileChooser_FilterFiles:
        {
            if (item.ti_Data)
            {
                if (F_Get(LOD -> pattern,FA_Hidden))
                {
                    F_Do(Obj,FM_MultiSet,FA_Hidden,FALSE,LOD -> balance,LOD -> pattern,NULL);
                }                
            }
            else
            {
                if (!F_Get(LOD -> pattern,FA_Hidden))
                {
                    F_Do(Obj,FM_MultiSet,FA_Hidden,TRUE,LOD -> balance,LOD -> pattern,NULL);
                }
            }

            F_Do(LOD -> match,FM_Set,FA_NoNotify,TRUE,FA_Selected,item.ti_Data,TAG_DONE);
            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"FilterFiles",item.ti_Data,TAG_DONE);
        }
        break;
    
        case FA_FileChooser_Pattern:
        {
            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"Pattern",item.ti_Data,TAG_DONE);
            
            tag -> ti_Data = F_Get(LOD -> list,(ULONG) "Pattern");

            F_Do(LOD -> pattern,FM_Set,FA_NoNotify,TRUE,"Contents",tag -> ti_Data,TAG_DONE);
        }
        break;
/*
        case FA_FileChooser_Choosed:
        {
            F_Log(0,"choosed");
        }
        break;
*/
*/
    }

    F_SUPERDO();
}
//+
 
///FileChooser_Setup
F_METHOD_SETUP(FileChooser_Setup)
{
    if (F_SUPERDO())
    {
        filechooser_create_volumes_list(Class,Obj);
    }
    
    return TRUE;
}
//+
   
///FileChooser_Update
F_METHODM(void,FileChooser_Update,FS_FileChooser_Update)
{
/*
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    switch (Msg -> From)
    {
        case FV_FROM_PATH:
        {
            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"Path",Msg -> Value,TAG_DONE);
            F_Do(LOD -> Path,FM_Set,FA_NoNotify,TRUE,"Contents",F_Get(LOD -> list,(ULONG) "Path"),TAG_DONE);
        }
        break;

        case FV_FROM_FILE:
        {
            ULONG prev = F_Get(LOD -> list,(ULONG) "Path");

            F_Do(LOD -> list,FM_Set,FA_NoNotify,TRUE,"File",Msg -> Value,TAG_DONE);
            F_Do(LOD -> File,FM_Set,FA_NoNotify,TRUE,"Contents",F_Get(LOD -> list,(ULONG) "File"),TAG_DONE);

            if (prev == F_Get(LOD -> list,(ULONG) "Path"))
            {
                F_Set(Obj,F_IDA(FA_FileChooser_Choosed),TRUE);
            }
        }
        break;

        case FV_FROM_LIST_PATH:
        {
            F_Do(LOD -> Path,FM_Set,FA_NoNotify,TRUE,"Contents",Msg -> Value,TAG_DONE);
        }
        break;

        case FV_FROM_LIST_FILE:
        {
            F_Do(LOD -> File,FM_Set,FA_NoNotify,TRUE,"Contents",Msg -> Value,TAG_DONE);
        }
        break;
    }

    if (Msg -> From == FV_FROM_PATH || Msg -> From == FV_FROM_LIST_PATH)
    {
        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_FileChooser_Path),F_Get(LOD -> list,(ULONG) "Path"),TAG_DONE);
    }
    else
    {
        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_FileChooser_File),F_Get(LOD -> list,(ULONG) "File"),TAG_DONE);
    }

    filechooser_select_disabled(Class,Obj,FeelinBase);
*/
}
//+
///FileChooser_Activate
F_METHODM(void,FileChooser_Activate,FS_FileChooser_Activate)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_Log(0,"Activate (%ld)(0x%08lx)",Msg -> From,Msg -> Active);

    if (Msg -> From == FV_FILECHOOSER_LIST_VOLUMES)
    {
        struct ListEntry *le = (APTR) F_Do(LOD -> list_volumes,F_IDO(FM_List_GetEntry),Msg -> Active);
        
        if (le -> Type == FV_ENTRY_VOLUME || le -> Type == FV_ENTRY_ASSIGN)
        {
            LOD -> flags |= FF_FILECHOOSER_PATH_NOTIFIED;
            
            F_Set(Obj,F_IDA(FA_FileChooser_Path),(uint32) le -> Name);
        }
        else
        {
            F_Do(LOD -> list_volumes,FM_Set, FA_NoNotify,TRUE, "FA_List_Active",FV_List_Active_None, TAG_DONE);
        }
    }
/*
    if (le)
    {
       uint32 activation = F_Get(Obj,F_IDO(FA_List_Activation));

       if (activation == FV_List_Activation_Click)
       {
          switch (le -> Type)
          {
             case FV_ENTRY_VOLUME:
             case FV_ENTRY_ASSIGN:
             {
                F_Set(Obj,F_IDA(FA_DOSList_Path),(ULONG) le -> Name);
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

                   if (lock = Lock(buf,SHARED_LOCK))
                   {
                      NameFromLock(lock,buf,1024);

                      UnLock(lock);
                   }
                }

                F_Set(Obj,F_IDA(FA_DOSList_Path),(ULONG) buf);

                F_Dispose(buf);
             }
             break;

             case FV_ENTRY_FILE:
             {
                F_Set(Obj,F_IDA(FA_DOSList_File),(ULONG) le -> Name);
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
*/
}
//+

