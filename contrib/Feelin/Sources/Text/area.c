#include "Private.h"

/*** Methods ***************************************************************/

///Text_Setup
F_METHOD(int32,Text_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        STRPTR prep;

        LOD -> Prep[0] = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> Prep[0],NULL);
        LOD -> Prep[1] = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> Prep[1],NULL);

        prep = LOD -> Prep[(FF_Area_Selected & _flags) ? 1 : 0];
        if (!prep) prep = LOD -> Prep[0];

        LOD -> TextDisplay = TextDisplayObject,

            FA_TextDisplay_Font,       _font,
            FA_TextDisplay_PreParse,   prep,
            FA_TextDisplay_Contents,   LOD -> Text,
            FA_TextDisplay_Shortcut,   (0 != (FF_Text_Shortcut & LOD -> Flags)),

            End;

        if (F_Do(LOD -> TextDisplay,FM_TextDisplay_Setup,_render))
        {
            F_Set(Obj,FA_ControlChar,F_Get(LOD -> TextDisplay,FA_TextDisplay_Shortcut));

            return TRUE;
        }
    }
    return FALSE;
}
//+
///Text_Cleanup
F_METHOD(uint32,Text_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        F_Do(LOD -> TextDisplay,FM_TextDisplay_Cleanup,_render);
        F_DisposeObj(LOD -> TextDisplay); LOD -> TextDisplay = NULL;
    }

    return F_SUPERDO();
}
//+
///Text_AskMinMax
F_METHOD(uint32,Text_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Text)
    {
        uint32 tw,th;
        
        F_Do(LOD -> TextDisplay,FM_Get,

            FA_TextDisplay_Width,   &tw,
            FA_TextDisplay_Height,  &th,

            TAG_DONE);

        if (FF_Area_SetMinW & _flags) _minw += tw;
        if (FF_Area_SetMinH & _flags) _minh += th;
    }
    else
    {
        _minh += _font -> tf_YSize;
    }

    return F_SUPERDO();
}
//+
///Text_Draw
F_METHODM(void,Text_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FRect rect;
    
    uint32 tw,th;

    rect.x1 = _ix; rect.x2 = rect.x1 + _iw - 1;
    rect.y1 = _iy; rect.y2 = rect.y1 + _ih - 1;

    F_Do(LOD -> TextDisplay,FM_Get,

        FA_TextDisplay_Width,   &tw,
        FA_TextDisplay_Height,  &th,

        TAG_DONE);

    if (((FF_Area_SetMinH & _flags) && (th > rect.y2 - rect.y1 + 1)) ||
        ((FF_Area_SetMinW & _flags) && (tw > rect.x2 - rect.x1 + 1)))
    {
//        F_Log(0,"(please report) need rethink: th (%ld) h (%ld) - tw (%ld) w (%ld)",th,rect.y2 - rect.y1 + 1,tw,rect.x2 - rect.x1 + 1);
 
        F_Do(_win,FM_Window_RequestRethink,Obj);
        
        return;
    }

    F_SUPERDO();

    if (FF_Draw_Update & Msg -> Flags)
    {
        F_Do(Obj,FM_Erase,&rect,0);
    }
 
    if (FF_Text_VCenter & LOD -> Flags)
    {
        uint16 h = rect.y2 - rect.y1 + 1;

        if (th < h)
        {
            rect.y1 = (h - th) / 2 + rect.y1 - 1;
        }
    }

    if (FF_Text_HCenter & LOD -> Flags)
    {
        uint16 w = rect.x2 - rect.x1 + 1;

        if (tw < w)
        {
            rect.x1 = (w - tw) / 2 + rect.x1 - 1;
        }
    }

    F_Do(LOD -> TextDisplay, FM_TextDisplay_Draw, &rect);
}
//+
