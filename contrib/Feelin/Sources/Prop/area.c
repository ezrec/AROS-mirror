#include "Private.h"

///Prop_Setup
F_METHOD(LONG,Prop_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

      if (F_OBJDO(LOD -> Knob)) return TRUE;
   }
   return FALSE;
}
//+
///Prop_Cleanup
F_METHOD(void,Prop_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_OBJDO(LOD -> Knob);
   F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);
   F_SUPERDO();
}
//+
///Prop_Show
F_METHOD(ULONG,Prop_Show)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      return F_OBJDO(LOD -> Knob);
   }
   return FALSE;
}
//+
///Prop_Hide
F_METHOD(void,Prop_Hide)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_OBJDO(LOD -> Knob);
   F_SUPERDO();
}
//+
///Prop_AskMinMax
F_METHOD(ULONG,Prop_AskMinMax)
{
   struct LocalObjectData  *LOD = F_LOD(Class,Obj);

   if (_knob_area_public)
   {
      _knob_minw = 0; _knob_maxw = FV_MAXMAX;
      _knob_minh = 0; _knob_maxh = FV_MAXMAX;

      F_Do(LOD -> Knob,FM_AskMinMax);
   }

   if (FF_Horizontal & _flags)
   {
      _minw += 10 + _knob_minw;
      _minh +=  4 + _knob_minh;
   }
   else
   {
      _minw +=  4 + _knob_minw;
      _minh += 10 + _knob_minh;
   }

   return F_SUPERDO();
}
//+
///Prop_Layout
F_METHOD(void,Prop_Layout)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   WORD  x1 = _ix, x2 = x1 + _iw - 1,
         y1 = _iy, y2 = y1 + _ih - 1;
   UWORD w = FF_Horizontal & _flags ? x2 - x1 + 1 : y2 - y1 + 1;
   ULONG s,p;

   LOD -> PreviousPos = (FF_Horizontal & _flags ? _knob_x : _knob_y);

   if (LOD -> Entries)
   {
      s = LOD -> Visible * w / LOD -> Entries;

      if (LOD -> First == 0)
      {
         p = FF_Horizontal & _flags ? x1 : y1;
      }
      else if (LOD -> First + LOD -> Visible == LOD -> Entries)
      {
         p = FF_Horizontal & _flags ? x2 - s + 1 : y2 - s + 1;
      }
      else
      {
         p = w * LOD -> First / LOD -> Entries;
         p = FF_Horizontal & _flags ? MIN(x2 - s + 1, p + x1) : MIN(y2 - s + 1, p + y1);
      }
   }
   else
   {
      s = w;
      p = (FF_Horizontal & _flags) ? x1 : y1;
   }

   if (FF_Horizontal & _flags)
   {
      _knob_x = p;
      _knob_y = y1;
      _knob_w = MAX(s,_knob_minw);
      _knob_h = y2 - y1 + 1;
   }
   else
   {
      _knob_x = x1;
      _knob_y = p;
      _knob_w = x2 - x1 + 1;
      _knob_h = MAX(s,_knob_minh);
   }

   F_Do(LOD -> Knob,FM_Layout);
}
//+
///Prop_Draw
F_METHODM(void,Prop_Draw,FS_Draw)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   WORD x1 = _ix, x2 = x1 + _iw - 1,
        y1 = _iy, y2 = y1 + _ih - 1,
        
        ox1,oy1,ox2,oy2;

   F_SUPERDO();

   if (!(FF_Prop_Scroll & LOD -> Flags))
   {
      F_ClassDo(Class,Obj,FM_Layout);
   }

   ox1 = _knob_x, ox2 = ox1 + _knob_w - 1,
   oy1 = _knob_y, oy2 = oy1 + _knob_h - 1;

   if (FF_Draw_Update & Msg -> Flags)
   {
      if (FF_Prop_Scroll & LOD -> Flags)
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

   if ((FF_Draw_Object & Msg -> Flags) ||
      ((FF_Draw_Update & Msg -> Flags) && !(FF_Prop_Scroll & LOD -> Flags)))
   {
      F_Draw(LOD -> Knob,FF_Draw_Object);
   }

   if (FF_Horizontal & _flags)
   {
      if ((FF_Draw_Object & Msg -> Flags) || (LOD -> PreviousPos < ox1)) F_Erase(Obj,x1,y1,ox1-1,y2,0);
      if ((FF_Draw_Object & Msg -> Flags) || (LOD -> PreviousPos > ox1)) F_Erase(Obj,ox2+1,y1,x2,y2,0);
   }
   else
   {
      if ((FF_Draw_Object & Msg -> Flags) || (LOD -> PreviousPos < oy1)) F_Erase(Obj,x1,y1,x2,oy1-1,0);
      if ((FF_Draw_Object & Msg -> Flags) || (LOD -> PreviousPos > oy1)) F_Erase(Obj,x1,oy2+1,x2,y2,0);
   }
}
//+
///Prop_GoActive
F_METHOD(void,Prop_GoActive)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);
   F_Set(LOD -> Knob,FA_Selected,TRUE);
}
//+
///Prop_GoInactive
F_METHOD(void,Prop_GoInactive)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Set(LOD -> Knob,FA_Selected,FALSE);
   F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);
}
//+
///Prop_GoDisabled
F_METHOD(void, Prop_GoDisabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_SUPERDO();

    F_OBJDO(LOD->Knob);
}
//+
///Prop_GoEnabled
F_METHOD(void, Prop_GoEnabled)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
                    
    F_SUPERDO();
    
    F_OBJDO(LOD->Knob);
}
//+
///Prop_HandleEvent
F_METHODM(ULONG,Prop_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;
    uint32 val,size;

    switch (fev -> Class)
    {
        case FF_EVENT_KEY:
        {
           if (FF_Horizontal & _flags)
           {
              switch (fev -> Key)
              {
                 case FV_KEY_LEFT:       val = LOD -> First - 1;                break;
                 case FV_KEY_RIGHT:      val = LOD -> First + 1;                break;
                 case FV_KEY_STEPLEFT:   val = LOD -> First - LOD -> Visible;   break;
                 case FV_KEY_STEPRIGHT:  val = LOD -> First + LOD -> Visible;   break;
                 case FV_KEY_FIRST:      val = 0;                               break;
                 case FV_KEY_LAST:       val = LOD -> Entries - LOD -> Visible; break;
                 default:                return 0;
              }
           }
           else
           {
              switch (fev -> Key)
              {
                 case FV_KEY_UP:         val = LOD -> First - 1;                break;
                 case FV_KEY_DOWN:       val = LOD -> First + 1;                break;
                 case FV_KEY_STEPUP:     val = LOD -> First - LOD -> Visible;   break;
                 case FV_KEY_STEPDOWN:   val = LOD -> First + LOD -> Visible;   break;
                 case FV_KEY_TOP:        val = 0;                               break;
                 case FV_KEY_BOTTOM:     val = LOD -> Entries - LOD -> Visible; break;
                 default:                return 0;
              }
           }

           F_Set(Obj,F_IDA(FA_Prop_First),val);

           return FF_HandleEvent_Eat;
        }
        break;

        case FF_EVENT_BUTTON:
        {
            if (fev -> Code == FV_EVENT_BUTTON_SELECT)
            {
                if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                {
                    if (_inside(fev -> MouseX,_ix,_ix2) &&
                        _inside(fev -> MouseY,_iy,_iy2))
                    {
                        if (_inside(fev -> MouseX,_knob_x,_knob_x + _knob_w - 1) &&
                            _inside(fev -> MouseY,_knob_y,_knob_y + _knob_h - 1))
                        {
                            LOD -> MouseOff = 1 + ((FF_Horizontal & _flags) ? fev -> MouseX - _knob_x : fev -> MouseY - _knob_y);
                            LOD -> Saved    = LOD -> First;
                            LOD -> Flags   |= FF_Prop_Scroll;

                            F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION,0);
                            F_Set(LOD -> Knob,FA_Selected,TRUE);
                        }
                        else
                        {
                            if (FF_Horizontal & _flags)
                            {
                                val = (fev -> MouseX < _knob_x) ? (-LOD -> Visible) : (LOD -> Visible);
                            }
                            else
                            {
                                val = (fev -> MouseY < _knob_y) ? (-LOD -> Visible) : (LOD -> Visible);
                            }

                            F_Set(Obj,F_IDA(FA_Prop_First),LOD -> First + val);
                        }
                        return FF_HandleEvent_Eat;
                    }
                }
                else
                {
                    if (FF_Prop_Scroll & LOD -> Flags)
                    {
                        F_Do(Obj,FM_ModifyHandler,NULL,FF_EVENT_MOTION);
                        
                        if (!(FF_Area_Active & _flags))
                        {
                            F_Set(LOD -> Knob,FA_Selected,FALSE);
                        }
                         
                        LOD -> Flags &= ~FF_Prop_Scroll;
                    }
                }
                break;
            }
            else if ((fev -> Code == FV_EVENT_BUTTON_MENU) && (FF_EVENT_BUTTON_DOWN & fev -> Flags))
            {
                if (FF_Prop_Scroll & LOD -> Flags)
                {
                    LOD -> Flags &= ~FF_Prop_Scroll;

                    if (!(FF_Area_Active & _flags)) F_Set(LOD -> Knob,FA_Selected,FALSE);

                    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION);
                    F_Set(Obj,F_IDA(FA_Prop_First),LOD -> Saved);

                    return FF_HandleEvent_Eat;
                }
            }
            else if (fev -> Code == FV_EVENT_BUTTON_WHEEL)
            {
                if (_inside(fev -> MouseX,_x,_x2) &&
                    _inside(fev -> MouseY,_y,_y2))
                {
                    if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                    {
                        F_Do(Obj,F_IDM(FM_Prop_Increase),LOD -> Step);
                    }
                    else
                    {
                        F_Do(Obj,F_IDM(FM_Prop_Decrease),LOD -> Step);
                    }
                }
            }
        }
        break;

        case FF_EVENT_MOTION:
        {
            int32 bnd1,bnd2,npos;

            if (FF_Prop_Scroll & LOD -> Flags)
            {
                if (FF_Horizontal & _flags)
                {
                    size = _knob_w;
                    bnd1 = _ix;
                    bnd2 = _ix2;
                    npos = fev -> MouseX - LOD -> MouseOff + 1;
                }
                else
                {
                    size = _knob_h;
                    bnd1 = _iy;
                    bnd2 = _iy2;
                    npos = fev -> MouseY - LOD -> MouseOff + 1;
                }

                if (npos < bnd1)             npos = bnd1;
                if (npos > bnd2 - size + 1)  npos = bnd2 - size + 1;

                if (npos != (FF_Horizontal & _flags ? _knob_x : _knob_y))
                {
    //               val = (npos - bnd1) * LOD -> Entries / (bnd2 - bnd1 + 1);
                    val = (npos - bnd1) * (LOD -> Entries - LOD -> Visible) / (bnd2 - bnd1 + 1 - size);

                    if (FF_Horizontal & _flags)
                    {
                        LOD -> PreviousPos = _knob_x;
                        _knob_x = npos;
                        _knob_w = size;
                    }
                    else
                    {
                        LOD -> PreviousPos = _knob_y;
                        _knob_y = npos;
                        _knob_h = size;
                    }

                    if (val != LOD -> First)
                    {
                        F_Set(Obj,F_IDA(FA_Prop_First),val);
                    }
                    else
                    {
                        F_Draw(Obj,FF_Draw_Update);
                    }
                }
                return FF_HandleEvent_Eat;
            }
        }
        break;
    }
    return 0;
}
//+
