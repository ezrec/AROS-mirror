#include "Private.h"

#define FF_DRAWBORDER_UP                        (1 << 0)
#define FF_DRAWBORDER_DOWN                      (1 << 1)
#define FF_DRAWBORDER_SEPARE                    (1 << 2)
#define FF_DRAWBORDER_SIZEGAD                   (1 << 3)

///shade_draw_border
STATIC void shade_draw_border(FClass *Class,FObject Obj,struct RastPort *rp,uint32 *pn,bits32 Flags)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    int16 x1 = _x, x2 = _x2, ix1 = _ix - 1, ix2 = _ix2 + 1;
    int16 y1 = _y, y2 = _y2, iy1 = _iy - 1, iy2 = _iy2 + 1;

    if (FF_DRAWBORDER_UP & Flags)
    {
//      F_Log(0,"draw_border_up");

        _APen(pn[FV_Pen_Dark]);
        _Line(x1,iy1,x1,y1); _Draw(x2,y1); _Draw(x2,iy1);
        _Line(ix1,iy1,ix2,iy1);

        _APen(pn[FV_Pen_Shine]);
        _Line(x1+1,iy1,x1+1,y1+1); _Draw(x2-2,y1+1); _Plot(ix2+1,iy1);

        _APen(pn[FV_Pen_Shadow]);
        _Line(ix1,iy1-1,ix2,iy1-1); _Plot(ix1-1,iy1); _Plot(ix1-1,iy1-1);
        _Line(x2-1,y1+2,x2-1,iy1);

        _APen(pn[FV_Pen_Fill]);
        _Boxf(x1+2,y1+2,ix1-2,iy1);
        _Boxf(ix2+2,y1+2,x2-2,iy1); _Plot(ix2+1,iy1-1); _Plot(x2-1,y1+1);
        _Boxf(ix1-1,y1+2,ix2+1,iy1-2);
    }

    if (FF_DRAWBORDER_DOWN & Flags)
    {
//      F_Log(0,"draw_border_down");

        _APen(pn[FV_Pen_Dark]);
        _Line(x1,iy1+1,x1,y2); _Draw(x2,y2); _Draw(x2,iy1+1);
        _Line(ix1,iy1+1,ix1,iy2); _Draw(ix2,iy2); _Draw(ix2,iy1+1);

        _APen(pn[FV_Pen_Shine]);
        _Line(x1+1,iy1+1, x1+1,y2-2);
        _Line(ix1,iy2+1,  ix2+1,iy2+1); _Draw(ix2+1,iy1+1);

        _APen(pn[FV_Pen_Shadow]);
        _Line(ix1-1,iy1+1,ix1-1,iy2);
        _Line(x1+2,y2-1,x2-1,y2-1); _Draw(x2-1,iy1+1);

        _APen(pn[FV_Pen_Fill]);
        _Plot(x1+1,y2-1); _Plot(ix1-1,iy2+1);
        _Boxf(x1+2,iy1+1, ix1-2,iy2+1);
        _Boxf(x1+2,iy2+2, x2-2,y2-2);
        _Boxf(ix2+2,iy1+1,   x2-2,iy2+1);
    }

    if (FF_DRAWBORDER_SEPARE & Flags)
    {
//      F_Log(0,"draw_border_separate");

        _APen(pn[FV_Pen_Shine]);
        _Line(x1+2,iy1+1,ix1-2,iy1+1);
        _Line(ix2+2,iy1+1,x2-2,iy1+1);
        _APen(pn[FV_Pen_Fill]);
        _Plot(ix1-1,iy1+1); _Plot(x2-1,iy1+1);
    }

    if (FF_DRAWBORDER_SIZEGAD & Flags)
    {
        int16 x = x2 - FV_GADSIZE_WIDTH + 1;
        int16 y = y2 - FV_GADSIZE_WIDTH + 1;

//      F_Log(0,"draw_border_size");

        if (x >= ix1 + 3)
        {
            _APen(pn[FV_Pen_Dark]);   _Line(x,iy2+1, x,y2);
            _APen(pn[FV_Pen_Shadow]); _Line(x-1,iy2+2, x-1,y2-1);
            _APen(pn[FV_Pen_Fill]);   _Plot(x-1,iy2+1); _Plot(x+1,y2-1);
            _APen(pn[FV_Pen_Shine]);  _Line(x+1,iy2+1, x+1,y2-2);
        }
        if (y >= iy1 + 3)
        {
            _APen(pn[FV_Pen_Dark]);    _Line(ix2+1,y, x2,y);
            _APen(pn[FV_Pen_Shine]);   _Line(ix2+1,y+1, x2-1,y+1);
            _APen(pn[FV_Pen_Shadow]);  _Line(ix2+2,y-1, x2-1,y-1);
            _APen(pn[FV_Pen_Fill]);    _Plot(x2-1,y+1); _Plot(ix2+1,y-1);
        }
    }
}
//+
///shade_draw_active
STATIC void shade_draw_active(FClass *Class,FObject Obj,bits32 Flags)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
            
    #ifdef F_USE_LAST_DRAWN
    if (LOD->last_drawn != FV_Shade_LastDrawn_Active)
    #endif
    {
        struct RastPort *rp = _rp;
        
        uint32 *pn = LOD->scheme_active -> Pens;
        
        int16 x1 = _x;
        int16 y1 = _y;
        int16 x2 = _x2;
        int16 ix1 = _ix - 1;
        int16 ix2 = _ix2 + 1;
        int16 iy1 = _iy - 1;

        bits32 draw_border_flags = FF_DRAWBORDER_DOWN;

        _render -> Palette = LOD->scheme_active;
        
        if (FF_SHADE_DRAG & LOD->flags)
        {
            FAreaPublic *gad_ad = (FAreaPublic *) F_Get(LOD -> gadget_drag,FA_Area_PublicData);
            
            int16 dx1 = gad_ad -> Box.x - 1;
            int16 dx2 = dx1 + gad_ad -> Box.w + 1;

            _APen(pn[FV_Pen_Dark]);
            _Line( x1,iy1,  x1, y1); _Draw(x2,y1) ; _Draw(x2,iy1);
            _Line(ix1,iy1, ix2,iy1);
            _Line(dx1,y1+1,dx1,iy1-1);
            _Line(dx2,y1+1,dx2,iy1-1);

            _APen(pn[FV_Pen_Shine]);
            _Line(x1+1,iy1,x1+1,y1+1); _Draw(dx1-2,y1+1);
            _Line(dx2+1,iy1,dx2+1,y1+1); _Draw(x2-2,y1+1);

            _APen(pn[FV_Pen_Shadow]);
            _Line(dx1-1,iy1,dx1-1,y1+2);
            _Line(x2-1,iy1,x2-1,y1+2);

            _APen(pn[FV_Pen_Fill]);
            _Line(x1+2,iy1,ix1-2,iy1);
            _Line(ix2+2,iy1,x2-2,iy1);
            _Boxf(x1+2,y1+2,dx1-2,iy1-1); _Plot(dx1-1,y1+1);
            _Boxf(dx2+2,y1+2,x2-2,iy1-1); _Plot(x2-1,y1+1);

            F_Draw(LOD -> gadget_drag,FF_Draw_Object);
 
            if (!(FF_SHADE_TITLEBARONLY & LOD->flags))
            {
                if (LOD -> gadget_close || LOD -> gadget_iconify)
                {
                    _APen(pn[FV_Pen_Dark]);    _Plot(dx1-1,iy1);
                    _APen(pn[FV_Pen_Shadow]);  _Plot(ix1-1,iy1); _Line(ix1-1,iy1-1,dx1-1,iy1-1);
                
                    F_Draw(LOD -> gadget_close,FF_Draw_Object);
                    F_Draw(LOD -> gadget_iconify,FF_Draw_Object);
                }

                if (LOD -> gadget_zoom || LOD -> gadget_depth)
                {
                    _APen(pn[FV_Pen_Dark]);    _Plot(dx2+1,iy1);
                    _APen(pn[FV_Pen_Shadow]);  _Line(dx2+2,iy1-1,ix2,iy1-1);
                    _APen(pn[FV_Pen_Shine]);   _Plot(ix2+1,iy1);
                    _APen(pn[FV_Pen_Fill]);    _Plot(dx2+1,iy1-1);
                    
                    F_Draw(LOD -> gadget_zoom,FF_Draw_Object);
                    F_Draw(LOD -> gadget_depth,FF_Draw_Object);
                }
            }
        }
        else
        {
            draw_border_flags |= FF_DRAWBORDER_UP;
        }

        if (FF_SHADE_SIZE & LOD->flags)
        {
            draw_border_flags |= FF_DRAWBORDER_SIZEGAD;
        }

        shade_draw_border(Class,Obj,rp,pn,draw_border_flags);

        #ifdef F_USE_LAST_DRAWN
        LOD->last_drawn = FV_Shade_LastDrawn_Active;
        #endif
    }
}
//+
///shade_draw_inactive
STATIC void shade_draw_inactive(FClass *Class,FObject Obj,bits32 Flags)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    #ifdef F_USE_LAST_DRAWN
    if (LOD->last_drawn != FV_Shade_LastDrawn_Inactive)
    #endif
    {
        struct RastPort *rp  = _rp;
        uint32 *pn  = LOD->scheme_inactive -> Pens;
        int16 x1 = _x, x2 = _x2;
        int16 y1 = _y, iy1 = _iy - 1;
        bits32 border_flags = FF_DRAWBORDER_DOWN;

        _render -> Palette = LOD->scheme_inactive;

        if (FF_SHADE_DRAG & LOD->flags)
        {
            _APen(pn[FV_Pen_Dark]);
            _Line(x1,iy1,x1,y1); _Draw(x2,y1); _Draw(x2,iy1); _Draw(x1,iy1);

            F_Draw(LOD -> gadget_drag,FF_Draw_Object);

            border_flags |= FF_DRAWBORDER_SEPARE;
        }
        else
        {
            border_flags |= FF_DRAWBORDER_UP;
        }

        shade_draw_border(Class,Obj,rp,pn,border_flags);

        #ifdef F_USE_LAST_DRAWN
        LOD->last_drawn = FV_Shade_LastDrawn_Inactive;
        #endif
    }
}
//+

/*** Methods ***************************************************************/

///Shade_Draw
F_METHODM(void,Shade_Draw,FS_Draw)
{
    if (FF_Draw_Object & Msg -> Flags)
    {
        if (F_Get(Obj,FA_Active))
        {
            shade_draw_active(Class,Obj,Msg -> Flags);
        }
        else
        {
            shade_draw_inactive(Class,Obj,Msg -> Flags);
        }
    }
}
//+

