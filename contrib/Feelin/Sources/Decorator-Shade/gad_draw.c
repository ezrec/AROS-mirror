#include "Private.h"

#undef LocalObjectData
#define LocalObjectData GAD_LocalObjectData

#define GRAPHICS_LIBRARY_IS_BUGGED 1 // GROG: grrrrr!!! oblique lines hasn't being drawed correctly !

/*** Private ***************************************************************/

///DrBevel
void DrBevel(struct RastPort *rp,int16 x1,int16 y1,int16 x2,int16 y2,uint32 p1,uint32 p2,uint32 p3)
{
    _APen(p1); _Move(x1,y2-1); _Draw(x1,y1); _Draw(x2-1,y1);
    _APen(p2); _Move(x1+1,y2); _Draw(x2,y2); _Draw(x2,y1+1);
    _APen(p3); _Plot(x1,y2);   _Plot(x2,y1);
}
//+
///DrBox
void DrBox(struct RastPort *rp,int16 x1,int16 y1,int16 x2,int16 y2)
{
    _Move(x1,y2); _Draw(x1,y1); _Draw(x2,y1); _Draw(x2,y2); _Draw(x1,y2);
}
//+
///DrShape
void DrShape(struct RastPort *rp,int16 x1,int16 y1,int16 x2,int16 y2,uint32 * pn)
{
    if (x1 > x2 || y1 > y2) return;

    _APen(pn[FV_Pen_HalfDark]);
    DrBox(rp,x1,y1,x2,y2);

    if (x1+2 <= x2-2 && y1+2 <= y2-2)
    {
        _APen(pn[FV_Pen_Fill]);
        _Boxf(x1+2,y1+2,x2-2,y2-2);
    }
    DrBevel(rp,x1+1,y1+1,x2-1,y2-1,pn[FV_Pen_HalfShine],pn[FV_Pen_HalfShadow],pn[FV_Pen_Fill]);
}
//+
///DrGlow
void DrGlow(struct RastPort *rp,int16 x1,int16 y1,int16 x2,int16 y2,uint32 *pn)
{
    if (x1 > x2 || y1 > y2) return;

    _APen(pn[FV_Pen_HalfDark])   ;  DrBox(rp,x1,y1,x2,y2);

    x1++; y1++; x2--; y2--;

    _APen(pn[FV_Pen_HalfShadow]) ; _Boxf(x1,y1,x2,y2);
    _APen(pn[FV_Pen_Shine])      ; _Plot(x1,y1);
                                          ; _Move(x2-2,y2) ; _Draw(x2,y2) ; _Draw(x2,y2-2);
    _APen(pn[FV_Pen_Shadow])     ; _Move(x1,y2-1) ; _Draw(x1,y1+2) ; _Draw(x1+2,y1) ; _Draw(x2-1,y1);
                                          ; _Plot(x1+1,y1+2) ; _Plot(x1+2,y1+1);
    _APen(pn[FV_Pen_HalfShine])  ; _Move(x1+1,y2) ; _Draw(x2-3,y2) ; _Draw(x2,y2-3) ; _Draw(x2,y1+1) ; _Plot(x2-1,y2-1);
}
//+
///DrOblique
int16 DrOblique(struct RastPort *rp,int16 x1,int16 y1,int16 y2,uint32 n,uint32 s,uint32 *pn)
{
    uint16 i,w = y2 - y1 + 1;

    for (i = 1 ; i <= n ; i++)
    {
        _APen(pn[FV_Pen_HalfShadow])  ; _Move(x1,y2) ; _Draw(x1+w-1,y1);
        
        #ifdef GRAPHICS_LIBRARY_IS_BUGGED
        _Plot(x1 + w - 1, y1);
        #endif
 
        _APen(pn[FV_Pen_HalfShine])   ; _Plot(x1+w,y1-1);
        _APen(pn[FV_Pen_HalfDark])    ; _Plot(x1-1,y2+1);
        
        x1++;
    }

    for (n = i ; n <= s ; n++)
    {
        _APen(pn[FV_Pen_Fill])     ; _Move(x1,y2) ; _Draw(x1+w-1,y1);
        
        #ifdef GRAPHICS_LIBRARY_IS_BUGGED
        _Plot(x1 + w - 1, y1);
        #endif
        
        _APen(pn[FV_Pen_Shine])    ; _Plot(x1+w,y1-1);
        _APen(pn[FV_Pen_Shadow])   ; _Plot(x1-1,y2+1);
        
        x1++;
    }
    return x1;
}
//+

/*** Methods ***************************************************************/

