#include "Private.h"

/*** Private ***************************************************************/

///Slider_DrawKnob
STATIC void Slider_DrawKnob(FClass *Class,FObject Obj,ULONG Flags)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Draw(LOD -> Knob,Flags);

    if (LOD -> OwnBitMap && (FF_Area_CanDraw && _flags))
    {
        BltBitMapRastPort(LOD -> OwnBitMap,0,0,_rp,LOD -> kx,LOD -> ky,LOD -> kw,LOD -> kh,0x0C0);
    }
}
//+
///Slider_KnobState
STATIC void Slider_KnobState(FClass *Class,FObject Obj,BOOL State)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Set(LOD -> Knob,FA_Selected,State);

    if (LOD -> OwnBitMap && (FF_Area_CanDraw && _flags))
    {
        BltBitMapRastPort(LOD -> OwnBitMap,0,0,_rp,LOD -> kx,LOD -> ky,LOD -> kw,LOD -> kh,0x0C0);
    }
}
//+

/*** Methods ***************************************************************/

///Slider_Setup
F_METHOD(ULONG,Slider_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_DebugOut("Slider.Setup\n");

    if (F_SUPERDO())
    {
        ULONG data;
        
        data = F_Do(_app,FM_Application_Resolve,LOD -> p_knob_preparse,"<align=center>");
        
        F_Set(LOD -> Knob,FA_Text_PreParse,data);
 
        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

        if (FF_Slider_Buffer & LOD -> Flags)
        {
            if ((LOD -> OwnRPort = F_New(sizeof (struct RastPort))) != NULL)
            {
                InitRastPort(LOD -> OwnRPort);

                LOD -> OwnRender = RenderObject,

                    FA_Render_Friend,  _render,
                    FA_Render_RPort,   LOD -> OwnRPort,

                    End;
                    
                if (LOD -> OwnRender)
                {
                    return F_Do(LOD -> Knob,Method,LOD -> OwnRender);
                }

    	    #ifdef __AROS
	        DeinitRastPort( LOD -> OwnRPort);
	    #endif
                F_Dispose(LOD -> OwnRPort);
            }
        }

        return F_OBJDO(LOD -> Knob);
    }
    return FALSE;
}
//+
///Slider_Cleanup
F_METHOD(void,Slider_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_DebugOut("Slider.Cleanup\n");

    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);

    if (LOD -> OwnRender)
    {
        F_Do(LOD -> Knob,Method,LOD -> OwnRender);
        F_DisposeObj(LOD -> OwnRender); LOD -> OwnRender = NULL;
	
    #ifdef __AROS__
    	DeinitRastPort(LOD -> OwnRPort);
    #endif
        F_Dispose(LOD -> OwnRPort);     LOD -> OwnRPort  = NULL;

        if (LOD -> OwnBitMap)
        {
            FreeBitMap(LOD -> OwnBitMap); LOD -> OwnBitMap = NULL;
        }
    }
    else
    {
        F_OBJDO(LOD -> Knob);
    }

    F_SUPERDO();
}
//+
///Slider_Show
F_METHOD(ULONG,Slider_Show)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        slider_cache_update(Class,Obj);

        return F_OBJDO(LOD -> Knob);
    }
    return FALSE;
}
//+
///Slider_Hide
F_METHOD(void,Slider_Hide)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_OBJDO(LOD -> Knob);
    F_SUPERDO();
}
//+
///Slider_Draw
F_METHODM(void,Slider_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    ULONG val = F_Get(Obj,F_IDR(FA_Numeric_Value));
    
    ULONG x1 = _ix,y1 = _iy,x2 = _ix2,y2 = _iy2, ox1,oy1,ox2,oy2;

    if (LOD -> last_drawn_value != val)
    {
        LOD -> last_drawn_value = val;

        F_Set(LOD -> Knob,FA_Text,F_Get(Obj,F_IDR(FA_Numeric_Buffer)));
    }

    if (FF_Draw_Object & Msg -> Flags)
    {
        F_SUPERDO();
    }
 
    if (((FF_Draw_Object & Msg -> Flags) && LOD -> LastLayoutValue != val) ||
         !(FF_Slider_Scroll & LOD -> Flags))
    {
        F_ClassDo(Class,Obj,FM_Layout);
    }

    ox1 = LOD -> kx; ox2 = ox1 + LOD -> kw - 1;
    oy1 = LOD -> ky; oy2 = oy1 + LOD -> kh - 1;
    
    if (FF_Draw_Update & Msg -> Flags)
    {
        if (FF_Draw_MoveKnob & Msg -> Flags)
        {
            if (FF_Horizontal & _flags)
            {
                ClipBlit(_rp,LOD -> PreviousPos,oy1,_rp,ox1,oy1,ox2 - ox1 + 1,oy2 - oy1 + 1,0x0C0);
            }
            else
            {
                ClipBlit(_rp,ox1,LOD -> PreviousPos,_rp,ox1,oy1,ox2 - ox1 + 1,oy2 - oy1 + 1,0x0C0);
            }
        }
    }

    if ((FF_Draw_Object & Msg -> Flags) || !(FF_Draw_MoveKnob & Msg -> Flags))
    {
        Slider_DrawKnob(Class,Obj,FF_Draw_Object);
    }

    if (FF_Horizontal & _flags)
    {
        if ((LOD -> PreviousPos < ox1) || (FF_Draw_Object & Msg -> Flags)) F_Erase(Obj,x1,y1,ox1-1,y2,0);
        if ((LOD -> PreviousPos > ox1) || (FF_Draw_Object & Msg -> Flags)) F_Erase(Obj,ox2+1,y1,x2,y2,0);
    }
    else
    {
        if ((LOD -> PreviousPos < oy1) || (FF_Draw_Object & Msg -> Flags)) F_Erase(Obj,x1,y1,x2,oy1-1,0);
        if ((LOD -> PreviousPos > oy1) || (FF_Draw_Object & Msg -> Flags)) F_Erase(Obj,x1,oy2+1,x2,y2,0);
    }
}
//+
///Slider_GoActive
F_METHOD(void,Slider_GoActive)
{
    F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);

    Slider_KnobState(Class,Obj,TRUE);
}
//+
///Slider_GoInactive
F_METHOD(void,Slider_GoInactive)
{
    Slider_KnobState(Class,Obj,FALSE);

    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);
}
//+
///Slider_GoEnabled
F_METHOD(void,Slider_GoEnabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    //F_Set(LOD -> Knob,FA_Disabled,FALSE);
    F_OBJDO(LOD->Knob);
    
    F_SUPERDO();

/*
    if (LOD -> OwnBitMap && (FF_Area_CanDraw && _flags))
    {
        BltBitMapRastPort(LOD -> OwnBitMap,0,0,_rp,LOD -> kx,LOD -> ky,LOD -> kw,LOD -> kh,0x0C0);
    }
*/
}
//+
///Slider_GoDisabled
F_METHOD(void,Slider_GoDisabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_OBJDO(LOD->Knob);
    
    F_SUPERDO();

    //F_Set(LOD -> Knob,FA_Disabled,TRUE);
/*
    if (LOD -> OwnBitMap && (FF_Area_CanDraw && _flags))
    {
        BltBitMapRastPort(LOD -> OwnBitMap,0,0,_rp,LOD -> kx,LOD -> ky,LOD -> kw,LOD -> kh,0x0C0);
    }
*/
}
//+
///Slider_HandleEvent
F_METHODM(ULONG,Slider_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;
    ULONG    v,size;
    UWORD    bnd1,bnd2;
    WORD     npos;

    switch (fev -> Class)
    {
        case FF_EVENT_KEY:
        {
            return F_SUPERDO();
        }

        case FF_EVENT_BUTTON:
        {
            if (fev -> Code == FV_EVENT_BUTTON_SELECT)
            {
                if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                {
                    if (_inside(fev -> MouseX,_ix,_ix2) && _inside(fev -> MouseY,_iy,_iy2))
                    {
                        LONG val = F_Get(Obj,F_IDR(FA_Numeric_Value));

                        if (_inside(fev -> MouseX,LOD -> kx,LOD -> kx + LOD -> kw - 1)
                         && _inside(fev -> MouseY,LOD -> ky,LOD -> ky + LOD -> kh -1))
                        {
                            LOD -> MouseOff = 1 + ((FF_Horizontal & _flags) ? fev -> MouseX - LOD -> kx : fev -> MouseY - LOD -> ky);
                            LOD -> Saved    = val;
                            LOD -> Flags   |= FF_Slider_Scroll;

                            F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                            Slider_KnobState(Class,Obj,TRUE);
                            F_SuperDo(Class,Obj,FM_Set,FA_Pressed,TRUE,TAG_DONE);
                        }
                        else
                        {
                            if (FF_Horizontal & _flags) v = val + ((fev -> MouseX < LOD -> kx) ? -1 : 1);
                            else                        v = val + ((fev -> MouseY < LOD -> ky) ? -1 : 1);

                            F_Set(Obj,F_IDR(FA_Numeric_Value),v);
                        }
                        return FF_HandleEvent_Eat;
                    }
                }
                else if (FF_Slider_Scroll & LOD -> Flags)
                {
                    F_Do(Obj,FM_ModifyHandler,NULL,FF_EVENT_MOTION);
                    if (!(FF_Area_Active & _flags)) Slider_KnobState(Class,Obj,FALSE);
                    LOD -> Flags &= ~FF_Slider_Scroll;
                    F_SuperDo(Class,Obj,FM_Set,FA_Pressed,FALSE,TAG_DONE);
                }
            }
            else if (fev -> Code == FV_EVENT_BUTTON_MENU)
            {
                if ((FF_EVENT_BUTTON_DOWN & fev -> Flags) && (FF_Slider_Scroll & LOD -> Flags))
                {
                    LOD -> Flags &= ~FF_Slider_Scroll;

                    if (!(FF_Area_Active & _flags)) Slider_KnobState(Class,Obj,FALSE);

                    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);
                    F_Set(Obj,F_IDR(FA_Numeric_Value),LOD -> Saved);
                    F_SuperDo(Class,Obj,FM_Set,FA_Pressed,FALSE,FA_NoNotify,TRUE,TAG_DONE);

                    return FF_HandleEvent_Eat;
                }
            }
            else
            {
                return F_SUPERDO();
            }
        }
        break;

        case FF_EVENT_MOTION:
        {
            if (FF_Slider_Scroll & LOD -> Flags)
            {
                LONG val,min,max;

                F_Do(Obj,FM_Get,
                            F_IDR(FA_Numeric_Value),   &val,
                            F_IDR(FA_Numeric_Min),     &min,
                            F_IDR(FA_Numeric_Max),     &max, TAG_DONE);

                if (FF_Horizontal & _flags)
                {
                    size = LOD -> kw;
                    bnd1 = _ix;
                    bnd2 = bnd1 + _iw - 1;
                    npos = fev -> MouseX - LOD -> MouseOff + 1;
                }
                else
                {
                    size = LOD -> kh;
                    bnd1 = _iy;
                    bnd2 = bnd1 + _ih - 1;
                    npos = fev -> MouseY - LOD -> MouseOff + 1;
                }

                if (npos < bnd1)
                {
                    if (val > min)
                    {
                        F_Set(Obj,F_IDR(FA_Numeric_Value),min);
                        return FF_HandleEvent_Eat;
                    }
                    npos = bnd1;
                }

                if (npos > (bnd2 - size + 1))
                {
                    if (val < max)
                    {
                        F_Set(Obj,F_IDR(FA_Numeric_Value),max);
                        return FF_HandleEvent_Eat;
                    }
                    npos = bnd2 - size + 1;
                }

                if (npos != (FF_Horizontal & _flags ? LOD -> kx : LOD -> ky))
                {
                    v = (npos - bnd1) * (max - min) / (bnd2 - bnd1 + 1 - size) + min;

                    if (FF_Horizontal & _flags)
                    {
                        LOD -> PreviousPos = LOD -> kx ; LOD -> kx = npos;

                        if (LOD -> OwnBitMap == NULL)
                        {
                            _knob_x = npos;
                            _knob_w = size;
                        }
                    }
                    else
                    {
                        LOD -> PreviousPos = LOD -> ky ; LOD -> ky = npos;

                        if (LOD -> OwnBitMap == NULL)
                        {
                            _knob_y = npos;
                            _knob_h = size;
                        }
                    }

//               F_DebugOut('Pos \d (\d,\d)\n',npos,bnd1,bnd2)

                    if (v != val)  F_Set(Obj,F_IDR(FA_Numeric_Value),v);
                    else           F_Draw(Obj,FF_Draw_Update | FF_Draw_MoveKnob);
                }
                return FF_HandleEvent_Eat;
            }
        }
        break;
    }
    return NULL;
}
//+

