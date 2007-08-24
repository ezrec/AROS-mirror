#include "Private.h"

//#define DB_DRAW

///String_ClipText
void String_ClipText(struct FeelinClass *Class,FObject Obj,struct RastPort *rp,ULONG w, ULONG cur,ULONG len,ULONG cw,struct FeelinString_TextFit *Fit)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    ULONG       pos = Fit -> Position;
    STRPTR      str = Fit -> String;
    UWORD       tw  = Fit -> TextWidth;
    UWORD       dc;
    ULONG       a;

    if (FF_Area_Selected & _flags && cur == len) tw += cw;

    if (tw <= w)
    {
        dc    = len;
        pos   = 0;
        Fit -> Clip = FALSE;
    }
    else
    {
        Fit -> Clip = TRUE;

        if (cur < pos)
        {
            pos = cur;
        }
        else
        {
            dc = 0 ; tw = 0;
            while ((tw += TextLength(rp,str+pos+dc,1)) <= w)
            {
                if (pos + dc == len) break; dc++;
            }

            if ((pos + dc) == len) // dc déjà au minimum
            {
                while (TextLength(rp,str+len-dc,dc) <= (w - ((cur == len) ? cw : 0))) dc++;
                pos = len - dc + 1;
            }
            else if (TextLength(rp,str+pos,cur-pos) > (w-cw))
            {
                dc = 1;
                while (TextLength(rp,str+cur-dc,dc) <= (w-cw)) dc++;
                pos = cur - dc + 1;
            }
        }

        str += pos;

//    Troncage

        dc = 0; tw = 0; /*a = 0;*/
        while ((a = TextLength(rp,str+dc,1)) <= (w-tw))
        {
            if ((pos + dc) == len) break; dc++; tw += a;
        }

        if (FF_Area_Selected & _flags && cur == len) tw += cw;
    }

    Fit -> Displayable   = dc;
    Fit -> Position      = pos;
    Fit -> String        = str;
    Fit -> TextWidth     = tw;
}
//+

/*** Methods ***************************************************************/

