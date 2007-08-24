#include "Private.h"

///Radio_New
F_METHOD(FObject,Radio_New)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,item;
    UWORD                   i;
    BOOL                    objects = FALSE;
    STRPTR                 *entries = NULL;

    LOD -> p_PreParse = "$radio-preparse";

    /* parse initial taglist */

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Radio_Entries:     entries = (STRPTR *)(item.ti_Data); break;
        case FA_Radio_PreParse:    LOD -> p_PreParse = (STRPTR)(item.ti_Data); break;
        case FA_Radio_EntriesType: objects = (item.ti_Data == FV_Radio_EntriesType_Objects) ? TRUE : FALSE; break;
    }

    if (!entries)
    {
        F_Log(FV_LOG_DEV,"Entries is NULL");
        return NULL;
    }

    /* Count the number of entries */

    for (LOD -> Num = 0 ; entries[LOD -> Num] ; LOD -> Num++);
        
    LOD -> Couples = F_New(LOD -> Num * sizeof (struct FeelinRadio_Couple));
 
    if (LOD->Couples)
    {
        Obj = (FObject) F_SuperDo(Class,Obj,Method,

            FA_ChainToCycle,     FALSE,
            FA_Group_HSpacing,  "$radio-spacing-horizontal",
            FA_Group_VSpacing,  "$radio-spacing-vertical",

            TAG_MORE, Msg);
        
        if (Obj)
        {
            for (i = 0 ; i < LOD -> Num ; i++)
            {
                BOOL state = (LOD -> Active == i) ? TRUE : FALSE;
 
                LOD -> Couples[i].Image = ImageObject,
                    FA_InputMode,        FV_InputMode_Immediate,
                    FA_Selected,         state,
                    FA_SetMax,           TRUE,
                    FA_Back,             "$radio-back",
//               FA_Frame,            "$radio-Frame",
                    FA_ColorScheme,      "$radio-scheme",
                  "FA_Image_Spec",      "$radio-image",
                    End;

                if (F_Do(Obj,FM_AddMember,LOD -> Couples[i].Image,FV_AddMember_Tail))
                {
                    if (objects)
                    {
                        LOD -> Couples[i].Text = entries[i];
                    }
                    else
                    {
                        LOD -> Couples[i].Text = TextObject,
                            FA_InputMode,        FV_InputMode_Immediate,
                            FA_SetMax,           FV_SetHeight,
                            FA_ChainToCycle,     FALSE,
                            FA_Text,             entries[i],
                            FA_Text_PreParse,    LOD -> p_PreParse,
                            End;
                    }
                
                    if (F_Do(Obj,FM_AddMember,LOD -> Couples[i].Text,FV_AddMember_Tail))
                    {
                        F_Do(LOD -> Couples[i].Image,FM_Notify, FA_Selected,FV_Notify_Always,
                            Obj,FM_Set,2,F_IDA(FA_Radio_Active),i);

                        if (!objects)
                        {
                            F_Do(LOD -> Couples[i].Text,FM_Notify, FA_Selected,FV_Notify_Always,
                                Obj,FM_Set,2,F_IDA(FA_Radio_Active),i);
                        }
                    }
                    else return NULL;
                }
                else return NULL;
            }

            return Obj;
        }
    }
    return NULL;
}
//+
///Radio_Dispose
F_METHOD(void,Radio_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Dispose(LOD -> Couples); LOD -> Couples = NULL;

    F_SUPERDO();
}
//+
///Radio_Set
F_METHOD(void,Radio_Set)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem         *Tags = Msg,
                                    item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Radio_Active:
        {
//         F_DebugOut(FV_LOG_DEV,"%s{%08lx}.Set() - FA_Radio_Active %ld (%ld)\n",_classname(Obj),Obj,item.ti_Data,LOD -> Active);

            if (item.ti_Data >= 0 &&
                 item.ti_Data  < LOD -> Num &&
                 item.ti_Data != LOD -> Active)
            {
                F_Do(LOD -> Couples[LOD -> Active].Image,
                      FM_Set,FA_NoNotify,TRUE, FA_Selected, FALSE, TAG_DONE);

                LOD -> Active = item.ti_Data;

                F_Do(LOD -> Couples[LOD -> Active].Image,
                      FM_Set,FA_NoNotify,TRUE, FA_Selected, TRUE, TAG_DONE);
            }
        }
        break;
    }

    F_SUPERDO();
}
//+
///Radio_Get
F_METHOD(void,Radio_Get)
{
    struct LocalObjectData *LOD  = F_LOD(Class, Obj);
    struct TagItem         *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Radio_Active:   F_STORE(LOD -> Active); break;
    }

    F_SUPERDO();
}
//+

