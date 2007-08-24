#include "Private.h"

#define BY_SIDE 3

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Preview_New
F_METHOD(uint32,Preview_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;

   return F_SUPERDO();
}
//+

///Preview_Setup
F_METHOD(LONG,Preview_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      uint32 spec = F_Get(Obj,F_IDR(FA_Preview_Spec));
 
      if (spec)
      {
         LOD -> scheme = (FPalette *) F_Do(_display,FM_CreateColorScheme,spec,F_Get(Obj,FA_ColorScheme));
      }

      return TRUE;
   }
   return FALSE;
}
//+
///Preview_Cleanup
F_METHOD(void,Preview_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (_render)
   {
      if (LOD -> scheme)
      {
         F_Do(_display,FM_RemPalette,LOD -> scheme); LOD -> scheme = NULL;
      }
   }
   F_SUPERDO();
}
//+
///Preview_Draw
F_METHODM(void,Preview_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;

    static uint8 translate[] =
    {
       FV_Pen_Shine,FV_Pen_HalfShine,FV_Pen_Fill,FV_Pen_HalfShadow,FV_Pen_Shadow,FV_Pen_HalfDark,FV_Pen_Dark,FV_Pen_Text,FV_Pen_Highlight
    };

    uint32 *pn;

    F_SUPERDO();

    if (LOD -> scheme)
    {
        uint8 alpha;
            
        if (FF_Render_TrueColors & _render_flags)
        {
            if (FF_Area_Disabled & _flags)
            {
                alpha = 128;
            
                F_Erase(Obj, _ix, _iy, _ix2, _iy2, 0);
            }
            else
            {
                alpha = 0;
            }
            pn = LOD -> scheme -> ARGB;
        }
        else
        {
            pn = LOD -> scheme -> Pens;
        }


        if (_ih < 20 && _iw > 20)
        {
            int16  x = _ix;
            int16  y = _iy;
            uint16 w = _iw / FV_PEN_COUNT;
            uint16 h = _ih;
            uint16 dw = w;
            uint16 bonus_w = _iw - (w * FV_PEN_COUNT);
            uint8 a;
            
            if (FF_Render_TrueColors & _render->Flags)
            {
                for (a = 0 ; a < FV_PEN_COUNT ; a++)
                {
                    if (a == (FV_PEN_COUNT - 1)) dw = w + bonus_w;

                    if ((int16)(dw) > 0 && (int16)(h) > 0)
                    {
                        F_Do(_render, (uint32) "FM_Render_Fill",
                            x,y,dw,h,
                            ((pn[translate[a]]) << 8) | alpha);
                    }
                    x += dw;
                }
            }
            else
            {
                for (a = 0 ; a < FV_PEN_COUNT ; a++)
                {
                    if (a == (FV_PEN_COUNT - 1)) dw = w + bonus_w;

                    if ((int16)(dw) > 0 && (int16)(h) > 0)
                    {
                        _APen(pn[translate[a]]);
                        _Boxf(x,y,dw + x - 1,h + y - 1);
                    }
                    x += dw;
                }
            }
        }
        else if (_iw < 20 && _ih > 20)
        {
            int16  x = _ix;
            int16  y = _iy;
            uint16 w = _iw;
            uint16 h = _ih / FV_PEN_COUNT;
            uint16 dh = h;
            uint16 bonus_h = _ih - (h * FV_PEN_COUNT);
            UBYTE a;

            if (FF_Render_TrueColors & _render->Flags)
            {
                for (a = 0 ; a < FV_PEN_COUNT ; a++)
                {
                    if (a == (FV_PEN_COUNT - 1)) dh = h + bonus_h;

                    if ((int16)(dh) > 0 && (int16)(w) > 0)
                    {
                        F_Do(_render, (uint32) "FM_Render_Fill",
                            x,y,w,dh,
                            ((pn[translate[a]]) << 8) | alpha);
                    }
                    y += dh;
                } 
            }
            else
            {
                for (a = 0 ; a < FV_PEN_COUNT ; a++)
                {
                    if (a == (FV_PEN_COUNT - 1)) dh = h + bonus_h;

                    if ((int16)(dh) > 0 && (int16)(w) > 0)
                    {
                        _APen(pn[translate[a]]);
                        _Boxf(x,y,w + x - 1,dh + y - 1);
                    }
                    y += dh;
                }
            }
        }
        else
        {
            int16  y = _iy;
            uint16 w = _iw / BY_SIDE;
            uint16 h = _ih / BY_SIDE;
            uint16 bonusw = _iw - (w * BY_SIDE);
            uint16 bonush = _ih - (h * BY_SIDE);
            uint16 dh = h,a,b;
            uint8 p = 0;
            
            if (FF_Render_TrueColors & _render_flags)
            {
                for (a = 0 ; a < BY_SIDE ; a++)
                {
                    uint16 dw = w;
                    uint16 x = _ix;
                            
                    if (a == (BY_SIDE - 1)) dh = h + bonush;

                    for (b = 0 ; b < BY_SIDE ; b++)
                    {
                        if (b == (BY_SIDE - 1)) dw = w + bonusw;

                        if ((int16)(dw) > 0 && (int16)(dh) > 0)
                        {
                            F_Do(_render, (uint32) "FM_Render_Fill",
                                x,y,dw,dh,
                                ((pn[translate[p]]) << 8) | alpha);
                        }

                        x += w;
                        p++;
                    }
                    y += h;
                }
            }
            else
            {
                for (a = 0 ; a < BY_SIDE ; a++)
                {
                    uint16 dw = w;
                    uint16 x = _ix;

                    if (a == (BY_SIDE - 1)) dh = h + bonush;

                    for (b = 0 ; b < BY_SIDE ; b++)
                    {
                        if (b == (BY_SIDE - 1)) dw = w + bonusw;

                        if ((int16)(dw) > 0 && (int16)(dh) > 0)
                        {
                            _APen(pn[translate[p]]);
                            _Boxf(x,y,dw + x - 1,dh + y - 1);
                        }

                        x += w;
                        p++;
                    }
                    y += h;
                }
            }
        }

        if ((FF_Area_Disabled & _flags) && ((FF_Render_TrueColors & _render_flags) == 0))
        {
           STATIC uint16 pattern[] = {0xAAAA,0x5555,0xAAAA,0x5555};
           uint16 *prev_ptrn = rp -> AreaPtrn;
           uint32 prev_ptsz = rp -> AreaPtSz;

           rp -> AreaPtrn = pattern;
           rp -> AreaPtSz = 2;

           SetDrMd(rp,JAM1);

           _FPen(FV_Pen_Fill);
           _BPen(0);
           _Boxf(_ix,_iy,_ix2,_iy2);

           rp -> AreaPtSz = prev_ptsz;
           rp -> AreaPtrn = prev_ptrn;
        }
    }
}
//+

///Preview_Query
F_METHODM(uint32,Preview_Query,FS_Preview_Query)
{
   if (F_StrCmp(Msg -> Spec,"<scheme ",8) == 0)
   {
      return TRUE;
   }
   return FALSE;
}
//+
///Preview_ToString
F_METHODM(void,Preview_ToString,FS_Preview_ToString)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    STRPTR spec = (STRPTR) F_SUPERDO();

    if (_render)
    {
        F_Do(_display,FM_RemPalette,LOD -> scheme); LOD -> scheme = NULL;

        if (spec)
        {
            LOD -> scheme = (FPalette *) F_Do(_display,FM_CreateColorScheme,spec,F_Get(Obj,FA_ColorScheme));
        }

        F_Draw(Obj,(LOD -> scheme) ? FF_Draw_Update : FF_Draw_Object);
    }

    F_SUPERDO();
}
//+

