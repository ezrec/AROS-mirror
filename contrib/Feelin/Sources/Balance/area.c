#include "Private.h"

/*** Private ***************************************************************/

///DrPlot
STATIC void DrPlot(struct LocalObjectData *LOD,WORD x,WORD y)
{
    struct RastPort *rp = _rp;

    _FPen(FV_Pen_Shine);      _Plot(x-1,y); _Plot(x-1,y-1); _Plot(x,y-1);
    _FPen(FV_Pen_HalfShadow); _Plot(x,y+1); _Plot(x+1,y+1); _Plot(x+1,y);
}
//+

/*** Methods ***************************************************************/

///Balance_Setup
F_METHOD(LONG,Balance_Setup)
{
    if (F_SUPERDO())
    {
//      balance_begin(Class,Obj);

        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

        return TRUE;
    }
    return FALSE;
}
//+
///Balance_Cleanup
F_METHOD(uint32,Balance_Cleanup)
{
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);

    return F_SUPERDO();
}
//+
///Balance_GoActive
F_METHOD(void,Balance_GoActive)
{
    F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);
    F_Draw(Obj,FF_Draw_Object);
}
//+
///Balance_GoInactive
F_METHOD(void,Balance_GoInactive)
{
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);
    F_Draw(Obj,FF_Draw_Object);
}
//+
///Balance_AskMinMax
F_METHOD(uint32,Balance_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_Get(_parent,FA_Horizontal))
    {
        LOD -> Flags |= FF_Balance_Vertical;
    }

    if (LOD -> Flags & FF_Balance_Vertical)
    {
        _minw = 7;
        _maxw = _minw;
    }
    else
    {
        _minh = 7;
        _maxh = _minh;
    }

    return F_SUPERDO();
}
//+ 
///Balance_Draw
F_METHOD(void,Balance_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;
    int16 x1 = _x;
    int16 x2 = _x2;
    int16 y1 = _y;
    int16 y2 = _y2;
    uint32 ap;
    uint32 bp;

    F_SUPERDO();
        
    ap = _pens[(FF_Area_Active & _flags) ? FV_Pen_Shine  : FV_Pen_HalfShine];
    bp = _pens[(FF_Area_Active & _flags) ? FV_Pen_Shadow : FV_Pen_HalfShadow];
      
    if (FF_Area_Selected & _flags)
    {
        uint32 temp = ap; ap = bp; bp = temp;
    }

    _APen(ap); _Move(x1,y2-1); _Draw(x1,y1); _Draw(x2-1,y1);
    _APen(bp); _Move(x1+1,y2); _Draw(x2,y2); _Draw(x2,y1+1);

    if (FF_Area_Active & _flags)
    {
        _FPen(FV_Pen_HalfShine);
        _Boxf(x1+1,y1+1,x2-1,y2-1) ; _Plot(x1,y2) ; _Plot(x2,y1);
    }

    x1 = _w / 2 + x1;
    y1 = _h / 2 + y1;

    if (FF_Balance_Vertical & LOD -> Flags)
    {
        DrPlot(LOD,x1,y1);
        DrPlot(LOD,x1,y1 - 4);
        DrPlot(LOD,x1,y1 + 4);
    }
    else
    {
        DrPlot(LOD,x1 - 4,y1);
        DrPlot(LOD,x1,    y1);
        DrPlot(LOD,x1 + 4,y1);
    }
}
//+
