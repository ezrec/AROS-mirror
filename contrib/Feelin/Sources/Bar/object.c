#include "Private.h"

/*** Methods ***************************************************************/

///Bar_New
F_METHOD(uint32,Bar_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    F_AREA_SAVE_PUBLIC;

    LOD -> p_PreParse  = "$bar-preparse";
    LOD -> p_ColorUp   = "$bar-color-shadow";
    LOD -> p_ColorDown = "$bar-color-shine";
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Bar_Title:      LOD -> Title    = (STRPTR)(item.ti_Data); break;
        case FA_Bar_PreParse:   LOD -> PreParse = (STRPTR)(item.ti_Data); break;
    }

    return F_SuperDo(Class,Obj,Method,

        FA_Font,          "$bar-font",
        FA_ChainToCycle,  FALSE,
        (LOD -> Title) ? FA_SetMax : TAG_IGNORE, FV_SetHeight,
        
    TAG_MORE, Msg);
}
//+
///Bar_Get
F_METHOD(void,Bar_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg, item;
    
    BOOL up=FALSE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Bar_Title:      F_STORE(LOD -> Title);    break;
        case FA_Bar_PreParse:   F_STORE(LOD -> PreParse); break;

        default: up = TRUE;
    }

    if (up) F_SUPERDO();
}
//+
///Bar_Setup
F_METHOD(uint32,Bar_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        uint32 data;
        
        if (F_Get(_parent,FA_Horizontal)) LOD -> Flags |= FF_Bar_Vertical;
        else                              LOD -> Flags &= ~FF_Bar_Vertical;
            
        data = F_Do(_app,FM_Application_Resolve,LOD -> p_ColorUp,NULL);
  
        if (data)
        {
            LOD -> ColorUp = (FColor *) F_Do(_display,FM_CreateColor,data,F_Get(Obj,FA_ColorScheme));
        }
    
        data = F_Do(_app,FM_Application_Resolve,LOD -> p_ColorDown,NULL);

        if (data)
        {
            LOD -> ColorDown = (FColor *) F_Do(_display,FM_CreateColor,data,F_Get(Obj,FA_ColorScheme));
        }
            
        if (LOD -> Title)
        {
            LOD -> PreParse = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> p_PreParse,NULL);

            LOD -> TD = TextDisplayObject,

                FA_TextDisplay_Contents,  LOD -> Title,
                FA_TextDisplay_PreParse,  LOD -> PreParse,
                FA_TextDisplay_Font,      _font,

                End;

            if (LOD -> TD)
            {
                return F_Do(LOD -> TD,FM_TextDisplay_Setup,_render);
            }
        }
        return TRUE;
    }
    return FALSE;
}
//+
///Bar_Cleanup
F_METHOD(uint32,Bar_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        if (LOD -> ColorUp)
        {
            F_Do(_display,FM_RemColor,LOD -> ColorUp); LOD -> ColorUp = NULL;
        }
     
        if (LOD -> ColorDown)
        {
            F_Do(_display,FM_RemColor,LOD -> ColorDown); LOD -> ColorDown = NULL;
        }

        if (LOD -> TD)
        {
            F_Do(LOD -> TD,FM_TextDisplay_Cleanup);
            F_DisposeObj(LOD -> TD); LOD -> TD = NULL;
        }
    }
    return F_SUPERDO();
}
//+
///Bar_AskMinMax
F_METHOD(uint32,Bar_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Title)
    {
        if ((LOD -> Flags & FF_Bar_Vertical) == FALSE)
        {
            _minh += F_Get(LOD -> TD,FA_TextDisplay_Height);
        }
    }
    else
    {
        if (LOD -> Flags & FF_Bar_Vertical)
        {
            _minw += 2;
            _maxw  = _minw;
        }
        else
        {
            _minh += 2;
            _maxh  = _minh;
        }
    }
    return F_SUPERDO();
}
//+
///Bar_Draw
F_METHODM(void,Bar_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort *rp = _rp;
    uint32 dark,shine;
    uint16 x1,y1,x2,y2,w,h, bw;
    
    F_SUPERDO();

    x1 = _x; w = _w; x2 = x1 + w - 1; 
    y1 = _y; h = _h; y2 = y1 + h - 1; 
                            
    dark = (LOD -> ColorUp) ? LOD -> ColorUp -> Pen : _pens[FV_Pen_Dark];
    shine = (LOD -> ColorDown) ? LOD -> ColorDown -> Pen : _pens[FV_Pen_Shine];
 
    if (LOD -> Flags & FF_Bar_Vertical)
    {
        if (w > 2) x1 = w / 2 + x1 - 1;

        _APen(dark);  _Move(x1,y1);   _Draw(x1,y2);
        _APen(shine); _Move(x1+1,y1); _Draw(x1+1,y2);
    }
    else
    {
        if (LOD -> Title)
        {
            FRect rect;
            uint32 td_w=0,td_h=0;

            rect.x1 = x1+10+1; rect.y1 = y1;
            rect.x2 = x2-10-1; rect.y2 = y2;

            F_Do(LOD -> TD,FM_Set,
                                FA_TextDisplay_Width,   rect.x2 - rect.x1 + 1,
                                FA_TextDisplay_Height,  rect.y2 - rect.y1 + 1,
                                TAG_DONE);

            F_Do(LOD -> TD,FM_Get,
                                FA_TextDisplay_Width,   &td_w,
                                FA_TextDisplay_Height,  &td_h,
                                TAG_DONE);

            if (td_w == 0)
            {
                goto __done;
            }

            if (td_h < (y2 - y1 + 1))
            {
                rect.y1 = ((y2 - y1 + 1) - td_h) / 2 + rect.y1;
            }
 
            bw = (w - 10 - td_w) / 2;
            y1 = (h / 2) + y1;

            _APen(shine); _Move(x1,y1);   _Draw(x1+bw-1,y1);   _Move(x2-bw+1,y1);   _Draw(x2,y1);
            _APen(dark);  _Move(x1,y1-1); _Draw(x1+bw-1,y1-1); _Move(x2-bw+1,y1-1); _Draw(x2,y1-1);

            rect.x1 = x1+bw+5; rect.x2 = x2-bw-5;

            F_Do(LOD -> TD,FM_TextDisplay_Draw,&rect);
        }
        else
        {
__done:
            if (h > 2) y1 = h / 2 + y1 - 1;

            _APen(dark);  _Move(x1,y1);   _Draw(x2,y1);
            _APen(shine); _Move(x1,y1+1); _Draw(x2,y1+1);
        }
    }
}
//+

STATIC FPreferenceScript Script[] =
{
    { "$bar-font",         FV_TYPE_STRING, "Contents", NULL,0     },
    { "$bar-preparse",     FV_TYPE_STRING, "Contents", NULL,0     },
    { "$bar-color-shadow", FV_TYPE_STRING, "Spec",     "dark",0   },
    { "$bar-color-shine",  FV_TYPE_STRING, "Spec",     "shine",0  },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Bar",

        "Script", Script,
        "XMLSource", "feelin/preference/bar.xml",

    TAG_MORE,Msg);
}

