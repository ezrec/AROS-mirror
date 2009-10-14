#include "Private.h"

/*** Methods ***************************************************************/

///Crawler_Setup
F_METHOD_SETUP(Crawler_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,FM_ModifyHandler,FF_EVENT_WINDOW,0);

    if (F_SuperDoA(Class -> Super,Obj,Method,Msg))
    {
        InitRastPort(&LOD -> OwnRPort);

        LOD -> delay         = F_Do(_app,FM_Application_ResolveInt,LOD -> p_Delay,FV_DEF_DELAY);
        LOD -> sh.fsh_Micros = F_Do(_app,FM_Application_ResolveInt,LOD -> p_Micros,FV_DEF_MICROS);

        LOD -> OwnRender = RenderObject,

            FA_Render_Friend,  Msg -> Render,
            FA_Render_RPort,   &LOD -> OwnRPort,

            End;

        if (LOD -> OwnRender)
        {
            FFamilyNode *node = (FFamilyNode *) F_Get(Obj,FA_Family_Head);

            if (node)
            {
                return F_Do(node -> Object,FM_Setup,LOD -> OwnRender);
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
//+
///Crawler_Cleanup
F_METHOD_CLEANUP(Crawler_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_WINDOW);

    if (LOD -> OwnRender)
    {
        FFamilyNode *node = (FFamilyNode *) F_Get(Obj,FA_Family_Head);

        if (node)
        {
            F_Do(node -> Object,FM_Cleanup,LOD -> OwnRender);
        }

        F_DisposeObj(LOD -> OwnRender); LOD -> OwnRender = NULL;
    }

    if (LOD -> OwnRPort.BitMap)
    {
        FreeBitMap(LOD -> OwnRPort.BitMap);
        LOD -> OwnRPort.BitMap = NULL;
    }

#ifdef __AROS__
    DeinitRastPort(&LOD -> OwnRPort);
#endif

    F_SuperDoA(Class -> Super,Obj,Method,Msg);
}
//+
///Crawler_Show
F_METHOD_SHOW(Crawler_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        FFamilyNode *node = (FFamilyNode *) F_Get(Obj,FA_Family_Head);
        F_Do(node -> Object,FM_Layout);

        if (!(FF_SignalHandler_Active & LOD -> sh.Flags))
        {
            F_Do(_app,FM_Application_AddSignalHandler,&LOD -> sh);
        }

        return TRUE;
    }
    return FALSE;
}
//+
///Crawler_Hide
F_METHOD_HIDE(Crawler_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        if (FF_SignalHandler_Active & LOD -> sh.Flags)
        {
            F_Do(_app,FM_Application_RemSignalHandler,&LOD -> sh);
        }
    }
    F_SUPERDO();
}
//+
///Crawler_AskMinMax
F_METHOD_ASKMINMAX(Crawler_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *node = (FAreaNode *) F_Get(Obj,FA_Family_Head);

    if (node)
    {
        _node_minw = 0; _node_maxw = FV_MAXMAX;
        _node_minh = 0; _node_maxh = FV_MAXMAX;

        F_Do(node -> Object,FM_AskMinMax);

        _maxw = _node_maxw;
        _maxh = _node_maxh;
    }

    F_SuperDoA(Class -> Super,Obj,Method,Msg);
}
//+
///Crawler_Layout
F_METHOD(void,Crawler_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *node;

    F_SUPERDO();

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if ((node = (FAreaNode *) F_Get(Obj,FA_Family_Head)))
    {
        UWORD bmpw;
        UWORD bmph;

        _node_x = 0; _node_w = MAX(_node_minw,MIN(_node_maxw,_w));
        _node_y = 0; _node_h = MAX(_node_minh,MIN(_node_maxh,_h));

        F_Do(node -> Object,FM_Layout);

        bmpw = _node_w;
        bmph = _node_h;

        if (bmpw != LOD -> bmpw || bmph != LOD -> bmph)
        {
            struct BitMap *bmp = LOD -> OwnRPort.BitMap;
            struct Screen *scr = (struct Screen *) F_Get(_display,(ULONG) "FA_Display_Screen");

            if (bmp)
            {
                FreeBitMap(bmp); LOD -> OwnRPort.BitMap = NULL;
            }

            LOD -> OwnRPort.BitMap = AllocBitMap(bmpw,bmph,scr -> RastPort.BitMap -> Depth,0,scr -> RastPort.BitMap);

            SetRast(&LOD -> OwnRPort,3);

            LOD -> bmpw = bmpw;
            LOD -> bmph = bmph;
        }
    }

    F_Do(Obj,FM_Unlock);
}
//+
///Crawler_Draw
F_METHODM(void,Crawler_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort        *rp = _rp;
    struct BitMap          *bmp = LOD -> OwnRPort.BitMap;
    int16  x = _ix;
    int16  y = _iy;
    uint16 w = _iw;
    uint16 h = _ih;
    uint16 bmpw = LOD -> bmpw;
    uint16 bmph = LOD -> bmph,pad=0,off;

    if (!bmp)
    {
        F_Log(FV_LOG_DEV,"BitMap is NULL");
        return;
    }

    if (bmpw > w)
    {
        pad = (bmpw - w) / 2;
    }

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if (FF_Draw_Object & Msg -> Flags)
    {
        F_SUPERDO();
    }
    else if (LOD -> delay)
    {
        LOD -> delay--;

        F_Do(Obj,FM_Unlock);

        return;
    }

    if (_h < bmph)
    {
        LOD -> pos++;

        if (LOD -> pos + h >= bmph)
        {
            off = bmph - LOD -> pos - 1;

            BltBitMapRastPort(bmp,pad,LOD->pos, rp,x,y, w, off, 0x0C0);
            BltBitMapRastPort(bmp,pad,0, rp,x,y+off, w, h-off, 0x0C0);
        }
        else
        {
            BltBitMapRastPort(bmp,pad,LOD -> pos, rp,x,y,w,h, 0x0C0);
        }

        if (LOD -> pos + 2 == bmph) LOD -> pos = -1;
    }
    else
    {
        BltBitMapRastPort(bmp,pad,0, rp,x,y,w,h, 0x0C0);
    }

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);
}
//+
///Crawler_Crawl
F_METHOD(ULONG,Crawler_Crawl)
{
    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    F_Draw(Obj,FF_Draw_Update);

    F_Do(Obj,FM_Unlock);

    return TRUE;
}
//+
///Crawler_HandleEvent
F_METHODM(ULONG,Crawler_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,FM_Lock,FF_Lock_Exclusive);

    if (Msg -> Event -> Class == FF_EVENT_WINDOW)
    {
        if (Msg -> Event -> Code == FV_EVENT_WINDOW_ACTIVE)
        {
            if (!(FF_SignalHandler_Active & LOD -> sh.Flags))
            {
                F_Do(_app,FM_Application_AddSignalHandler,&LOD -> sh);
            }
        }
        else if (Msg -> Event -> Code == FV_EVENT_WINDOW_INACTIVE)
        {
            if (FF_SignalHandler_Active & LOD -> sh.Flags)
            {
                F_Do(_app,FM_Application_RemSignalHandler,&LOD -> sh);
            }
        }
    }

    F_Do(Obj,FM_Unlock);

    return 0;
}
//+