///String_Draw
F_METHODM(void,String_Draw,FS_Draw)
{
    struct LocalObjectData       *LOD = F_LOD(Class,Obj);
    struct RastPort              *rp  = _rp;
    struct FeelinString_TextFit   fit;
    int32 x1  = _ix, x2 = x1 + _iw - 1;
    int32 y1  = _iy, y2 = y1 + _ih - 1;
    uint32 pos = LOD -> Pos;
    uint32 len = LOD -> Len;
    uint32 cur = LOD -> Cur;
    uint32 old = LOD -> Old;
    STRPTR str = LOD -> String;
    int32 tx,cx;
    uint32 tw,cw;
    uint32 repair=REPAIR_NOTHING;

    F_SUPERDO();

 
/*
    if (FF_Draw_Object & Msg -> Flags)
    {
        old = 0;
    }
*/
    y1 = (y2 - y1 + 1 - _font -> tf_YSize) / 2 + y1; /* Center string vertically */
    y2 = y1 + _font -> tf_YSize - 1;
/*
    if (!(FF_Draw_Blink & Msg -> Flags))
    {

        if (cur == len)   cw = TextLength(rp," ",1);
        else              cw = TextLength(rp,str + cur,MIN(LOD -> Sel,LOD -> Dsp));

        LOD -> CursorW = cw;
    }
*/
    if (!((Msg -> Flags & (FF_Draw_Move | FF_Draw_Blink)))) LOD -> TextW = TextLength(rp,str,len);

    cw = LOD -> CursorW;
    tw = LOD -> TextW;

///DB_DRAW
#ifdef DB_DRAW
    F_DebugOut("String.Draw - Pos %03ld (%03ld,%03ld) - Cur %03ld (%lc - %03ld,%03ld) - Old %03ld - Len %03ld - Sel %03ld - Op ",pos,LOD -> TextX,LOD -> TextW,cur,(cur == len) ? '#' : *((STRPTR)(str+cur)),LOD -> CursorX,LOD -> CursorW,old,len,LOD -> Sel);
    if (FF_Draw_Move & Msg -> Flags)      F_DebugOut("Move");
    if (FF_Draw_Insert & Msg -> Flags)    F_DebugOut("Insert");
    if (FF_Draw_Backspace & Msg -> Flags) F_DebugOut("Back");
    if (FF_Draw_Delete & Msg -> Flags)    F_DebugOut("Del");
    if (FF_Draw_Blink & Msg -> Flags)     F_DebugOut("Blink");
    F_DebugOut("\n");
#endif
//+

    SetFont(rp,_font);

///FF_Draw_Blink
    if (FF_Draw_Blink & Msg -> Flags)
    {
        APTR imobj = (FF_String_Blink & LOD -> Flags) ? LOD -> Blink : LOD -> Cursor;
        cx = LOD -> CursorX;

        if (imobj)
        {
            FRect rect;

            rect.x1 = cx ; rect.x2 = cx + cw - 1;
            rect.y1 = y1 ; rect.y2 = y2;

            F_Do(imobj,FM_ImageDisplay_Draw,_render,&rect,0);
        }
        else
        {
            if (FF_String_Blink & LOD -> Flags)
            {
                F_Erase(Obj,cx,y1,cx + cw - 1,y2,0);
            }
            else
            {
                _FPen(FV_Pen_Highlight);
                _Boxf(cx,y1,cx+cw-1,y2);
            }
        }

        if (cur != len)
        {
            _APen((FF_String_Blink & LOD -> Flags) ? LOD -> APen -> Pen : LOD -> BPen -> Pen);
            _Move(cx,y1 + _font -> tf_Baseline);
            _DrMd(JAM1);
            _Text(str+cur,MIN(LOD -> Sel,LOD -> Dsp));
            _DrMd(JAM2);
        }

        return;
    }
//+

    fit.Position  = pos;
    fit.String    = str;
    fit.TextWidth = tw;

    String_ClipText(Class,Obj,rp,x2 - x1 + 1, cur,len,cw, &fit);

    pos = fit.Position;
    str = fit.String;
    tw  = fit.TextWidth;

    switch (LOD -> Justify)
    {
        case FV_String_Center:  tx = (x2 - x1 + 1 - tw) / 2 + x1 - 1; break;
        case FV_String_Right:   tx = x2 - tw + 1; break;
        case FV_String_Left:
        default:                tx = x1; break;
    }

    cx = TextLength(rp,str,cur-pos) + tx;

//   F_DebugOut("Cursor '%lc' - x %ld - w %ld != ",*((STRPTR)(LOD -> String + cur)),cx,cw);

    if (cur == len) cw = TextLength(rp," ",1);
    else            cw = TextLength(rp,LOD -> String + cur,LOD -> Sel);

//   F_DebugOut("%ld (sel %ld)\n",cw,LOD -> Sel);

    LOD -> CursorX = cx;
    LOD -> CursorW = cw; /* FIXME: Is this necessary ?? */
    LOD -> TextX   = tx;
    LOD -> Dsp     = fit.Displayable;
/*
    F_DebugOut("   >> '");
    Write((BPTR) FeelinBase -> Console,LOD -> String + LOD -> Pos,LOD -> Dsp);
    F_DebugOut("'\n");
*/
    if (FF_Draw_Update & Msg -> Flags)
    {
        if (pos != LOD -> Pos)
        {
            repair = REPAIR_ALL;
        }
        else if (!LOD -> Len)
        {
            repair = REPAIR_ALL;
        }
        else if (cur == len || old == len)
        {
            if (LOD -> Justify == FV_String_Left)
            {
                if (FF_Draw_Move & Msg -> Flags)          repair = REPAIR_OLDC;
                else if (FF_Draw_Insert & Msg -> Flags)   repair = REPAIR_FROM_OLDC;
                else                                      repair = REPAIR_FROMCUR;
            }
            else
            {
                repair = REPAIR_ALL;
            }
        }
        else if (FF_Draw_Move & Msg -> Flags)
        {
            repair = REPAIR_OLDC;
        }
        else if (FF_Draw_Insert & Msg -> Flags)
        {
            switch (LOD -> Justify)
            {
                case FV_String_Left:   repair = REPAIR_FROM_OLDC; break;
                case FV_String_Center: repair = REPAIR_ALL; break;
                case FV_String_Right:  repair = (fit.Clip) ? REPAIR_ALL : REPAIR_TOCUR; break;
            }
        }
        else if ((FF_Draw_Delete | FF_Draw_Backspace) & Msg -> Flags)
        {
            switch (LOD -> Justify)
            {
                case FV_String_Left:    repair = REPAIR_FROMCUR; break;
                case FV_String_Center:  repair = REPAIR_ALL; break;
                case FV_String_Right:   repair = (fit.Clip) ? REPAIR_ALL : ((FF_Draw_Delete & Msg -> Flags) ? REPAIR_TOCUR : REPAIR_WITHCUR); break;
            }
        }

        if (repair)
        {
            if (repair == REPAIR_OLDC || repair == REPAIR_FROM_OLDC)
            {
                x1 = tx + TextLength(rp,LOD -> String + LOD -> Pos, old - LOD -> Pos);
            }

            switch (repair)
            {
                case REPAIR_OLDC:    x2 = x1 + TextLength(rp,LOD -> String + old, 1) - 1; break;
                case REPAIR_WITHCUR: x2 = cx; break;
                case REPAIR_FROMCUR: x1 = cx; break;
                case REPAIR_TOCUR:   x2 = cx-1; break;
            }

            F_Erase(Obj,x1,y1,x2,y2,0);
        }
    }

    if (FF_Area_Selected & _flags)
    {
        if (LOD -> Cursor)
        {
            struct FeelinRect rect;

            rect.x1 = cx; rect.x2 = cx + cw - 1;
            rect.y1 = y1; rect.y2 = y2;

            F_Do(LOD -> Cursor,FM_ImageDisplay_Draw,_render,&rect,0);
        }
        else
        {
            _FPen(FV_Pen_Highlight);
            _Boxf(cx,y1,cx+cw-1,y2);
        }
    }

    y1 += _font -> tf_Baseline;

    _APen((FF_Area_Selected & _flags) ? LOD -> APen -> Pen : LOD -> IPen -> Pen);
    _Move(tx,y1);
    _DrMd(JAM1);
    _Text(str,fit.Displayable);

    if (FF_Area_Selected & _flags && cur != len)
    {
        _APen(LOD -> BPen -> Pen);
        _Move(cx,y1);
        _Text(LOD -> String+cur,1);
    }

    _DrMd(JAM2);

    LOD -> Old = cur;
    LOD -> Pos = pos;
}
//+
