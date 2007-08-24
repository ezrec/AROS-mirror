#include "Private.h"

/*** Private ***************************************************************/

///String_PrevWord
uint32 String_PrevWord(struct LocalObjectData *LOD)
{
    uint32 pos=0;
    STRPTR str;

    if ((str = LOD -> String) && LOD -> Cur > 0)
    {
        pos = LOD -> Cur - 1;

        while (str[pos] == ' ' && pos) pos--; 
        while (str[pos] != ' ' && pos) pos--;

        if (pos) pos++;
    }
    return pos;
}
//+
///String_NextWord
uint32 String_NextWord(struct LocalObjectData *LOD)
{
    uint32 pos=0;
    STRPTR str;

    if ((str = LOD -> String) && (pos = LOD -> Cur) != LOD -> Len)
    {
        while (str[pos] != ' ' && pos != LOD -> Len) pos++;
        while (str[pos] == ' ' && pos != LOD -> Len) pos++;
    }
    return pos;
}
//+
///String_Insert
int32 String_Insert(FClass *Class,FObject Obj,UBYTE Char)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    STRPTR chars;
    int32 ok=TRUE;
    
    if (LOD -> Max && (LOD -> Len + 1 == LOD -> Max))
    {
        return FALSE;
    }
 
    if (Char)
    {
        if ((chars = LOD -> Accept) != NULL)
        {
            ok = FALSE;

            while (*chars)
                if (*chars++ == Char)
                    { ok = TRUE; break; }
        }
        else if ((chars = LOD -> Reject) != NULL)
        {
            while (*chars)
                if (*chars++ == Char)
                    { ok = FALSE; break; }
        }

        if (ok)
        {
            STRPTR dispose=NULL;
            STRPTR src = LOD -> String;
            STRPTR dst;
            uint32  len = LOD -> Len - LOD -> Cur;
 
            if (LOD -> Max)
            {
                if (!LOD -> String)
                {
                    LOD -> String = F_New(LOD -> Max + 1);
                }
            }
            else
            {
                if (!LOD -> String || (LOD -> Len + 2 == LOD -> Allocated))
                {
                    if (!LOD -> Allocated)
                    {
                        LOD -> Allocated = 32;
                    }
                    else
                    {
                        LOD -> Allocated *= 2;
                    }
                
                    dispose = LOD -> String;
                    LOD -> String = F_New(LOD -> Allocated);
                    
//               F_Log(0,"new 0x%08lx (%ld)",LOD -> String,LOD -> Allocated);
                }
            }
        
            if (LOD -> String)
            {
                dst = LOD -> String + LOD -> Cur + 1;

//            F_Log(0,"src 0x%08lx (%ld)(%ld)",src,LOD -> Cur,len);
                
                if (src && src != LOD -> String)
                {
                    CopyMem(src,LOD -> String,LOD -> Cur);
                }
            
                src += LOD -> Cur;

                while (len--) dst[len] = src[len];

                LOD -> String[LOD -> Cur++] = Char;
                LOD -> String[++LOD -> Len] = 0;
            }
             
            F_Dispose(dispose);

            return TRUE;
        }
    }

    return FALSE;
}
//+
///String_FoundMousePos
uint32 String_FoundMousePos(FClass *Class,FObject Obj,int32 Msx)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct RastPort        *rp  = (_render) ? _rp : NULL;
    int32             c,x;
    uint32            cw;

    if (rp)
    {
        SetFont(rp,_font);

        x = LOD -> TextX;
        if (Msx < x) return 0;
        c = 0;

        for (;;)
        {
            cw = TextLength(rp,LOD -> String + LOD -> Pos + c,1);
            if (Msx >= x && Msx <= x + cw - 1) return (uint32)(LOD -> Pos + c);
            x = x + cw;
            if (LOD -> Pos + c == LOD -> Len)  return LOD -> Len;
            c++;
        }
    }
    return 0;
}
//+
///String_ClipLoad
uint32 String_ClipLoad(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Library *IFFParseBase = OpenLibrary("iffparse.library",36);
    uint32 added=0;

    if (IFFParseBase)
    {
        struct IFFHandle *iff = AllocIFF();
        
        if (iff)
        {
            if ((iff -> iff_Stream = (uint32) OpenClipboard(PRIMARY_CLIP)) != NULL)
            {
                InitIFFasClip(iff);

                if (!OpenIFF(iff,IFFF_READ))
                {
                    if (!StopChunk(iff,MAKE_ID('F','T','X','T'),MAKE_ID('C','H','R','S')))
                    {
                        if (!ParseIFF(iff,IFFPARSE_SCAN))
                        {
                            struct ContextNode *cn = CurrentChunk(iff);
                            
                            if (cn)
                            {
                                UBYTE c;
                                uint32 oldcur = LOD -> Cur;
                                uint32 oldpos = LOD -> Pos;

                                for (;;)
                                {
                                    if (!ReadChunkBytes(iff,&c,1))    break;
                                    if (String_Insert(Class,Obj,c))   added++;
                                    if (LOD -> Len + 1 == LOD -> Max) break;
                                } 

                                LOD -> Pos = oldpos;
                                LOD -> Cur = oldcur;
                            }
                        }
                    }
                    CloseIFF(iff);
                }
                CloseClipboard((APTR)(iff -> iff_Stream));
            }
            FreeIFF(iff);
        }
        CloseLibrary(IFFParseBase);
    }
    return added;
}
//+
///String_ClipSave
void String_ClipSave(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct Library *IFFParseBase = OpenLibrary("iffparse.library",36);

    if (IFFParseBase)
    {
        struct IFFHandle *iff = AllocIFF();
        
        if (iff)
        {
            if ((iff -> iff_Stream = (uint32) OpenClipboard(PRIMARY_CLIP)) != NULL)
            {
                InitIFFasClip(iff);

                if (!OpenIFF(iff,IFFF_WRITE))
                {
                    if (!PushChunk(iff,MAKE_ID('F','T','X','T'),MAKE_ID('F','O','R','M'),IFFSIZE_UNKNOWN))
                    {
                        if (!PushChunk(iff,NULL,MAKE_ID('C','H','R','S'),IFFSIZE_UNKNOWN))
                        {
                            WriteChunkBytes(iff,LOD -> String,LOD -> Len);
                            PopChunk(iff);
                        }
                        PopChunk(iff);
                    }
                    CloseIFF(iff);
                }
                CloseClipboard((APTR)(iff -> iff_Stream));
            }
            FreeIFF(iff);
        }
        CloseLibrary(IFFParseBase);
    }
}
//+

