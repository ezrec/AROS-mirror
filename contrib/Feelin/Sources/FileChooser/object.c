#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///filechooser_select_disabled
void filechooser_select_disabled(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
/*
    F_Do(Obj,FM_MultiSet,

        FA_Disabled, FF_Disabled_Check | (F_Get(LOD->Path, (uint32) "Contents") ? FALSE : TRUE),

        LOD->icons, LOD->volumes, LOD->parent, LOD ->match, NULL);
*/
    if (!F_Get(LOD->Path,(uint32) "Contents"))
    {
        F_Do(Obj,FM_MultiSet,FA_Disabled,FF_Disabled_Check | TRUE,LOD->icons,LOD->volumes,LOD->parent,LOD->match,NULL);
    }
    else
    {
        F_Do(Obj,FM_MultiSet,FA_Disabled,FF_Disabled_Check | FALSE,LOD->icons,LOD->volumes,LOD->parent,LOD->match,NULL);
    }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///FileChooser_New
F_METHOD(FObject,FileChooser_New)
{
    struct LocalObjectData * LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    STRPTR path="Prog",file=NULL;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path: path = (STRPTR) item.ti_Data; tag->ti_Tag = TAG_IGNORE; break;
        case FA_FileChooser_File: file = (STRPTR) item.ti_Data; tag->ti_Tag = TAG_IGNORE; break;
    }

    if (F_SuperDo(Class,Obj,Method,

        F_IDA(FA_FileChooser_FilterIcons),  TRUE,
        F_IDA(FA_FileChooser_FilterFiles),  FALSE,
        F_IDA(FA_FileChooser_Pattern),      "#?",

        /***/

        Child, ListviewObject, FA_ChainToCycle,FALSE, FA_MinHeight,100, "FA_Listview_List",
            LOD->list = DOSListObject, End,
        End,

        Child, HGroup,
            Child, LOD->volumes = F_MakeObj(FV_MakeObj_Button,":",FA_ChainToCycle,FALSE,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD->parent = F_MakeObj(FV_MakeObj_Button,"/",FA_ChainToCycle,FALSE,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD->Path = StringObject, "AdvanceOnCR",FALSE, End,
        End,

        Child, HGroup, FA_SetMax,FV_SetHeight,
            Child, LOD->File = StringObject, "AdvanceOnCR",FALSE, FA_Weight,80, End,
            Child, LOD->balance = BalanceObject, End,
            Child, LOD->pattern = StringObject, "AdvanceOnCR",FALSE, FA_Weight,20, End,
            Child, LOD->match = F_MakeObj(FV_MakeObj_Button,"?",FA_ChainToCycle,FALSE,FA_InputMode,FV_InputMode_Toggle,FA_SetMax,FV_SetBoth,TAG_DONE),
            Child, LOD->icons = F_MakeObj(FV_MakeObj_Button,"i",FA_ChainToCycle,FALSE,FA_InputMode,FV_InputMode_Toggle,FA_SetMax,FV_SetBoth,TAG_DONE),
        End,

    TAG_MORE,Msg))
    {
        F_Set(LOD->File,(uint32) "AttachedList",(uint32) LOD->list);

        F_Do(LOD->Path,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_PATH,FV_Notify_Value);
        F_Do(LOD->File,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_FILE,FV_Notify_Value);
        F_Do(LOD->list,FM_Notify, "FA_DOSList_Path",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_LIST_PATH,FV_Notify_Value);
        F_Do(LOD->list,FM_Notify, "FA_DOSList_File",FV_Notify_Always, Obj,F_IDM(FM_FileChooser_Update),2,FV_FROM_LIST_FILE,FV_Notify_Value);
        F_Do(LOD->list,FM_Notify, "FA_List_Activation",FV_List_Activation_DoubleClick,Obj,FM_Set,2,F_IDA(FA_FileChooser_Choosed),TRUE);
        F_Do(LOD->list,FM_Notify, "FA_List_Activation",FV_List_Activation_Key,Obj,F_IDM(FM_FileChooser_Activate),0);

        F_Do(LOD->pattern,FM_Notify, "FA_String_Changed",FV_Notify_Always, Obj,FM_Set,2,F_IDA(FA_FileChooser_Pattern),FV_Notify_Value);

        F_Do(LOD->volumes,FM_Notify,FA_Pressed,FALSE,LOD->list,FM_Set,2,"FA_DOSList_Path","Volumes");
        F_Do(LOD->parent,FM_Notify,FA_Pressed,FALSE,LOD->list,FM_Set,2,"FA_DOSList_Path","Parent");
        F_Do(LOD->match,FM_Notify,FA_Selected,FV_Notify_Always,Obj,FM_Set,2,F_IDA(FA_FileChooser_FilterFiles),FV_Notify_Value);
        F_Do(LOD->icons,FM_Notify,FA_Selected,FV_Notify_Always,Obj,FM_Set,2,F_IDA(FA_FileChooser_FilterIcons),FV_Notify_Toggle);

        F_Do(Obj,FM_Set,

            F_IDA(FA_FileChooser_Path), path,
            F_IDA(FA_FileChooser_File), file,

            TAG_DONE);

        return Obj;
    }
    return NULL;
}
//+
///FileChooser_Get
F_METHOD(void,FileChooser_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path:     F_STORE(F_Get(LOD->Path,(uint32) "Contents")); break;
        case FA_FileChooser_File:     F_STORE(F_Get(LOD->File,(uint32) "Contents")); break;
        case FA_FileChooser_Pattern:  F_STORE(F_Get(LOD->pattern,(uint32) "Contents")); break;
    }

    F_SUPERDO();
}
//+
///FileChooser_Set
F_METHOD(void,FileChooser_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)))
    switch (item.ti_Tag)
    {
        case FA_FileChooser_Path:
        {
//         F_Log(0,"path (0x%08lx)(%s)",item.ti_Data,item.ti_Data);

            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"Path",item.ti_Data,TAG_DONE);

            tag->ti_Data = F_Get(LOD->list,(uint32) "Path");

            F_Do(LOD->Path,FM_Set,FA_NoNotify,TRUE,"Contents",tag->ti_Data,TAG_DONE);

            filechooser_select_disabled(Class,Obj,FeelinBase);

//         F_Log(0,">> path (0x%08lx)(%s)",tag->ti_Data,tag->ti_Data);
        }
        break;

        case FA_FileChooser_File:
        {
//         F_Log(0,"file (0x%08lx)(%s)",item.ti_Data,item.ti_Data);

            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"File",item.ti_Data,TAG_DONE);

            tag->ti_Data = F_Get(LOD->list,(uint32) "File");

            F_Do(LOD->File,FM_Set,FA_NoNotify,TRUE,"Contents",tag->ti_Data,TAG_DONE);

