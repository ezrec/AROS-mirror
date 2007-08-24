#include "Private.h"

/*** Methods ***************************************************************/

///Gauge_New
F_METHOD_NEW(Gauge_New)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    BOOL horiz=FALSE;
  
    F_AREA_SAVE_PUBLIC;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Horizontal:  horiz = item.ti_Data; break;
        case FA_Gauge_Info:  LOD -> Info = (STRPTR)(item.ti_Data); break;
    }

    return (FObject) F_SuperDo(Class,Obj,Method,

        FA_Back,          (horiz) ? "$gauge-back" : "$gauge-back-vertical",
        FA_Frame,         "$gauge-frame",
        FA_ColorScheme,   "$gauge-scheme",
        FA_NoFill,        TRUE,
        FA_ChainToCycle,  FALSE,

        TAG_MORE, Msg);
}
//+
///Gauge_Set
F_METHOD_SET(Gauge_Set)
{
    struct LocalObjectData *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Gauge_Simple:
        {
            if (item.ti_Data) LOD -> Flags |=  FF_Gauge_Simple;
            else              LOD -> Flags &= ~FF_Gauge_Simple;

            F_Draw(Obj,FF_Draw_Update);
        }
        break;
    }
    F_SUPERDO();
}
//+
///Gauge_Get
F_METHOD_GET(Gauge_Get)
{
    struct LocalObjectData  *LOD  = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Gauge_Simple:   F_STORE(0 != (LOD -> Flags & FF_Gauge_Simple)); break;
    }

    F_SUPERDO();
}
//+
///Gauge_AskMinMax
F_METHOD_ASKMINMAX(Gauge_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _minh += 10;
    _minw += 10;

    F_SUPERDO();
}
//+
///Gauge_Draw
F_METHOD_DRAW(Gauge_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;
    uint32 *pn;
    int16 x1 = _ix;
    int16 x2 = x1 + _iw - 1;
    int16 y1 = _iy;
    int16 y2 = y1 + _ih - 1;
    int16 s,mp;
    int32 val,max,min;

    STATIC uint16 _Gauge_Pattern1[] =
    {
       0xFF00, 0x7F80, 0x3FC0, 0x1FE0,
       0x0FF0, 0x07F8, 0x03FC, 0x01FE,
       0x00FF, 0x807F, 0xC03F, 0xE01F,
       0xF00F, 0xF807, 0xFC03, 0xFE01
    };

    F_SUPERDO();
    
    pn = _pens;

    F_Do(Obj,FM_Get,
       
       F_IDR(FA_Numeric_Value),   &val,
       F_IDR(FA_Numeric_Min),     &min,
       F_IDR(FA_Numeric_Max),     &max,
       
       TAG_DONE);

    if (((max - min) == 0) || (min > max))
    {
       goto _Gauge_Draw_Fill;
    }

    s  = (FF_Horizontal & _flags) ? (x2 - x1 + 1) : (y2 - y1 + 1);
    mp = val * s / (max - min);

    if (FF_Gauge_Simple & LOD -> Flags)
    {
       if (mp)
       {
          _APen(pn[FV_Pen_HalfShine]);
          _BPen(pn[FV_Pen_HalfShadow]);

          if (FF_Horizontal & _flags)
          {
             mp += x1 - 1;

             if (x1 < x2)
             {
                rp -> AreaPtrn = _Gauge_Pattern1;
                rp -> AreaPtSz = 4;

                _Boxf(x1,y1,mp,y2);

                rp -> AreaPtSz = 0;
                rp -> AreaPtrn = NULL;
             }
             x1 = mp + 1;
          }
          else
          {
             mp = y2 - mp + 1;

             if (y1 < y2)
             {
                rp -> AreaPtrn = _Gauge_Pattern1;
                rp -> AreaPtSz = 4;

                _Boxf(x1,mp,x2,y2);

                rp -> AreaPtSz = 0;
                rp -> AreaPtrn = NULL;
             }
             y2 = mp - 1;
          }
       }
    }
    else
    {
       if (FF_Horizontal & _flags)
       {
          if (mp)
          {
             mp = x1 + mp - 1;
             s  = mp - x1 + 1;

             if (s > 0)
             {
                _APen(pn[FV_Pen_Dark]);  _Move(mp,y1);  _Draw(mp,y2);
             }

             if (s > 1)
             {
                _APen(pn[FV_Pen_Dark]) ; _Move(x1,y1) ; _Draw(x1,y2);
             }

             if (s > 2);
             {
                x1 += 1;

                _APen(pn[FV_Pen_Fill]);        _Plot(x1,y1);     _Plot(x1,y2-2);
                _APen(pn[FV_Pen_HalfShine]);   _Plot(x1,y1+1);   _Plot(x1,y2-3);
                _APen(pn[FV_Pen_Shine]);       _Move(x1,y1+2);   _Draw(x1,y2-4);
                _APen(pn[FV_Pen_HalfShadow]);  _Plot(x1,y2-1);
                _APen(pn[FV_Pen_Shadow]);      _Plot(x1,y2);
                x1 -= 1;
             }

             if (s > 3)
             {
                mp -= 1;
                _Move(mp,y1+1); _Draw(mp,y2-1);
                _APen(pn[FV_Pen_HalfShadow]);  _Plot(mp,y1);
                _APen(pn[FV_Pen_HalfDark]);    _Plot(mp,y2);
                mp += 1;
             }

             if (s > 4)
             {
                mp -= 2;
                _Plot(mp,y2);
                _APen(pn[FV_Pen_Shadow]);      _Plot(mp,y2-1);
                _APen(pn[FV_Pen_HalfShadow]);  _Plot(mp,y2-2);   _Plot(mp,y1);
                _APen(pn[FV_Pen_Fill]);        _Plot(mp,y2-3);   _Plot(mp,y1+1);
                _APen(pn[FV_Pen_HalfShine]);   _Move(mp,y1+2);   _Draw(mp,y2-4);
                mp += 2;
             }

             if (s > 5)
             {
                x1 += 2;
                _Plot(x1,y2-4); _Plot(x1,y1+2);
                _APen(pn[FV_Pen_HalfDark]);    _Plot(x1,y2);
                _APen(pn[FV_Pen_Shadow]);      _Plot(x1,y2-1);
                _APen(pn[FV_Pen_HalfShadow]);  _Plot(x1,y2-2);   _Plot(x1,y1);
                _APen(pn[FV_Pen_Fill]);        _Plot(x1,y2-3);   _Plot(x1,y1+1);
                _APen(pn[FV_Pen_Shine]);       _Move(x1,y2-5);   _Draw(x1,y1+3);
                x1 -= 2;
             }

             if (s > 6)
             {
                x1 += 3;
                mp -= 3;
                _Boxf(x1,y1+3,mp,y2-5);
                _APen(pn[FV_Pen_HalfDark]);    _Move(x1,y2);     _Draw(mp,y2);
                _APen(pn[FV_Pen_Shadow]);      _Move(x1,y2-1);   _Draw(mp,y2-1);
                _APen(pn[FV_Pen_HalfShadow]);  _Move(x1,y2-2);   _Draw(mp,y2-2); _Move(x1,y1);   _Draw(mp,y1);
                _APen(pn[FV_Pen_Fill]);        _Move(x1,y2-3);   _Draw(mp,y2-3); _Move(x1,y1+1); _Draw(mp,y1+1);
                _APen(pn[FV_Pen_HalfShine]);   _Move(x1,y2-4);   _Draw(mp,y2-4); _Move(x1,y1+2); _Draw(mp,y1+2);
                mp += 3;
             }
             x1 = mp + 1;
          }
       }
       else
       {
          if (mp)
          {
             mp = y2 - mp + 1;
             s  = y2 - mp + 1;

             if (s > 0)
             {
                _APen(pn[FV_Pen_Dark]); _Move(x1,y2); _Draw(x2,y2);
             }

             if (s > 1)
             {
                _APen(pn[FV_Pen_Dark]); _Move(x1,mp); _Draw(x2,mp);
             }

             if (s > 2)
             {
                mp += 1;
                _APen(pn[FV_Pen_Fill]);       _Plot(x1,mp);   _Plot(x2-2,mp);
                _APen(pn[FV_Pen_HalfShine]);  _Plot(x1+1,mp); _Plot(x2-3,mp);
                _APen(pn[FV_Pen_Shine]);      _Move(x1+2,mp); _Draw(x2-4,mp);
                _APen(pn[FV_Pen_HalfShadow]); _Plot(x2-1,mp);
                _APen(pn[FV_Pen_Shadow]);     _Plot(x2,mp);
                mp -= 1;
             }

             if (s > 3)
             {
                y2 -= 1;
                _Move(x1+1,y2); _Draw(x2-1,y2);
                _APen(pn[FV_Pen_HalfDark]);   _Plot(x2,y2);
                _APen(pn[FV_Pen_HalfShadow]); _Plot(x1,y2);
                y2 += 1;
             }

             if (s > 4)
             {
                y2 -= 2;
                _Plot(x1,y2); _Plot(x2-2,y2);
                _APen(pn[FV_Pen_HalfDark]);  _Plot(x2,y2);
                _APen(pn[FV_Pen_Shadow]);    _Plot(x2-1,y2);
                _APen(pn[FV_Pen_Fill]);      _Plot(x1+1,y2); _Plot(x2-3,y2);
                _APen(pn[FV_Pen_HalfShine]); _Move(x1+2,y2); _Draw(x2-4,y2);
                y2 += 2;
             }

             if (s > 5)
             {
                mp += 2;
                _Plot(x1+2,mp); _Plot(x2-4,mp);
                _APen(pn[FV_Pen_HalfDark]);   _Plot(x2,mp);
                _APen(pn[FV_Pen_Shadow]);     _Plot(x2-1,mp);
                _APen(pn[FV_Pen_HalfShadow]); _Plot(x1,mp);   _Plot(x2-2,mp);
                _APen(pn[FV_Pen_Fill]);       _Plot(x1+1,mp); _Plot(x2-3,mp);
                _APen(pn[FV_Pen_Shine]);      _Move(x1+3,mp); _Draw(x2-5,mp);
                mp -= 2;
             }

             if (s > 6)
             {
                y2 -= 3;
                mp += 3;
                _Boxf(x1+3,mp,x2-5,y2);
                _APen(pn[FV_Pen_HalfDark]);   _Move(x2,mp);   _Draw(x2,y2);
                _APen(pn[FV_Pen_Shadow]);     _Move(x2-1,mp); _Draw(x2-1,y2);
                _APen(pn[FV_Pen_HalfShadow]); _Move(x2-2,mp); _Draw(x2-2,y2); _Move(x1,mp)   ; _Draw(x1,y2);
                _APen(pn[FV_Pen_Fill]);       _Move(x2-3,mp); _Draw(x2-3,y2); _Move(x1+1,mp) ; _Draw(x1+1,y2);
                _APen(pn[FV_Pen_HalfShine]);  _Move(x2-4,mp); _Draw(x2-4,y2); _Move(x1+2,mp) ; _Draw(x1+2,y2);
                mp -= 3;
             }
             y2 = mp - 1;
          }
       }
    }

_Gauge_Draw_Fill:

    F_Erase(Obj,x1,y1,x2,y2,0);
}
//+