/*** Methods ***************************************************************/

///String_HandleEvent
F_METHODM(uint32,String_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;
    uint32 cur = LOD -> Cur;
    uint32 len = LOD -> Len;
    uint32 sel = LOD -> Sel;
    bits32 dflags = FF_Draw_Move;

    if (fev -> Key)
    {
///KEYS
        LOD -> BlinkElapsed = 0; LOD -> Flags &= ~FF_String_Blink;

        switch (fev -> Key)
        {
            case FV_KEY_LEFT:
            {
                if (IEQUALIFIER_CONTROL & fev -> Qualifier) { sel -= 1; }
                else                                        { sel  = 1; if (cur) --cur; }
            }
            break;
            
            case FV_KEY_RIGHT:
            {
                if (IEQUALIFIER_CONTROL & fev -> Qualifier) { sel += 1; }
                else                                        { sel  = 1; if (cur < len) cur++; }
            }
            break;
            
            case FV_KEY_STEPLEFT:   cur = String_PrevWord(LOD);   break;
            case FV_KEY_STEPRIGHT:  cur = String_NextWord(LOD);   break;
            case FV_KEY_FIRST:      cur = 0;                      break;
            case FV_KEY_LAST:       cur = len;                    break;

            case FV_KEY_LINEBACK:
            {
                if (len && cur)
                {
                    if (cur == len)
                    {
                        len = 0; cur = 0; *LOD -> String = 0;
                    }
                    else
                    {
                        CopyMem(LOD -> String + cur,LOD -> String,len - cur + 1);
                        len -= cur; cur = 0;
                    }
                    dflags = FF_Draw_Backspace;
                }
            }
            break;

            case FV_KEY_LINEDEL:
            {
                if (len && cur < len)
                {
                    len = cur; LOD -> String[cur] = 0;
                    dflags = FF_Draw_Delete;
                }
            }
            break;

            case FV_KEY_WORDBACK:
            {
                if (len && cur)
                {
                    uint32 pos = String_PrevWord(LOD);
                    CopyMem(LOD -> String + cur, LOD -> String + pos,len - (cur - pos));
                    len = len - (cur - pos);
                    cur = pos;

                    dflags = FF_Draw_Backspace;
                }
            }
            break;

            case FV_KEY_WORDDEL:
            {
                if (len && cur < len)
                {
                    uint32 pos = String_NextWord(LOD);
                    CopyMem(LOD -> String + pos, LOD -> String + cur, len - (pos - cur));
                    len = len - (pos - cur);

                    dflags = FF_Draw_Delete;
                }
            }
            break;

            case FV_KEY_CHARDEL:
            {
                if (len && cur < len)
                {
                    CopyMem(LOD -> String + cur + 1,LOD -> String + cur, len - cur + 1);
                    len--;
                    dflags = FF_Draw_Delete;
                }
            }
            break;

            case FV_KEY_CHARBACK:
            {
                if (len && cur )
                {
                    CopyMem(LOD -> String + cur,LOD -> String + cur - 1,len - cur + 1);
                    len--;
                    cur--;
                    dflags = FF_Draw_Backspace;
                }
            }
            break;

            case FV_KEY_PRESS:
            {
                F_Dispose(LOD -> Undo);
                LOD -> Undo = F_StrNew(NULL,"%s",LOD -> String);
 
                F_Set(Obj,F_IDA(FA_String_Changed),(uint32) LOD -> String);

                if (FF_String_AdvanceOnCR & LOD -> Flags)
                {
                    F_Set(_win,FA_Window_ActiveObject,FV_Window_ActiveObject_Next);
                }
        
                return FF_HandleEvent_Eat;
            }
            break;

            default:
            {
                if (LOD -> AttachedList)
                {
                    return F_OBJDO(LOD -> AttachedList);
                }
                return 0;
            }
        }

        if (fev -> Key != FV_KEY_LEFT && fev -> Key != FV_KEY_RIGHT)
        {
            sel = 1;
        }

        if (cur != LOD -> Cur || len != LOD -> Len || sel != LOD -> Sel)
        {
            LOD -> Cur = cur;
            LOD -> Sel = sel;

            if (LOD -> Len != len)
            {
                LOD -> Len = len;
                
                if (!LOD -> Len && !LOD -> Max)
                {
                    F_Dispose(LOD -> String);
                    
                    LOD -> String = NULL;
                    LOD -> Allocated = 0;
                }
 
                F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_String_Contents),LOD -> String,TAG_DONE);
            }
            
            F_Draw(Obj,FF_Draw_Update | dflags);
        }

        return FF_HandleEvent_Eat;
    }