//         F_Log(0,">> file (0x%08lx)(%s)",tag->ti_Data,tag->ti_Data);
        }
        break;

        case FA_FileChooser_FilterIcons:
        {
            F_Do(LOD->icons,FM_Set,FA_NoNotify,TRUE,FA_Selected,!item.ti_Data,TAG_DONE);
            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"FilterIcons",item.ti_Data,TAG_DONE);
        }
        break;

        case FA_FileChooser_FilterFiles:
        {
            F_Do(Obj,FM_MultiSet, FA_Hidden, FF_Hidden_Check | (item.ti_Data ? FALSE : TRUE),LOD->balance,LOD->pattern,NULL);
            F_Do(LOD->match,FM_Set,FA_NoNotify,TRUE,FA_Selected,item.ti_Data,TAG_DONE);
            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"FilterFiles",item.ti_Data,TAG_DONE);
        }
        break;

        case FA_FileChooser_Pattern:
        {
            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"Pattern",item.ti_Data,TAG_DONE);

            tag->ti_Data = F_Get(LOD->list,(uint32) "Pattern");

            F_Do(LOD->pattern,FM_Set,FA_NoNotify,TRUE,"Contents",tag->ti_Data,TAG_DONE);
        }
        break;
/*
        case FA_FileChooser_Choosed:
        {
            F_Log(0,"choosed");
        }
        break;
*/
    }

    F_SUPERDO();
}
//+
///FileChooser_Update
F_METHODM(void,FileChooser_Update,FS_FileChooser_Update)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    switch (Msg->From)
    {
        case FV_FROM_PATH:
        {
            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"Path",Msg->Value,TAG_DONE);
            F_Do(LOD->Path,FM_Set,FA_NoNotify,TRUE,"Contents",F_Get(LOD->list,(uint32) "Path"),TAG_DONE);
        }
        break;

        case FV_FROM_FILE:
        {
            uint32 prev = F_Get(LOD->list,(uint32) "Path");

            F_Do(LOD->list,FM_Set,FA_NoNotify,TRUE,"File",Msg->Value,TAG_DONE);
            F_Do(LOD->File,FM_Set,FA_NoNotify,TRUE,"Contents",F_Get(LOD->list,(uint32) "File"),TAG_DONE);

            if (prev == F_Get(LOD->list,(uint32) "Path"))
            {
                F_Set(Obj,F_IDA(FA_FileChooser_Choosed),TRUE);
            }
        }
        break;

        case FV_FROM_LIST_PATH:
        {
            F_Do(LOD->Path,FM_Set,FA_NoNotify,TRUE,"Contents",Msg->Value,TAG_DONE);
        }
        break;

        case FV_FROM_LIST_FILE:
        {
            F_Do(LOD->File,FM_Set,FA_NoNotify,TRUE,"Contents",Msg->Value,TAG_DONE);
        }
        break;
    }

    if (Msg->From == FV_FROM_PATH || Msg->From == FV_FROM_LIST_PATH)
    {
        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_FileChooser_Path),F_Get(LOD->list,(uint32) "Path"),TAG_DONE);
    }
    else
    {
        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_FileChooser_File),F_Get(LOD->list,(uint32) "File"),TAG_DONE);
    }

    filechooser_select_disabled(Class,Obj,FeelinBase);
}
//+
///FileChooser_Activate
F_METHOD(void,FileChooser_Activate)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(LOD->File,FM_Set, FA_NoNotify,TRUE, (uint32) "Contents",F_Get(LOD->list,(uint32) "FA_DOSList_File"),TAG_DONE);
}
//+

