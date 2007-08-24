#include "Private.h"

///BitMap_New
F_METHOD(uint32,BitMap_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_BitMap_Width:
        {
            LOD->source.Width = item.ti_Data;
        }
        break;
         
        case FA_BitMap_Height:
        {
            LOD->source.Height = item.ti_Data;
        }
        break;
        
        case FA_BitMap_Depth:
        {
            LOD->source.Depth = item.ti_Data;
        }
        break;

        case FA_BitMap_PixelSize:
        {
            LOD->source.PixelSize = item.ti_Data;
        }
        break;

        case FA_BitMap_PixelArray:
        {
            LOD->source.PixelArray = (APTR) item.ti_Data;
        }
        break;
        
        case FA_BitMap_PixelArrayMod:
        {
            LOD->source.PixelArrayMod = item.ti_Data;
        }
        break;
        
        case FA_BitMap_ColorType:
        {
            LOD->source.ColorType = item.ti_Data;
        }
        break;
        
        case FA_BitMap_ColorCount:
        {
            LOD->source.ColorCount = item.ti_Data;
        }
        break;
        
        case FA_BitMap_ColorArray:
        {
            LOD->source.ColorArray = (uint32 *) item.ti_Data;
        }
        break;
    }
/*
    F_Log(0,"(%ld x %ld) PixelSize (%ld) Array 0x%08lx Mod (%ld) - ColorType (0x%08lx) ColorArray (0x%08lx)",
        LOD->source.Width, LOD->source.Height, LOD->source.PixelSize, LOD->source.PixelArray,
        LOD->source.PixelArrayMod, LOD->source.ColorType, LOD->source.ColorArray);
*/
    if (LOD->source.Width && LOD->source.Height &&
        LOD->source.PixelArray && LOD->source.PixelArrayMod &&
        LOD->source.ColorType)
    {
        return F_SUPERDO();
    }
    return NULL;
}
//+
///BitMap_Dispose
F_METHOD(void,BitMap_Dispose)
{
    bitmap_render_free(Class,Obj);
 
    F_SUPERDO();
}
//+
///BitMap_Get
F_METHOD(void,BitMap_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_BitMap_PixelArray:      F_STORE(LOD -> source.PixelArray); break;
        case FA_BitMap_Width:           F_STORE(LOD -> source.Width); break;
        case FA_BitMap_Height:          F_STORE(LOD -> source.Height); break;

        case FA_BitMap_Rendered:        F_STORE(LOD -> rendered); break;
        case FA_BitMap_RenderedType:    F_STORE(0 != (FF_BITMAP_RENDERED_BITMAP & LOD -> flags)); break;
        case FA_BitMap_RenderedWidth:   F_STORE(LOD -> rendered_w); break;
        case FA_BitMap_RenderedHeight:  F_STORE(LOD -> rendered_h); break;
        case FA_BitMap_Pens:            F_STORE(LOD -> pens); break;
        case FA_BitMap_NumPens:         F_STORE(LOD -> numpens); break;
    }

    F_SUPERDO();
}
//+
