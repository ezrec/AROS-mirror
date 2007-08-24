#include "Private.h"

///List_AskMinMax
F_METHOD(void,List_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _minh += (((LOD -> MaximumH) ? LOD -> MaximumH : _font -> tf_YSize) + LOD -> Spacing) * ((FF_Area_SetMinH & _flags) ? LOD -> LineCount : MIN(5,LOD -> LineCount)) - LOD -> Spacing + LOD -> TitleBarH;
    _minh += (((LOD -> MaximumH) ? LOD -> MaximumH : _font -> tf_YSize) + LOD -> Spacing) * MIN(5,LOD -> LineCount) - LOD -> Spacing + LOD -> TitleBarH;
 
    if ((FF_Area_SetMinW | FF_Area_SetMaxW) & _flags)
    {
        uint32 minw = 0;
        uint32 j;
        
        if (LOD -> Columns)
        {
            for (j = 0 ; j < LOD -> ColumnCount ; j++)
            {
                minw += LOD -> Columns[j].Width;
            }
        }

        if (FF_Area_SetMinW & _flags) _minw += minw;
//        if (FF_Area_SetMaxW & _flags) _maxw  = minw;
    }
    F_SUPERDO();
}
//+
///List_Layout
F_METHOD(void,List_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> LastFirst = NULL;

    list_update(Class,Obj,TRUE);

    if (LOD -> First && LOD -> Last && LOD -> Active)
    {
        if (LOD -> Active -> Position < LOD -> First -> Position ||
             LOD -> Active -> Position > LOD -> Last -> Position)
        {
            LOD -> First = LOD -> Active;

            list_update(Class,Obj,TRUE);
        }
    }
}
//+

