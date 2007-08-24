#include "Private.h"

///Histogram_New
F_METHOD(uint32,Histogram_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    uint32 resolution = 15;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Histogram_Resolution: resolution = item.ti_Data; break;
    }

    if (resolution < 12)
    {
        LOD -> bitspergun = 4;
    }
    else if (resolution > 24)
    {
        LOD -> bitspergun = 8;
    }
    else
    {
        switch (resolution)
        {
            case 13: case 14: case 15: LOD -> bitspergun = 5; break;
            case 16: case 17: case 18: LOD -> bitspergun = 6; break;
            case 19: case 20: case 21: LOD -> bitspergun = 7; break;
            case 22: case 23: case 24: LOD -> bitspergun = 8; break;
        }
    }

    LOD -> pool = F_CreatePool(1024, FA_Pool_Items,1, TAG_DONE);
    
    if (LOD -> pool != NULL)
    {
        uint32 mask = (0x000000FF << (8 - LOD -> bitspergun)) & 0x000000FF;

        LOD -> rgbmask = (mask << 16) | (mask << 8) | mask;

        return F_SUPERDO();
    }
    return NULL;
}

//+
///Histogram_Dispose
F_METHOD(void,Histogram_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    F_DeletePool(LOD -> pool); LOD -> pool = NULL;
    
    F_SUPERDO();
}
//+
///Histogram_Get
F_METHOD(void,Histogram_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Histogram_Resolution: F_STORE(LOD -> bitspergun * 3); break;
        case FA_Histogram_Pool: F_STORE(LOD -> pool); break;
        case FA_Histogram_NumColors: F_STORE(LOD -> numcolors); break;
        case FA_Histogram_NumPixels: F_STORE(LOD -> numpixels); break;
    }
    
    F_SUPERDO();
}
//+
