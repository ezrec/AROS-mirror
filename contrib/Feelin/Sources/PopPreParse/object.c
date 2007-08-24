#include "Private.h"

struct FS_HOOK_OPEN                             { FClass *Class; };
struct FS_HOOK_RESPONSE                         { FClass *Class; int32 Response; };
struct FS_HOOK_CHANGED                          { FClass *Class; STRPTR Spec; };

/****************************************************************************
**** Private ****************************************************************
****************************************************************************/

///code_response
STATIC F_HOOKM(void,code_response,FS_HOOK_RESPONSE)
{
    struct FeelinClass *Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Response == FV_Dialog_Response_Ok)
    {
        F_Set(Obj,F_IDA(FA_PopPreParse_Contents),F_Get(LOD -> Adjust,(ULONG) "Spec"));
    }

    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;
    LOD -> Adjust = NULL;
}
//+
///code_changed
STATIC F_HOOKM(void,code_changed,FS_HOOK_CHANGED)
{
    struct FeelinClass *Class = Msg -> Class;
 
    F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_PopPreParse_Contents),Msg -> Spec,TAG_DONE);
}
//+
///code_open
STATIC F_HOOKM(void,code_open,FS_HOOK_OPEN)
{
    struct FeelinClass * Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (!LOD -> Dialog)
    {
        LOD -> Dialog = DialogObject,
            
            FA_Window_Title,     F_CAT(TITLE),
            FA_Window_Open,      TRUE,
            FA_Width,            "60%",
            FA_Height,           "50%",

          "FA_Dialog_Buttons",     FV_Dialog_Buttons_Boolean,
          "FA_Dialog_Separator",   TRUE,
          
            End;
            
        if (LOD -> Dialog)
        {
            LOD -> Adjust = AdjustPreParseObject,

                "Spec", F_Get(LOD -> String,(ULONG) "Contents"),

            End;
        
            if (LOD -> Adjust)
            {
                if (F_Do((FObject) F_Get(LOD -> Dialog,FA_Child),FM_AddMember,LOD -> Adjust,FV_AddMember_Head))
                {
                    if (F_Do(_app,FM_AddMember,LOD -> Dialog,FV_AddMember_Tail))
                    {
                        F_Do(LOD -> Dialog,FM_Notify,"FA_Dialog_Response",FV_Notify_Always,FV_Notify_Application,FM_Application_PushMethod,6,Obj,FM_CallHookEntry,3,code_response,Class,FV_Notify_Value);
                        
                        return;
                    }
                    else
                    {
                        F_DisposeObj(LOD -> Dialog); LOD -> Dialog = NULL;
                    }
                }
                F_DisposeObj(LOD -> Adjust); LOD -> Adjust = NULL;
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

///PopPreParse_New
F_METHOD(APTR,PopPreParse_New)
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
          "FA_Image_Spec",  "$poppreparse-image",
            FA_SetMax,        FV_SetWidth,
            FA_InputMode,     FV_InputMode_Release,
            FA_ChainToCycle,  FALSE,
            End,
            
        FA_ContextHelp, F_CAT(HELP),

        TAG_MORE,Msg))
    {
        F_Do(LOD -> String,FM_Notify,"FA_String_Changed",FV_Notify_Always,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_changed),Class,FV_Notify_Value);
        F_Do(pop,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_open),Class,0);

        return Obj;
    }
    return NULL;
}
//+
///PopPreParse_Dispose
F_METHOD(void,PopPreParse_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;

    F_SUPERDO();
}
//+
///PopPreParse_Set
F_METHOD(void,PopPreParse_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,*tag,item;

    while  ((tag = F_DynamicNTI(&Tags,&item,Class)) != NULL)
    switch (item.ti_Tag)
    {
        case FA_PopPreParse_Contents:
        {
            F_Do(LOD -> String,FM_Set,FA_NoNotify,TRUE,"Contents",item.ti_Data,TAG_DONE);
            
            tag -> ti_Data = F_Get(LOD -> String,(ULONG) "Contents");
        }
        break;
    }

    F_SUPERDO();
}
//+
///PopPreParse_Get
F_METHOD(void,PopPreParse_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PopPreParse_Contents:
        {
            F_STORE(F_Get(LOD -> String,(ULONG) "Contents"));
        }
        break;
    }

    F_SUPERDO();
}
//+

///PopPreParse_DnDQuery
F_METHODM(FObject,PopPreParse_DnDQuery,FS_DnDQuery)
{
    if (F_SUPERDO())
    {
        STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_PopPreParse_Contents));
        
        if (spec)
        {
            return Obj;
        }
    }
    return NULL;
}
//+
///PopPreParse_DnDDrop
F_METHODM(void,PopPreParse_DnDDrop,FS_DnDDrop)
{
    STRPTR spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_PopPreParse_Contents));

    if (spec)
    {
        F_Set(Obj,F_IDA(FA_PopPreParse_Contents),(ULONG)(spec));
    }
}
//+

