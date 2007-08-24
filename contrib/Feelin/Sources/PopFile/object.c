#include "Private.h"

struct FS_HOOK_OPEN                             { FClass *Class; };
struct FS_HOOK_RESPONSE                         { FClass *Class; ULONG Response; };

/****************************************************************************
**** Private ****************************************************************
****************************************************************************/

///code_response
F_HOOKM(void,code_response,FS_HOOK_RESPONSE)
{
    struct FeelinClass *Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Response == FV_Dialog_Response_Ok)
    {
        STRPTR spec = F_New(1024);

        AddPart(spec,(STRPTR) F_Get(LOD -> Chooser,(ULONG) "FA_FileChooser_Path"),1024);
        AddPart(spec,(STRPTR) F_Get(LOD -> Chooser,(ULONG) "FA_FileChooser_File"),1024);
 
        F_Set(Obj,F_IDA(FA_PopFile_Contents),(ULONG)(spec));

        F_Dispose(spec);
    }

    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;
    LOD -> Chooser = NULL;
}
//+
///code_choosed
F_HOOKM(void,code_choosed,FS_HOOK_RESPONSE)
{
    struct LocalObjectData *LOD = F_LOD(Msg -> Class,Obj);
    
    F_Set(LOD -> Chooser,FA_Window_Open,FALSE);
    F_Do(_app,FM_Application_PushMethod,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Msg -> Class,FV_Dialog_Response_Ok);
}
//+
///code_open
F_HOOKM(void,code_open,FS_HOOK_OPEN)
{
    struct FeelinClass * Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (!LOD -> Dialog)
    {
        LOD -> Dialog = DialogObject,

            FA_Window_Title,        F_CAT(TITLE),
            FA_Window_Open,         TRUE,
            FA_Width,               "40%",
            FA_Height,              "70%",

          "FA_Dialog_Buttons",     FV_Dialog_Buttons_Boolean,
          "FA_Dialog_Separator",   TRUE,
        
        End;
        
        if (LOD -> Dialog)
        {
            LOD -> Chooser = F_NewObj("FileChooser",
                        
                "FA_FileChooser_Path", NULL,
                "FA_FileChooser_File", F_Get(LOD -> String,(ULONG) "Contents"),
                    
            End;
            
            if (LOD -> Chooser)
            {
                if (F_Do((FObject) F_Get(LOD -> Dialog,FA_Child),FM_AddMember,LOD -> Chooser,FV_AddMember_Head) &&
                     F_Do(_app,FM_AddMember,LOD -> Dialog,FV_AddMember_Head))
                {
                    F_Do(LOD -> Chooser,FM_Notify,"Choosed",TRUE,FV_Notify_Application,FM_Application_PushMethod,6,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_choosed),Class,FV_Dialog_Response_Ok);
                    F_Do(LOD -> Dialog,FM_Notify,"FA_Dialog_Response",FV_Notify_Always,FV_Notify_Application,FM_Application_PushMethod,6,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Notify_Value);
                    
                    return;
                }
                F_DisposeObj(LOD -> Chooser); LOD -> Chooser = NULL;
            }
            F_DisposeObj(LOD -> Dialog); LOD -> Dialog = NULL;
        }
    }
    else
    {
        F_Set(LOD -> Dialog,FA_Window_Open,TRUE);
    }
}
//+

/****************************************************************************
**** Methods ****************************************************************
****************************************************************************/

///PopFile_New
F_METHOD(APTR,PopFile_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FObject pop;

    F_AREA_SAVE_PUBLIC;

    if (F_SuperDo(Class,Obj,Method,

        FA_Horizontal,       TRUE,
        FA_SetMax,           FV_SetHeight,
        FA_Group_HSpacing,   0,

        FA_Child, LOD -> String = StringObject, FA_SetMax,FV_SetNone, End,

        FA_Child, pop = ImageObject,
            FA_Back,         "$popstring-back",
            FA_Frame,        "$popstring-frame",
            FA_ColorScheme,  "$popstring-scheme",
           "FA_Image_Spec",  "$popfile-image",
            FA_SetMax,        FV_SetWidth,
            FA_InputMode,     FV_InputMode_Release,
            FA_ChainToCycle,  FALSE,
            End,

        TAG_MORE,Msg))
    {
        F_Do(LOD -> String, FM_Notify,
            "Changed", FV_Notify_Always, Obj, FM_Set, 2,
            F_IDA(FA_PopFile_Contents), FV_Notify_Value);
 
        F_Do(pop, FM_Notify,
            FA_Pressed, FALSE, Obj, FM_CallHookEntry, 2,
            F_FUNCTION_GATE(code_open),Class);

        return Obj;
    }
    return NULL;
}
//+
///PopFile_Dispose
F_METHOD(void,PopFile_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;
    LOD -> Chooser = NULL;
    
    F_SUPERDO();
}
//+
///PopFile_Set
F_METHOD(void,PopFile_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    switch (item.ti_Tag)
    {
        case FA_PopFile_Contents:
        {
            F_Do(LOD -> String,FM_Set,FA_NoNotify,TRUE,"Contents",item.ti_Data,TAG_DONE);
            
            tag -> ti_Data = F_Get(LOD -> String,(uint32) "Contents");
        }
        break;
    }

    F_SUPERDO();
}
//+
///PopFile_Get
F_METHOD(void,PopFile_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PopFile_Contents:
        {
            F_STORE(F_Get(LOD -> String,(uint32) "Contents"));
        }
        break;
    }

    F_SUPERDO();
}
//+

///PopFile_DnDQuery
F_METHODM(FObject,PopFile_DnDQuery,FS_DnDQuery)
{
    if (F_SUPERDO())
    {
        if (F_Get(Msg -> Source,F_IDA(FA_PopFile_Contents)))
        {
            return Obj;
        }
    }
    return NULL;
}
//+
///PopFile_DnDDrop
F_METHODM(void,PopFile_DnDDrop,FS_DnDDrop)
{
    STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_PopFile_Contents));

    if (spec)
    {
        F_Set(Obj,F_IDA(FA_PopFile_Contents),(uint32)(spec));
    }
}
//+

