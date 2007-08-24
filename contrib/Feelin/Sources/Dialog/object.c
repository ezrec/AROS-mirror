#include "Private.h"

struct FS_HOOK_RESPONSE                         { FClass *Class; uint32 Response; };

/****************************************************************************
**** Private ****************************************************************
****************************************************************************/

///code_response
STATIC F_HOOKM(void,code_response,FS_HOOK_RESPONSE)
{
    struct FeelinClass *Class = Msg -> Class;

    F_Set(Obj,FA_Window_Open,FALSE);
 
    F_Do((FObject) F_Get(Obj,FA_Application),FM_Application_PushMethod,Obj,FM_Set,2,F_IDA(FA_Dialog_Response),Msg -> Response);
}
//+

/****************************************************************************
**** Methods ****************************************************************
****************************************************************************/

///Dialog_New
F_METHOD(FObject,Dialog_New)
{
    struct TagItem *Tags = Msg,item;

    uint32 buttons=FV_Dialog_Buttons_None;
    BOOL separator=FALSE;
    FObject grp,ok=NULL,cancel=NULL,apply=NULL,yes=NULL,no=NULL,all=NULL,save=NULL,use=NULL;
    STRPTR applytext=F_CAT(APPLY);

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Dialog_Buttons:    buttons = item.ti_Data; break;
        case FA_Dialog_Separator:  separator = item.ti_Data; break;
        case FA_Dialog_ApplyText:  applytext = (STRPTR) item.ti_Data; break;
    }

    switch (buttons)
    {
        case FV_Dialog_Buttons_Ok:
        {
            grp = HGroup,
                Child, ok = SimpleButton(F_CAT(OK)),
            End;
        }
        break;
        
        case FV_Dialog_Buttons_Boolean:
        {
            grp = HGroup,
                Child, ok = SimpleButton(F_CAT(OK)),
                Child, cancel = SimpleButton(F_CAT(CANCEL)),
            End;
        }
        break;
          
        case FV_Dialog_Buttons_Confirm:
        {     
            grp = HGroup,
                Child, ok = SimpleButton(F_CAT(OK)),
                Child, apply = SimpleButton(applytext),
                Child, cancel = SimpleButton(F_CAT(CANCEL)),
            End;
        }
        break;

        case FV_Dialog_Buttons_Always:
        {
            grp = HGroup,
                Child, yes = SimpleButton(F_CAT(YES)),
                Child, all = SimpleButton(F_CAT(ALL)),
                Child, no = SimpleButton(F_CAT(NO)),
                Child, cancel = SimpleButton(F_CAT(CANCEL)),
            End;
        }
        break;
    
        case FV_Dialog_Buttons_Preference:
        {
            grp = HGroup,
                Child, save = SimpleButton(F_CAT(SAVE)),
                Child, use = SimpleButton(F_CAT(USE)),
                Child, cancel = SimpleButton(F_CAT(CANCEL)),
            End;
        }
        break;
    
        case FV_Dialog_Buttons_PreferenceTest:
        {
            grp = HGroup, FA_SetMax,FV_SetHeight,
                Child, save = SimpleButton(F_CAT(SAVE)),
                Child, use = SimpleButton(F_CAT(USE)),
                Child, apply = SimpleButton(applytext),
                Child, BarObject, End,
                Child, cancel = SimpleButton(F_CAT(CANCEL)),
            End;
        }
        break;

        default:
        {
            grp = (FObject) -1;
        }
        break;
    }

    if (F_SuperDo(Class,Obj,Method,
        
        FA_Window_Open, TRUE,
        
        Child, VGroup,
            (separator) ? FA_Child : TAG_IGNORE, (separator) ? BarObject, End : NULL,
            ((uint32)(grp) == -1) ? TAG_IGNORE : FA_Child, grp,
        End,

        TAG_MORE,Msg))
    {
        F_Do(ok,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Ok);
        F_Do(cancel,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Cancel);
        F_Do(apply,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Apply);
        F_Do(yes,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Yes);
        F_Do(no,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_No);
        F_Do(all,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_All);
        F_Do(save,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Save);
        F_Do(use,FM_Notify,FA_Pressed,FALSE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Use);

        return Obj;
    }
    return NULL;
}
//+
///Dialog_Open
F_METHOD(uint32,Dialog_Open)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    uint32 rc = F_SUPERDO();
  
    if (rc)
    {
        LOD -> CloseRequest_NH = (APTR) F_Do(Obj,FM_Notify,FA_Window_CloseRequest,TRUE,Obj,FM_CallHookEntry,3,F_FUNCTION_GATE(code_response),Class,FV_Dialog_Response_Cancel);
    }
    return rc;
}
//+
///Dialog_Close
F_METHOD(uint32,Dialog_Close)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> CloseRequest_NH)
    {
        F_Do(Obj,FM_UnNotify,LOD -> CloseRequest_NH); LOD -> CloseRequest_NH = NULL;
    }
    return F_SUPERDO();
}
//+