///Gad_Draw
F_METHOD(void,Gad_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;
    uint32 *pn = _pens;//_render -> Palette -> Pens; //_pens;
    int16 x1 = _x, x2 = _x2;
    int16 y1 = _y, y2 = _y2;
                    
//   F_Log(0,"Palette 0x%08lx - Pens 0x%08lx ?= 0x%08lx",_render -> Palette,_render -> Palette -> Pens,_pens);

//   pn = _render -> Palette -> Pens;

    if (x2 <= x1 || y2 <= y1) return;
    
    switch (LOD -> Type)
    {
///FV_GadgetType_Close
        case FV_GadgetType_Close:
        {
            if (FF_Area_Selected & _flags)
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrGlow(rp,x1+1,y1+1,x2-1,y2-1,pn);
            }
            else
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrShape(rp,x1+1,y1+1,x2-1,y2-1,pn);
            }
        }
        break;
//+
///FV_GadgetType_Iconify
        case FV_GadgetType_Iconify:
        {
            int16 x3 = (x2 - x1 + 1) / 2 + x1;
            int16 y3 = (y2 - y1 + 1) / 2 + y1;

            if (FF_Area_Selected & _flags)
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrGlow(rp,x1+1,y1+1,x2-1,y2-1,pn);

                _APen(pn[FV_Pen_HalfDark]); _Boxf(x1+3,y3+1,x3-1,y2-3);
                _APen(pn[FV_Pen_Shine]);    _Boxf(x1+4,y3+2,x3-2,y2-4);
            }
            else
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrShape(rp,x1+1,y1+1,x2-1,y2-1,pn);
                _APen(pn[FV_Pen_HalfDark]); DrBox(rp,x1+3,y3,x3+1,y2-3);
                _APen(pn[FV_Pen_Shine]);    _Boxf(x1+4,y3+1,x3,y2-4);
            }
        }
        break;
//+
///FV_GadgetType_Dragbar
        case FV_GadgetType_Dragbar:
        {
            if (!F_Get(_parent,FA_Active))
            {
                FAreaPublic *pub = (FAreaPublic *) F_Get(_parent,FA_Area_PublicData);

                x1 = pub -> Box.x + 1; x2 = pub -> Box.x + pub -> Box.w - 1 - 1;
                pn = _render -> Palette -> Pens;
            }
        
            if (x2 > (x1 + TITLEX1 + TITLEX2))
            {
                FRect rect;
                uint32 shine  = pn[FV_Pen_Shine];
                uint32 fill   = pn[FV_Pen_Fill];
                uint32 shadow = pn[FV_Pen_Shadow];
                STRPTR wtitle = (STRPTR) F_Get(_win,FA_Window_Title);
                uint16 tw     = 0;

                rect.x1 = x1 + TITLEX1; rect.x2 = x2 - TITLEX2;
                rect.y1 = y1 + TITLEY1; rect.y2 = y2 - TITLEY2;

                if (wtitle)
                {
                    if (LOD -> TDisplay)
                    {
                        F_Do(LOD -> TDisplay,FM_TextDisplay_Cleanup);

                        F_Do(LOD -> TDisplay,FM_Set,
                                                    FA_TextDisplay_PreParse, LOD -> Prep[F_Get(_parent,FA_Active) ? 0 : 1],
                                                    FA_TextDisplay_Contents, wtitle,
                                                    FA_TextDisplay_Width,    rect.x2 - rect.x1 + 1,
                                                    FA_TextDisplay_Height,   rect.y2 - rect.y1 + 1,
                                                    TAG_DONE);

                        F_Do(LOD -> TDisplay,FM_TextDisplay_Setup,_render);

                        tw = F_Get(LOD -> TDisplay,FA_TextDisplay_Width);
                    }
                }

                if (tw)
                {
                    _APen(shine);  _Move(x1,y2-1); _Draw(x1,y1); _Draw(rect.x1 + tw - 1,y1);
                    _APen(shadow); _Move(x1+1,y2); _Draw(rect.x1 + tw - 1,y2);
                    _APen(fill);   _Plot(x1,y2);   _Boxf(x1+1,y1+1,rect.x1 + tw - 1,y2-1);

                    x1 = rect.x1 + tw;
                }
                else
                {
                    _APen(shine);  _Move(x1,y2-1) ; _Draw(x1,y1) ; _Draw(x1 + TITLEX1 - 1,y1);
                    _APen(shadow); _Move(x1+1,y2) ; _Draw(x1 + TITLEX1 - 1,y2);
                    _APen(fill);   _Plot(x1,y2)   ; _Boxf(x1+1,y1+1,x1 + TITLEX1 - 1,y2-1);

                    x1 += TITLEX1;
                }

                if ((y2 - y1 + 1 + 20 + x1) > x2)
                {
                    _APen(shine);  _Move(x1,y1) ; _Draw(x2-1,y1);
                    _APen(shadow); _Move(x1,y2) ; _Draw(x2,y2) ; _Draw(x2,y1+1);
                    _APen(fill);   _Plot(x2,y1) ; _Boxf(x1,y1+1,x2-1,y2-1);
                }
                else
                {
                    uint16 p;
                    y1++; y2-- ; x2-- ;
                    p = y2 - y1 + 1;

                    _APen(shine)      ; _Move(x1,y1-1) ; _Draw(x1 + p,y1-1);
                    _APen(pn[FV_Pen_HalfShadow]) ; _Plot(x2+1,y1-1);

                    {
                        uint32 i;
                        int16 a = x1 + p - 1;

                        _APen(fill);

                        for (i = y1 ; i <= y2 ; i++)
                        {
                            _Move(x1,i); _Draw(a--,i);
                        }
                    }

                    x1++;

                    x1 = DrOblique(rp,x1,y1,y2,1,5,pn);
                    x1 = DrOblique(rp,x1,y1,y2,2,5,pn);
                    x1 = DrOblique(rp,x1,y1,y2,3,5,pn);
                    x1 = DrOblique(rp,x1,y1,y2,4,5,pn);

                    _APen(pn[FV_Pen_HalfShine]); _Move(x1+p,y1 - 1) ; _Draw(x2,y1-1);
                    _APen(pn[FV_Pen_HalfDark]);  _Move(x1-1,y2+1)   ; _Draw(x2+1,y2+1) ; _Draw(x2+1,y1);

                    {
                        int16 i;
                        int16 a = x1 + p - 1;

                        _APen(pn[FV_Pen_HalfShadow]);

                        for (i = y2 ; i > y1 ; i--)
                        {
                            _Move(x1++,i); _Draw(a,i);
                        }
                    }
                    _Boxf(x1,y1,x2,y2);
                }

                if (tw)
                {
                    uint16 h = rect.y2 - rect.y1 + 1;
                    uint16 td_h = F_Get(LOD -> TDisplay,FA_TextDisplay_Height);

                    if (td_h < h)
                    {
                        rect.y1 = (h - td_h) / 2 + rect.y1;
                    }

                    F_Do(LOD -> TDisplay,FM_TextDisplay_Draw,&rect);
                }
            }
            else if (x1 + 3 <= x2)
            {
                _APen(pn[FV_Pen_Shine]);
                _Move(x1,y2-1); _Draw(x1,y1); _Draw(x2-1,y1);
                _APen(pn[FV_Pen_Shadow]);
                _Move(x1+1,y2); _Draw(x2,y2); _Draw(x2,y1+1);
                _APen(pn[FV_Pen_Fill]);
                _Boxf(x1+1,y1+1,x2-1,y2-1); _Plot(x1,y2); _Plot(x2,y1);
            }
            else if (x2 > x1)
            {
                _APen(pn[FV_Pen_Fill]);
                _Boxf(x1,y1,x2,y2);
            }
        }
        break;
