#include "Private.h"

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

struct FS_NOTIFY                                { FClass *Class; uint32 Type; uint32 Value; };

enum  {

        FV_NOTIFY_LIST_ENTRIES  = 100,
        FV_NOTIFY_LIST_FIRST,
        FV_NOTIFY_LIST_VISIBLE,
        FV_NOTIFY_PROP_FIRST

        };

///code_notify
F_HOOKM(void,code_notify,FS_NOTIFY)
{
    struct LocalObjectData *LOD = F_LOD(Msg -> Class,Obj);

    switch (Msg -> Type)
    {
        case FV_NOTIFY_LIST_ENTRIES:
        {
//         F_Log(0,"FV_NOTIFY_LIST_ENTRIES %ld",Msg -> Value);

            F_Set(LOD -> vert,(uint32) "Entries",Msg -> Value);
        }
        break;

        case FV_NOTIFY_LIST_FIRST:
        {
//         F_Log(0,"FV_NOTIFY_LIST_FIRST %ld >> BEGIN",Msg -> Value);
            F_Do(LOD -> vert,FM_Set,

                FA_NoNotify, TRUE,
                "First", Msg -> Value,
                
                TAG_DONE);
//         F_Log(0,"FV_NOTIFY_LIST_FIRST %ld >> END",Msg -> Value);
        }
        break;

        case FV_NOTIFY_LIST_VISIBLE:
        {
//         F_Log(0,"FV_NOTIFY_LIST_VISIBLE %ld",Msg -> Value);

            F_Set(LOD -> vert,(uint32) "Visible",Msg -> Value);

        }
        break;

        case FV_NOTIFY_PROP_FIRST:
        {
//         F_Log(0,"FV_NOTIFY_PROP_FIRST %ld",Msg -> Value);

            F_Do(LOD -> list,FM_Set,
                
                FA_NoNotify, TRUE,
                "First", Msg -> Value,
                
                TAG_DONE);
        }
        break;
    }
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///LV_New
F_METHOD(FObject,LV_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_AREA_SAVE_PUBLIC;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Listview_List:  LOD -> list = (FObject)(item.ti_Data); break;
    }

    if (LOD -> list)
    {
        if (F_SuperDo(Class,Obj,Method,

            FA_Horizontal,       TRUE,
            FA_NoFill,           TRUE,
            FA_ChainToCycle,     TRUE,
            FA_Group_HSpacing,   0,
            
            Child, LOD -> list,

            Child, LOD -> vert = ScrollbarObject,
                FA_ChainToCycle,  FALSE,
                FA_Horizontal,    FALSE,
                FA_SetMax,        FV_SetWidth,
              "Entries",         F_Get(LOD -> list,(uint32) "FA_List_Entries"),
                End,

            TAG_MORE,Msg))
        {
            F_Do(LOD -> vert,FM_Notify,"First",FV_Notify_Always,
                  Obj,
                  FM_CallHookEntry,4,
                  F_FUNCTION_GATE(code_notify), Class,FV_NOTIFY_PROP_FIRST, FV_Notify_Value);

            /* these notifications update the prop according to the list */

            F_Do(LOD -> list,FM_Notify,"FA_List_Entries",FV_Notify_Always,
                  Obj,
                  FM_CallHookEntry,4,
                  F_FUNCTION_GATE(code_notify), Class,FV_NOTIFY_LIST_ENTRIES,FV_Notify_Value);

            F_Do(LOD -> list,FM_Notify,"FA_List_First",FV_Notify_Always,
                  Obj,
                  FM_CallHookEntry,4,
                  F_FUNCTION_GATE(code_notify), Class,FV_NOTIFY_LIST_FIRST,FV_Notify_Value);

            F_Do(LOD -> list,FM_Notify,"FA_List_Visible",FV_Notify_Always,
                  Obj,
                  FM_CallHookEntry,4,
                  F_FUNCTION_GATE(code_notify), Class,FV_NOTIFY_LIST_VISIBLE,FV_Notify_Value);

            return Obj;
        }
    }
    return NULL;
}
//+
///LV_GoActive
F_METHOD(void,LV_GoActive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(LOD -> list,Method,Msg);

    F_SUPERDO();
}
//+
///LV_GoInactive
F_METHOD(void,LV_GoInactive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(LOD -> list,Method,Msg);

    F_SUPERDO();
}
//+

