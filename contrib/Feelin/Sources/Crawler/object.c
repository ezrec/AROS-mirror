#include "Private.h"

///Crawler_New
F_METHOD_NEW(Crawler_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    LOD -> delay         = FV_DEF_DELAY;
    LOD -> pos           = -1;
    LOD -> sh.Object     = Obj;
    LOD -> sh.Method     = F_IDM(FM_Crawler_Crawl);
    LOD -> sh.Flags      = FF_SignalHandler_Timer;
    LOD -> sh.fsh_Secs   = 0;
    LOD -> sh.fsh_Micros = FV_DEF_MICROS;

    LOD -> p_Delay       = "$crawler-delay";
    LOD -> p_Micros      = "$crawler-micros";

    F_AREA_SAVE_PUBLIC;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Crawler_Delay:  LOD -> p_Delay  = (STRPTR)(item.ti_Data); break;
        case FA_Crawler_Micros: LOD -> p_Micros = (STRPTR)(item.ti_Data); break;
    }

    return (FObject) F_SuperDo(Class,Obj,Method,

        FA_ChainToCycle,  FALSE,

    TAG_MORE, Msg);
}
//+
///Crawler_Set
F_METHOD_SET(Crawler_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Crawler_Delay:
        {
            LOD -> delay = item.ti_Data;
            LOD -> saved = LOD -> delay;
        }
        break;
    }

    F_SUPERDO();
}
//+
