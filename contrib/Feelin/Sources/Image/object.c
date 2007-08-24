#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Image_New
F_METHOD(uint32,Image_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    LOD -> ImageDisplay = ImageDisplayObject,
       
       FA_ImageDisplay_Spec,   FI_Fill,
       FA_ImageDisplay_Origin, &LOD -> Origin,
       
    End;
    
    if (LOD -> ImageDisplay)
    {
       return F_SuperDo(Class,Obj,Method,

          FA_SetMin,  TRUE,

          TAG_MORE,   Msg);
    }
    return NULL;
}
//+
///Image_Dispose
F_METHOD(uint32,Image_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> ImageDisplay); LOD -> ImageDisplay = NULL;
    F_Dispose(LOD -> image_spec); LOD -> image_spec = NULL;

    return F_SUPERDO();
}
//+
///Image_Set
F_METHOD(void,Image_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Image_Spec:
        {
            F_Dispose(LOD -> image_spec); LOD -> image_spec = NULL;
            
            if (item.ti_Data)
            {
                LOD -> image_spec = F_StrNew(NULL,"%s",item.ti_Data);
            }

            if (_render)
            {
                uint32 spec;

                _render -> Palette = _palette;

                spec = F_Do(_app,FM_Application_Resolve,LOD -> image_spec,"dark;shine");

                F_Set(LOD -> ImageDisplay,FA_ImageDisplay_Spec,spec);

                F_Draw(Obj, FF_Draw_Update);
            }
        }
        break;
    }

    F_SUPERDO();
}
//+
///Image_Get
F_METHOD(void,Image_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg, item;

    BOOL up = FALSE;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Image_Spec:
        {
            F_STORE(LOD -> image_spec);
        }
        break;

        default: up = TRUE;
    }

    if (up) F_SUPERDO();
}
//+

///Image_Setup
F_METHODM(LONG,Image_Setup,FS_Setup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
        uint32 spec;

        _render -> Palette = (struct FeelinPalette *) F_Get(Obj,FA_ColorScheme);

        spec = F_Do(_app,FM_Application_Resolve,LOD -> image_spec,"dark;shine");

        F_Set(LOD -> ImageDisplay,FA_ImageDisplay_Spec,spec);

        F_DoA(LOD -> ImageDisplay,FM_ImageDisplay_Setup,Msg);

        return TRUE;
    }
    return FALSE;
}
//+
///Image_Cleanup
F_METHOD(uint32,Image_Cleanup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        F_Do(LOD -> ImageDisplay,FM_ImageDisplay_Cleanup,_render);
    }
    return F_SUPERDO();
}
//+
///Image_AskMinMax
F_METHOD(uint32,Image_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint32 imgw=0,imgh=0;

    F_Do(LOD -> ImageDisplay,FM_Get,

        FA_ImageDisplay_Width,  &imgw,
        FA_ImageDisplay_Height, &imgh,
        
        TAG_DONE);

    if (FF_Area_SetMinW & _flags) _minw += imgw;
    if (FF_Area_SetMinH & _flags) _minh += imgh;
    if (FF_Area_SetMaxW & _flags) _maxw  = imgw;
    if (FF_Area_SetMaxH & _flags) _maxh  = imgh;

    return F_SUPERDO();
}
//+
///Image_Draw
F_METHODM(void,Image_Draw,FS_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FRect rect;

    F_SUPERDO();

    rect.x1 = _ix ; rect.x2 = _ix2;
    rect.y1 = _iy ; rect.y2 = _iy2;
    
    LOD -> Origin.x = _ix; LOD -> Origin.w = _iw;
    LOD -> Origin.y = _iy; LOD -> Origin.h = _ih;

    if ((LOD -> image_spec == NULL) || (F_Get(LOD -> ImageDisplay,FA_ImageDisplay_Mask)))
    {
        F_Do(Obj,FM_Erase,&rect,0);
    }

    if (LOD -> image_spec)
    {
        F_Set(LOD -> ImageDisplay, FA_ImageDisplay_State, (FF_Area_Selected & _flags) ? FV_Image_Selected : FV_Image_Normal);
        F_Do(LOD -> ImageDisplay, FM_ImageDisplay_Draw, _render, &rect, 0);
    }
    else if (_iw > 4 && _ih > 4)
    {
        struct RastPort *rp = _rp;
        int16 x1 = _ix, x2 = x1 + _iw - 1;
        int16 y1 = _iy, y2 = y1 + _ih - 1;

        _FPen(FV_Pen_Shadow);
        _Move(x1,y1);   _Draw(x2-1,y2);
        _Move(x1+1,y2); _Draw(x2,y1);
        _FPen(FV_Pen_HalfShine);
        _Move(x1+1,y1); _Draw(x2,y2);
        _Move(x1,y2);   _Draw(x2-1,y1);
    }
}
//+