/*** Preferences ***********************************************************/

STATIC FPreferenceScript Script[] =
{
    { "$gauge-frame",         FV_TYPE_STRING, "Spec", "<frame id='18' />",0   },
    { "$gauge-back",          FV_TYPE_STRING, "Spec", "halfshadow",0    },
    { "$gauge-back-vertical", FV_TYPE_STRING, "Spec", "halfshadow",0    },
    { "$gauge-scheme",        FV_TYPE_STRING, "Spec", NULL,0            },

    F_ARRAY_END
};

///Prefs_New
F_METHOD_NEW(Prefs_New)
{
    struct p_LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_XML_REFS_INIT(1);
    F_XML_REFS_ADD("example",&LOD -> Example);
    F_XML_REFS_DONE;
  
    F_AREA_SAVE_PUBLIC;

    LOD -> Val                       = 1;
    LOD -> SignalHandler.Object      = Obj;
    LOD -> SignalHandler.Method      = FM_Gauge_Update,
    LOD -> SignalHandler.Flags       = FF_SignalHandler_Timer;
    LOD -> SignalHandler.fsh_Secs    = 0;
    LOD -> SignalHandler.fsh_Micros  = 30000;

    return (FObject) F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Gauge",

        "Script",         Script,
        "XMLSource",      "feelin/preference/gauge.xml",
        "XMLReferences",  F_XML_REFS,

        TAG_MORE,Msg);
}
//+
///Prefs_Show
F_METHOD_SHOW(Prefs_Show)
{
   struct p_LocalObjectData *LOD = F_LOD(Class,Obj);

   if (_render)
   {
      F_Do(_app,FM_Application_AddSignalHandler,&LOD -> SignalHandler);
   }

   return F_SUPERDO();
}
//+
///Prefs_Hide
F_METHOD_HIDE(Prefs_Hide)
{
   struct p_LocalObjectData *LOD = F_LOD(Class,Obj);

   F_SUPERDO();

   if (_render)
   {
      F_Do(_app,FM_Application_RemSignalHandler,&LOD -> SignalHandler);
   }
}
//+
///Prefs_Update
F_METHOD(LONG,Prefs_Update)
{
   struct p_LocalObjectData *LOD = F_LOD(Class,Obj);

   LONG min,max,val;

   F_Do(LOD -> Example,FM_Get, "Min",&min, "Max",&max, TAG_DONE);
   val = F_Do(LOD -> Example,(ULONG) "Increase",LOD -> Val);

   if (val == min)
   {
      LOD -> Val = 1;
   }
   else if (val == max)
   {
      LOD -> Val = -1;
   }

   return TRUE; // If we return FALSE the timer event won't be requested again
}
//+

