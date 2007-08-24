#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (F_SuperDo(Class,Obj,Method,

        "PreviewTitle", "Pen",
        "PreviewClass", "PreviewImage",
        "Separator",     FV_ImageDisplay_Separator,
        
        Child, VGroup,
            Child, LOD -> slider = SliderObject,
                FA_Horizontal, TRUE,
                FA_SetMax, FV_SetHeight,
                "Min", -128,
                "Max", 127,
                "Value", 0,
            End,
        End,

        TAG_MORE,Msg))
    {
        F_Do(LOD -> slider,FM_Notify,"Value",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),2,FV_Notify_Value,TRUE);

        return Obj;
    }

    return NULL;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
    if (Msg -> Spec)
    {
        int32 val;

/* FIXME: Remove compatibility code */
   
        if (Msg -> Spec[0] == 'p' && Msg -> Spec[1] == ':')
        {
            return TRUE;
        }
        else if (stcd_l(Msg -> Spec,&val))
        {
            return TRUE;
        }
    }
    return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
    if (Msg -> Notify)
    {
        uint32 rc;
        STRPTR spec;

        spec = F_StrNew(NULL,"%ld",Msg -> Spec);

        rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);

        F_Dispose(spec);

        return rc;
    }

    return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_Do(LOD -> slider,FM_Set,
    
        FA_NoNotify, TRUE,
        "Value", (Msg -> Spec) ? ((Msg -> Spec[0] == 'p' && Msg -> Spec[1]) ? atol(Msg -> Spec+2) : atol(Msg -> Spec)) : 0,
    
    TAG_DONE);
}
//+