//+
    else
    {
        switch (fev -> Class)
        {
///FF_EVENT_KEY
            case FF_EVENT_KEY:
            {
                LOD -> BlinkElapsed = 0;
                LOD -> Flags       &= ~FF_String_Blink;

                if (fev -> DecodedChar && !(FF_EVENT_KEY_UP & fev -> Flags))
                {
                    if ((IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND) & fev -> Qualifier)
                    {
                        switch (fev -> DecodedChar)
                        {
                            case 'l':
                            {
                                while (len--) LOD -> String[len] = ToLower(LOD -> String[len]);
                            }
                            break;
                            
                            case 'L':
                            {
                                while (len--) LOD -> String[len] = ToUpper(LOD -> String[len]);
                            }
                            break;
                            
                            case 'u':
                            {
                                F_Set(Obj,F_IDA(FA_String_Contents),(uint32)(LOD -> Undo));
                            }
                            break;
                            
                            case 'c':
                            {
                                String_ClipSave(Class,Obj);
                            }
                            return FF_HandleEvent_Eat;
                            
                            case 'v':
                            {
                                if (String_ClipLoad(Class,Obj) == 0) return FF_HandleEvent_Eat;
                            }
                            break;
                            
                            case 'x':
                            {
                                String_ClipSave(Class,Obj);
                                F_Set(Obj,F_IDA(FA_String_Contents),NULL);
                            }
                            return FF_HandleEvent_Eat;
                            
                            default: return 0;
                        }

                        F_Draw(Obj,FF_Draw_Object);
                        F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_String_Contents),LOD -> String,TAG_DONE);

                        return FF_HandleEvent_Eat;
                    }
                    else
                    {
                        if (String_Insert(Class,Obj,fev -> DecodedChar))
                        {
                            F_Draw(Obj,FF_Draw_Update | FF_Draw_Insert);
                            
                            if (LOD -> AttachedList)
                            {
                                uint32 pos = F_Do(LOD -> AttachedList,(uint32) "FM_List_FindString", LOD -> String, NULL);
                                
                                F_Set(LOD -> AttachedList,(uint32) "FA_List_Active",pos);
                            }
                            
                            F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_String_Contents),LOD -> String,TAG_DONE);
                            
                            return FF_HandleEvent_Eat;
                        }
                        else
                        {
                            if (_render) DisplayBeep((APTR) F_Get(_display,(uint32) "FA_Display_Screen"));
                        }
                    }
                }
