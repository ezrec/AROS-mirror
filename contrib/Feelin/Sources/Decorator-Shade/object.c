#include "Private.h"

/*** Methods ***************************************************************/

///Shade_New
F_METHOD(uint32,Shade_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    LOD -> flags = FF_SHADE_ZOOM_IS_ICONIFY;

    return F_SuperDo(Class,Obj,Method,

        FA_ChainToCycle, FALSE,
        
        FA_Font,          "$decorator-font",
        FA_ColorScheme,   "$decorator-scheme-active",

    TAG_MORE, Msg);
}
//+