///List_Setup
F_METHOD(uint32,List_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        STRPTR spec;
        FLine *line;

        LOD -> MaximumH  = 0;
        LOD -> Scheme    = (FPalette *) F_Get(Obj,FA_ColorScheme);
        LOD -> Steps     = F_Do(_app,FM_Application_ResolveInt,LOD -> p_Steps,1);
        LOD -> Spacing   = F_Do(_app,FM_Application_ResolveInt,LOD -> p_Spacing,1);
        
        spec = (STRPTR) F_Do(_app,FM_Application_Resolve,LOD -> p_CursorActive,NULL);
        
        if (spec)
        {
            LOD -> CursorActive = F_NewObj(FC_ImageDisplay,
                
                FA_ImageDisplay_Spec,    spec,
            
            TAG_DONE);
            
            if (LOD -> CursorActive)
            {
                if (!F_Do(LOD -> CursorActive,FM_ImageDisplay_Setup,_render))
                {
                    F_DisposeObj(LOD -> CursorActive); LOD -> CursorActive = NULL;
                }
            }
        }

        F_Set(LOD -> TDObj,FA_TextDisplay_Font,(uint32) _font);
        F_Do(LOD -> TDObj,FM_TextDisplay_Setup,_render);
        
        titlebar_setup(Class,Obj);

        for (line = (FLine *)(LOD -> LineList.Head) ; line ; line = line -> Next)
        {
            line_compute_dimensions(Class,Obj,line);
        }

        F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

        return TRUE;
    }
    return FALSE;
}
//+
///List_Cleanup
F_METHOD(void,List_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FLine *line;

    for (line = (FLine *)(LOD -> LineList.Head) ; line ; line = line -> Next)
    {
        line -> Flags &= ~FF_LINE_COMPUTED;
    }

    titlebar_cleanup(Class,Obj);

    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);

    F_Do(LOD -> TDObj,FM_TextDisplay_Cleanup);
    
    F_Do(LOD -> CursorActive,FM_ImageDisplay_Cleanup,_render);
    F_DisposeObj(LOD -> CursorActive);

    F_SUPERDO();
}
//+
///List_GoActive
F_METHOD(void,List_GoActive)
{
    F_Do(Obj,FM_ModifyHandler,FF_EVENT_KEY,0);
}
//+
///List_GoInactive
F_METHOD(void,List_GoInactive)
{
    F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_KEY);
}
//+
///List_HandleEvent
F_METHODM(uint32,List_HandleEvent,FS_HandleEvent)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FEvent *fev = Msg -> Event;

    if (fev -> Key)
    {
        uint32 active;

        switch (fev -> Key)
        {
            case FV_KEY_UP:         active = FV_List_Active_Up;         break;
            case FV_KEY_DOWN:       active = FV_List_Active_Down;       break;
            case FV_KEY_STEPUP:     active = FV_List_Active_PageUp;     break;
            case FV_KEY_STEPDOWN:   active = FV_List_Active_PageDown;   break;
            case FV_KEY_TOP:        active = FV_List_Active_Top;        break;
            case FV_KEY_BOTTOM:     active = FV_List_Active_Bottom;     break;
            default:                return 0;
        }

        LOD -> Flags |= FF_LIST_ACTIVATION_INTERNAL;
 
        F_Do(Obj,FM_Set,
            
            F_IDA(FA_List_Active),     active,
            F_IDA(FA_List_Activation), FV_List_Activation_Key,
        
        TAG_DONE);

        return FF_HandleEvent_Eat;
    }
    else
    {
        switch (fev -> Class)
        {
            case FF_EVENT_KEY:
            {
                if (fev -> DecodedChar && !(FF_EVENT_KEY_UP & fev -> Flags))
                {
                    char str[2];
                    int32 pos;
                    
                    str[0] = fev -> DecodedChar; str[1] = 0;
                    
                    pos = F_Do(Obj,F_IDM(FM_List_FindString),&str);
                    
                    if (pos != FV_List_FindString_NotFound)
                    {
                        F_Set(Obj,F_IDA(FA_List_Active),pos);
                    }
                }
            }
            break;
            
            case FF_EVENT_BUTTON:
            {
                if ((fev -> Code == FV_EVENT_BUTTON_SELECT) && (FF_EVENT_BUTTON_DOWN & fev -> Flags) && (LOD -> LineCount))
                {
                    if (fev -> MouseX >= _ix && fev -> MouseX <= _ix2 &&
                         fev -> MouseY >= _iy + LOD -> TitleBarH &&
                         fev -> MouseY <= _iy + LOD -> TitleBarH + LOD -> Visible * LOD -> MaximumH - 1)
                    {
                        uint32 pos = ((LOD -> First) ? LOD -> First -> Position : 0) + (fev -> MouseY - _iy - LOD -> TitleBarH) / LOD -> MaximumH;

                        LOD -> Flags |= FF_LIST_ACTIVATION_INTERNAL;
                        
                        if (LOD -> Active && (LOD -> Active -> Position == pos) && (FF_EVENT_BUTTON_DOUBLE & fev -> Flags))
                        {
                            F_Do(Obj,FM_Set,

                                F_IDA(FA_List_Active),     pos,
                                F_IDA(FA_List_Activation), FV_List_Activation_DoubleClick,

                            TAG_DONE);
                        }
                        else
                        {
                            F_Do(Obj,FM_Set,

                                F_IDA(FA_List_Active),     pos,
                                F_IDA(FA_List_Activation), FV_List_Activation_Click,

                            TAG_DONE);
                        }
 
                        return FF_HandleEvent_Eat;
                    }
                }
                else if (fev -> Code == FV_EVENT_BUTTON_WHEEL)
                {
                    if (fev -> MouseX >= _x && fev -> MouseX <= _x2 &&
                         fev -> MouseY >= _y && fev -> MouseY <= _y2)
                    {
                        uint32 pos = (LOD -> First) ? LOD -> First -> Position : 0;
     
                        if (FF_EVENT_BUTTON_DOWN & fev -> Flags)
                        {
                            F_Set(Obj,F_IDA(FA_List_First),pos + LOD -> Steps);
                        }
                        else
                        {
                            F_Set(Obj,F_IDA(FA_List_First),pos - LOD -> Steps);
                        }
                    }
                }
            }
            break;
        }
    }
    return 0;
}
//+

