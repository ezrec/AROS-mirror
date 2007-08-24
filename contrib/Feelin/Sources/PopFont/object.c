#include "Private.h"

struct FS_HOOK_OPEN                             { FClass *Class; };
struct FS_HOOK_RESPONSE                         { FClass *Class; uint32 Response; };
struct FS_HOOK_CHANGED                          { FClass *Class; STRPTR String; };

  ///code_response
F_HOOKM(void,code_response,FS_HOOK_RESPONSE)
{
    struct FeelinClass *Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (Msg -> Response == FV_Dialog_Response_Ok)
    {
        F_Set(Obj,F_IDA(FA_PopFont_Contents),F_Get(LOD -> Dialog,(uint32) "Spec"));
    }

    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;
}
//+
///code_changed
F_HOOKM(void,code_changed,FS_HOOK_CHANGED)
{
    struct FeelinClass *Class = Msg -> Class;

    F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_PopFont_Contents),Msg -> String,TAG_DONE);
}
//+
///code_open
F_HOOKM(void,code_open,FS_HOOK_OPEN)
{
    struct FeelinClass * Class = Msg -> Class;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (!LOD -> Dialog)
    {
        LOD -> Dialog = FontDialogObject,

            "FA_FontDialog_Spec", F_Get(LOD -> String,(uint32) "FA_String_Contents"),

            End;

        if (LOD -> Dialog)
        {
            if (F_Do(_app,FM_AddMember,LOD -> Dialog,FV_AddMember_Head))
            {
               F_Do(LOD -> Dialog,FM_Notify,"FA_Dialog_Response",FV_Notify_Always,FV_Notify_Application,FM_Application_PushMethod,6,Obj,FM_CallHookEntry,3,code_response,Class,FV_Notify_Value);

                return;
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

/*** Methods ***************************************************************/

///PopFont_New
F_METHOD(APTR,PopFont_New)
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
          "FA_Image_Spec",  "$popfont-image",
            FA_SetMax,        FV_SetWidth,
            FA_InputMode,     FV_InputMode_Release,
            FA_ChainToCycle,  FALSE,
            End,

        FA_Dropable,      TRUE,

        FA_ContextHelp,   F_CAT(HELP),

        TAG_MORE,Msg))
    {
        F_Do(LOD -> String,FM_Notify,"Changed",FV_Notify_Always,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_changed),Class,FV_Notify_Value);
        F_Do(pop,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,2,F_FUNCTION_GATE(code_open),Class);

        return Obj;
    }
    return NULL;
}
//+
///PopFont_Dispose
F_METHOD(void,PopFont_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> Dialog);

    LOD -> Dialog = NULL;

    F_SUPERDO();
}
//+
///PopFont_Set
F_METHOD(void,PopFont_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PopFont_Contents:
        {
            F_Do(LOD -> String,FM_Set,
                                     FA_NoNotify,         TRUE,
                                    "FA_String_Contents", item.ti_Data,
                                     TAG_DONE);
        }
        break;

        case FA_Font:
        {
            F_Set(LOD -> String,item.ti_Tag,item.ti_Data);
        }
        break;
    }

    F_SUPERDO();
}
//+
///PopFont_Get
F_METHOD(void,PopFont_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_PopFont_Contents:
        {
            F_STORE(F_Get(LOD -> String,(uint32) "FA_String_Contents"));
        }
        break;
    }

    F_SUPERDO();
}
//+

///PopFont_DnDQuery
F_METHODM(FObject,PopFont_DnDQuery,FS_DnDQuery)
{
    if (F_SUPERDO())
    {
        if (F_Get(Msg -> Source,F_IDA(FA_PopFont_Contents)) ||
             F_Get(Msg -> Source,FA_Font))
        {
            return Obj;
        }
    }
    return NULL;
}
//+
///PopFont_DnDDrop
F_METHODM(void,PopFont_DnDDrop,FS_DnDDrop)
{
    STRPTR spec;
    struct TextFont *font;

    if ((spec = (STRPTR) F_Get(Msg -> Source,F_IDA(FA_PopFont_Contents))) != NULL)
    {
        F_Set(Obj,F_IDA(FA_PopFont_Contents),(uint32)(spec));
    }
    else if ((font = (struct TextFont *) F_Get(Msg -> Source,FA_Font)) != NULL)
    {
        uint32 len = F_StrLen(font -> tf_Message.mn_Node.ln_Name);

        if (len > 8)
        {
            if ((spec = F_New(len + 8)) != NULL)
            {
                CopyMem(font -> tf_Message.mn_Node.ln_Name,spec,len - 5);
                F_StrFmt(spec + len - 5,"/%ld",font -> tf_YSize);

                F_Set(Obj,F_IDA(FA_PopFont_Contents),(uint32)(spec));

                F_Dispose(spec);
            }
        }
    }
}
//+