//+
///FV_GadgetType_Zoom
        case FV_GadgetType_Zoom:
        {
            if (FF_Area_Selected & _flags)
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrGlow(rp,x1+1,y1+1,x2-1,y2-1,pn);
            }
            else
            {
                DrBevel(rp,x1,y1,x2,y2,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrShape(rp,x1+1,y1+1,x2-1,y2-1,pn);

                x1++; y1++; x2--; y2--;

                DrShape(rp,x1,y1,(x2 - x1 + 1) / 2 + x1 + 1,(y2 - y1 + 1) / 2 + y1 + 1,pn);
            }
        }
        break;
//+
///FV_GadgetType_Depth
        case FV_GadgetType_Depth:
            if (FF_Area_Selected & _flags)
            {
                int16 a,b;

                _APen(pn[FV_Pen_Fill]); _Boxf(x1,y1,x2,y2);

                x1++; y1++; x2--; y2--;

                a = (x2 - x1 + 1) / 2;
                b = (y2 - y1 + 1) / 2;

                DrBevel(rp,x1-1,y1-1,x1+a+3,y1+b+3,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrBevel(rp,x2-a-3,y2-b-3,x2+1,y2+1,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrGlow(rp,x1,y1,x1+a+2,y1+b+2,pn);
                DrGlow(rp,x2-a-2,y2-b-2,x2,y2,pn);
            }
            else
            {
                int16 a,b;

                //_FPen(Obj,FV_Pen_Fill); _Boxf(x1,y1,x2,y2);
                _APen(pn[FV_Pen_Fill]); _Boxf(x1,y1,x2,y2);

                x1++; y1++; x2--; y2--;

                a = (x2 - x1 + 1) / 2;
                b = (y2 - y1 + 1) / 2;

                DrBevel(rp,x1-1,y1-1,x1+a+3,y1+b+3,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrBevel(rp,x2-a-3,y2-b-3,x2+1,y2+1,pn[FV_Pen_HalfShadow],pn[FV_Pen_HalfShine],pn[FV_Pen_Fill]);
                DrShape(rp,x2-a-2,y2-b-2,x2,y2,pn);
                DrShape(rp,x1,y1,x1+a+2,y1+b+2,pn);
            }
            break;
//+
    }
}
//+
