#include "Private.h"

STATIC FPreferenceScript Script[] =
{
    { "$decorator-flatty-spacing-horizontal", FV_TYPE_INTEGER, "Value", (STRPTR) DEF_SPACING_HORIZONTAL, 0 },
    { "$decorator-flatty-spacing-vertical", FV_TYPE_INTEGER, "Value", (STRPTR) DEF_SPACING_VERTICAL, 0 },
    { "$decorator-flatty-dragbar-back", FV_TYPE_STRING, "Spec", DEF_DRAGBAR_BACK , 0 },
    { "$decorator-flatty-sizebar-height", FV_TYPE_INTEGER, "Value", (STRPTR) DEF_SIZEBAR_HEIGHT, 0 },
    { "$decorator-flatty-sizebar-back", FV_TYPE_STRING, "Spec", DEF_SIZEBAR_BACK , 0 },
    { "$decorator-flatty-image-close", FV_TYPE_STRING, "Spec", DEF_IMAGE_CLOSE , 0 },
    { "$decorator-flatty-image-zoom", FV_TYPE_STRING, "Spec", DEF_IMAGE_ZOOM , 0 },
    { "$decorator-flatty-image-depth", FV_TYPE_STRING, "Spec", DEF_IMAGE_DEPTH , 0 },
    #if 0
    { "$decorator-flatty-gadgets-width", FV_TYPE_INTEGER, "Value",    (STRPTR) 16,0 },
    { "$decorator-flatty-gadgets-height", FV_TYPE_INTEGER, "Value",    (STRPTR) 14,0 },
    #endif

    F_ARRAY_END
};

///Prefs_New
F_METHOD(uint32, Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Frame, NULL,
        FA_Group_PageTitle, "Flatty",

        "Script", Script,
        "XMLSource", "feelin/preference/decorator-flatty.xml",

    TAG_MORE,Msg);
}
//+