/*
                    uint32 active=0;
                    BOOL  ok=TRUE;

                    switch (fev -> Key)
                    {
                        case FV_KEY_UP:         active = FV_List_Active_Up;         break;
                        case FV_KEY_DOWN:       active = FV_List_Active_Down;       break;
                        case FV_KEY_STEPUP:     active = FV_List_Active_PageUp;     break;
                        case FV_KEY_STEPDOWN:   active = FV_List_Active_PageDown;   break;
                        case FV_KEY_TOP:        active = FV_List_Active_Top;        break;
                        case FV_KEY_DOWN:       active = FV_List_Active_Down;       break;
                    }
*/
            }
            break;
//+
///FF_EVENT_BUTTON
            case FF_EVENT_BUTTON:
            {
                if ((fev -> Code == FV_EVENT_BUTTON_SELECT) && (FF_EVENT_BUTTON_DOWN & fev -> Flags))
                {
                    if (fev -> MouseX >= _x && fev -> MouseX <= _x2 && fev -> MouseY >= _y && fev -> MouseY <= _y2)
                    {
                        cur = String_FoundMousePos(Class,Obj,fev -> MouseX);
                        LOD -> Sel = 1;

                        if (FF_Area_Selected & _flags)
                        {
                            if (LOD -> Cur != cur)
                            {
                                LOD -> Cur = cur;
                                F_Draw(Obj,FF_Draw_Update | FF_Draw_Move);
                            }
                        }
                        else
                        {
                            LOD -> Cur = cur;

                            F_Set(_win,FA_Window_ActiveObject,(uint32) Obj);
                        }

                        return FF_HandleEvent_Eat;
                    }
                    else if (FF_Area_Active & _flags)
                    {
                        F_Set(_win,FA_Window_ActiveObject,NULL);
                    }
                }
            }
            break;
//+
///FF_EVENT_TICK
            case FF_EVENT_TICK:
            {
                if (LOD -> BlinkSpeed)
                {
                    if (++LOD -> BlinkElapsed == LOD -> BlinkSpeed)
                    {
                        LOD -> BlinkElapsed = 0;

                        if (FF_String_Blink & LOD -> Flags) LOD -> Flags &= ~FF_String_Blink;
                        else                                LOD -> Flags |=  FF_String_Blink;

                        F_Draw(Obj,FF_Draw_Update | FF_Draw_Blink);
                    }
                }
            }
            break;
//+
        }
    }
    return 0;
}
//+
