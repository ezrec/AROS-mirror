#include "Private.h"

STATIC FPreferenceScript Script[] =
{
    { "$decorator-shade-border",            FV_TYPE_INTEGER, "Value",    (STRPTR) 5,0 },
    { "$decorator-shade-gadgets-width",     FV_TYPE_INTEGER, "Value",    (STRPTR) 16,0 },
    { "$decorator-shade-gadgets-height",    FV_TYPE_INTEGER, "Value",    (STRPTR) 14,0 },

    F_ARRAY_END
};

///Prefs_New
F_METHOD(uint32, Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Frame, NULL,
        FA_Group_PageTitle, "Shade",

        "Script",         Script,
        "XMLSource",      "feelin/preference/decorator-shade.xml",

    TAG_MORE,Msg);
}
//+
