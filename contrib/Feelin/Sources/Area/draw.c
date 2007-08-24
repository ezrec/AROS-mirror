#include "Private.h"

/*
///DrRect
void DrRect(FRender *Render,FRect *Rect,ULONG Flags)
{
    ULONG Pen = 3;
 
    if (FF_Erase_Region & Flags)
    {
        struct RegionRectangle *rr;

        for (rr = ((struct Region *)(Rect)) -> RegionRectangle ; rr ; rr = rr -> Next)
        {
            if (rr -> bounds.MinX <= rr -> bounds.MaxX && rr -> bounds.MinY <= rr -> bounds.MaxY)
            {
                SetAPen(Render -> RPort,Pen);
                RectFill(Render -> RPort,((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MinX,
                                                 ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MinY,
                                                 ((struct Region *)(Rect)) -> bounds.MinX + rr -> bounds.MaxX,
                                                 ((struct Region *)(Rect)) -> bounds.MinY + rr -> bounds.MaxY);
            }
        }
    }
    else
    {
        SetAPen(Render -> RPort,Pen);
        RectFill(Render -> RPort,Rect -> x1,Rect -> y1,Rect -> x2,Rect -> y2);
    }
}
//+
*/
    
/*** Methods ***************************************************************/

///Area_Draw
F_METHODM(void,Area_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (_font)
    {
        SetFont(_rp,_font);
    }

    _render->Palette = _palette;

    #if 0
    F_Log(0,"disabled (%s) damaged (%s)",
        (FF_Area_Disabled & _flags) ? "yes" : "no",
        (FF_Area_Damaged & _flags) ? "yes" : "no");
    #endif
 
    if (FF_Area_Damaged & _flags)
    {
        _flags &= ~FF_Area_Damaged;
    }
    else
    {
        #if 0
        F_Log(0,"area has not been damaged");
        #endif
         
        return;
    }
 
    if (FF_Draw_Object & Msg -> Flags)
    {
        BOOL fill = TRUE;

        if ((FF_Area_Active & _flags) && (FF_AREA_HANDLEACTIVE & LOD -> Flags))
        {
            struct RastPort *rp = _render -> RPort;
            int16 x1 = _x - 1, x2 = x1 + _w + 1;
            int16 y1 = _y - 1, y2 = y1 + _h + 1;
            uint16 oldpt = rp -> LinePtrn;
                            
            SetABPenDrMd(rp, _pens[FV_Pen_Shine], _pens[FV_Pen_Dark], JAM2);

            SetDrPt(rp,0xCCCC);
            _Move(x1,y1); _Draw(x2,y1); _Draw(x2,y2); _Draw(x1,y2); _Draw(x1,y1);
            SetDrPt(rp,oldpt);
        }

        if (FF_AREA_NOFILL & LOD -> Flags)
        {
            fill = FALSE;
        }
        else if ((FF_Render_Refreshing & _render -> Flags) &&
                    (FF_AREA_INHERITED_BACK & LOD -> Flags) &&
                    (FF_AREA_INHERITED_PENS & LOD -> Flags) &&
                  !(FF_Render_Complex & _render -> Flags))
        {
            fill = FALSE;
        }

        F_SuperDo(Class, Obj, FM_Frame_Draw, _render, &_box, ((FF_Area_Selected & _flags) ? FF_Frame_Draw_Select : 0) | ((fill) ? FF_Frame_Draw_Erase : 0));
    }
}
//+
///Area_Erase
F_METHODM(void,Area_Erase,FS_Erase)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (FF_Area_CanDraw & _flags)
    {
        if (!(Msg -> Flags & FF_Erase_Fill))
        {
            if (FF_Render_Refreshing & _render -> Flags &&
              !(FF_Render_Complex    & _render -> Flags))
            {
                if (FF_AREA_INHERITED_BACK & LOD -> Flags &&
                     FF_AREA_INHERITED_PENS & LOD -> Flags)
                {
                    return;
                }
            }
        }

        _render->Palette = _palette;

        if (FF_Erase_Box & Msg -> Flags)
        {
            FRect r;
            
            r.x1 = ((FBox *)(Msg -> Rect)) -> x;
            r.y1 = ((FBox *)(Msg -> Rect)) -> y;
            r.x2 = ((FBox *)(Msg -> Rect)) -> x + ((FBox *)(Msg -> Rect)) -> w - 1;
            r.y2 = ((FBox *)(Msg -> Rect)) -> y + ((FBox *)(Msg -> Rect)) -> h - 1;

            F_Do(LOD -> FramePublic -> Back, FM_ImageDisplay_Draw, _render, &r, 0);
        }
        else
        {
            F_Do(LOD -> FramePublic -> Back, FM_ImageDisplay_Draw, _render, Msg->Rect, (FF_Erase_Region & Msg -> Flags) ? FF_ImageDisplay_Region : 0);
        }
    }
}
//+
