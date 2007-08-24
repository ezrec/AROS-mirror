#include "Private.h"

/*** Methods ***************************************************************/

///Group_Setup
F_METHOD(int32,Group_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        FAreaNode *node;

        LOD -> HSpace = F_Do(_app,FM_Application_ResolveInt,LOD -> p_HSpace,5);
        LOD -> VSpace = F_Do(_app,FM_Application_ResolveInt,LOD -> p_VSpace,5);

        for _each
        {
            if (!F_OBJDO(node -> Object))
            {
                F_Log(FV_LOG_USER,F_CAT(ER_SETUP),_classname(node -> Object),node -> Object);

                return FALSE;
            }
        }

        if (LOD -> PageData)
        {
            LOD -> PageData -> IPrep = (STRPTR)              F_Do(_app,FM_Application_Resolve,LOD -> PageData -> p_IPrep, DEF_PAGE_PREPARSE);
            LOD -> PageData -> APrep = (STRPTR)              F_Do(_app,FM_Application_Resolve,LOD -> PageData -> p_APrep, DEF_PAGE_ALTPREPARSE);
            LOD -> PageData -> Font  = (struct TextFont *)   F_Do(_app,FM_Application_OpenFont,Obj,LOD -> PageData -> p_Font);

            LOD -> PageData -> TD = TextDisplayObject,
                FA_TextDisplay_Font,      LOD -> PageData -> Font,
                FA_TextDisplay_Shortcut,  FALSE,
            End;

            F_Do(LOD -> PageData -> TD,FM_TextDisplay_Setup,_render);

            F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

            LOD -> PageData -> UpdateNode = NULL;

            if (!LOD -> PageData -> ActiveNode)
            {
                LOD -> PageData -> ActiveNode = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);
            }
        }
        return TRUE;
    }
    return FALSE;
}
//+
///Group_Cleanup
F_METHOD(void,Group_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> PageData)
    {
        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);

        F_Do(LOD -> PageData -> TD,FM_TextDisplay_Cleanup);
        F_DisposeObj(LOD -> PageData -> TD); LOD -> PageData -> TD = NULL;

        if (LOD -> PageData -> Font)
        {
            CloseFont(LOD -> PageData -> Font); LOD -> PageData -> Font = NULL;
        }
    }

    Group_DoA(Class,Obj,FM_Cleanup,Msg);

    F_SUPERDO();
}
//+
///Group_Show
F_METHOD(int32,Group_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *node;

    F_SUPERDO();

    if (LOD -> PageData)
    {
        if ((node = LOD -> PageData -> ActiveNode) != NULL)
        {
            F_Do(node -> Object,FM_Show);
        }
    }
    else
    {
        for _each
        {
            if (!F_Get(node -> Object,FA_Hidden))
            {
                F_Do(node -> Object,FM_Show);
            }
        }
    }
    return TRUE;
}
//+
///Group_Hide
F_METHOD(void,Group_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *node;

    if (LOD -> PageData)
    {
        if ((node = LOD -> PageData -> ActiveNode) != NULL)
        {
            F_Do(node -> Object,FM_Hide);
        }
    }
    else
    {
        for _each
        {
            if (!F_Get(node -> Object,FA_Hidden))
            {
                F_Do(node -> Object,FM_Hide);
            }
        }
    }
    F_SUPERDO();
}
//+
///Group_HandleEvent
F_METHODM(bits32,Group_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;

    if (!LOD -> PageData) return 0;

    if (fev -> Key)
    {
        uint32 page;

        switch (fev -> Key)
        {
            case FV_KEY_LEFT:    page = FV_Group_ActivePage_Prev;  break;
            case FV_KEY_RIGHT:   page = FV_Group_ActivePage_Next;  break;
            case FV_KEY_LAST:    page = FV_Group_ActivePage_Last;  break;
            case FV_KEY_FIRST:   page = FV_Group_ActivePage_First; break;
            default:             return 0;
        }

        F_Set(Obj,FA_Group_ActivePage,page);

        return FF_HandleEvent_Eat;
    }
    else if (fev -> Class == FF_EVENT_BUTTON)
    {
        int16 x = _x;
        int16 y = _y;

        if (fev -> MouseY >= y &&
             fev -> MouseX >= x &&
             fev -> MouseY <= y + 5 + LOD -> PageData -> Font -> tf_YSize &&
             fev -> MouseX <= x + _w - 1)
        {
            if ((fev -> Code == FV_EVENT_BUTTON_SELECT) && (FF_EVENT_BUTTON_DOWN & fev -> Flags))
            {
                if (F_Get(LOD -> Family,FA_Family_Head))
                {
                    FAreaNode *node;

                    uint16 i = 0,pw;

                    for _each i++;

                    pw = (_w + 1 - i) / i; i = 0;

                    for _each
                    {
                        if (node == LOD -> PageData -> ActiveNode)
                        {
                            if (fev -> MouseX >= x + 2 && fev -> MouseX <= x + pw - 3)
                            {
                                F_Set(Obj,FA_Group_ActivePage,i); break;
                            }
                        }
                        else
                        {
                            if (fev -> MouseX >= x + 4 && fev -> MouseX <= x + pw - 5 && fev -> MouseY >= y + 2 && fev -> MouseY <= y + _h - 1)
                            {
                                F_Set(Obj,FA_Group_ActivePage,i); break;
                            }
                        }
                        x += pw - 1; i++;
                    }
                }
                return FF_HandleEvent_Eat;
            }
            else if (fev -> Code == FV_EVENT_BUTTON_WHEEL)
            {
                F_Set(Obj,FA_Group_ActivePage,(FF_EVENT_BUTTON_DOWN & fev -> Flags) ? FV_Group_ActivePage_Next : FV_Group_ActivePage_Prev);
                
                return FF_HandleEvent_Eat;
            }
        }
    }
    return 0;
}
//+
///Group_GoActive
F_METHOD(void,Group_GoActive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> PageData)
    {
        F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);
    }
    F_SUPERDO();
}
//+
///Group_GoInactive
F_METHOD(void,Group_GoInactive)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> PageData)
    {
        F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);
    }
    F_SUPERDO();
}
//+

#if F_CODE_DEPRECATED
///Group_RethinkLayout
F_METHODM(bits32,Group_RethinkLayout,FS_RethinkLayout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    switch (Msg -> Operation)
    {
        case FV_RethinkLayout_Save:
        {
            _sub_declare_all;

            for _each
            {
                F_OBJDO(node -> Object);
            }

            F_SUPERDO();
        }
        break;

        case FV_RethinkLayout_Compare:
        {
            bits32 rc = F_SUPERDO();

            if (FF_RethinkLayout_Modified & rc)
            {
                return rc;
            }
            else
            {
                _sub_declare_all;

                LOD -> Flags |= FF_GROUP_RETHINKING;

                for _each
                {
                    if (F_OBJDO(node -> Object))
                    {
                        if (LOD -> Region)
                        {
                            DisposeRegion(LOD -> Region); LOD -> Region = NULL;
                        }
 
                        F_Draw(Obj,FF_Draw_Object); break;
                    }
                }

                LOD -> Flags &= ~FF_GROUP_RETHINKING;
            }
        }
        break;
    }

    return 0;
}
//+
#endif

