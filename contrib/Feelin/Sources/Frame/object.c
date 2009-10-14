#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Frame_New
F_METHOD(uint32,Frame_New)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    STRPTR frame   = NULL;
    STRPTR title   = NULL;
    STRPTR font    = "$frame-font";
    STRPTR prep    = "$frame-preparse";
    STRPTR pos     = "$frame-position";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Frame:             frame = (STRPTR)(item.ti_Data); break;
        case FA_Frame_Font:        font  = (STRPTR)(item.ti_Data); break;
        case FA_Frame_Title:       title = (STRPTR)(item.ti_Data); break;
        case FA_Frame_PreParse:    prep  = (STRPTR)(item.ti_Data); break;
        case FA_Frame_Position:    pos   = (STRPTR)(item.ti_Data); break;

        case FA_Frame_InnerLeft:   LOD -> UserInner.l = item.ti_Data; break;
        case FA_Frame_InnerTop:    LOD -> UserInner.t = item.ti_Data; break;
        case FA_Frame_InnerRight:  LOD -> UserInner.r = item.ti_Data; break;
        case FA_Frame_InnerBottom: LOD -> UserInner.b = item.ti_Data; break;

        case FA_Back:              LOD -> p_back = (STRPTR)(item.ti_Data); break;
    }

    LOD -> p_frame = frame;

    if (title)
    {
        LOD -> TitleData = F_New(sizeof (FFrameTitle));

        if (LOD->TitleData)
        {
            LOD -> TitleData -> Title = title;
            LOD -> TitleData -> p_Font = font;
            LOD -> TitleData -> p_Prep = prep;
            LOD -> TitleData -> p_Position = pos;

            return F_SUPERDO();
        }
        else return 0;
    }
    return F_SUPERDO();
}
//+
///Frame_Dispose
F_METHOD(void,Frame_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//    F_Log(0,"DISPOSE TITLE DATA >> BEGIN");
    F_Dispose(LOD -> TitleData); LOD -> TitleData = NULL;
//    F_Log(0,"DISPOSE TITLE DATA >> DONE");

//    F_Log(0,"SUPERDO >> BEGIN");
    F_SUPERDO();
//    F_Log(0,"SUPERDO >> DONE");
}
//+
///Frame_Get
F_METHOD(void,Frame_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Frame_PublicData: F_STORE(&LOD -> Public); break;
        case FA_Back: F_STORE(LOD -> Public.Back); break;
    }

    F_SUPERDO();
}
//+
///Frame_Set
F_METHOD(void,Frame_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Back:
        {
            if (LOD->Public.Back)
            {
                F_Set(LOD->Public.Back, FA_ImageDisplay_Spec, item.ti_Data);

                F_Draw(Obj, FF_Draw_Object);
            }
        }
        break;
    }

    F_SUPERDO();
}
//+
