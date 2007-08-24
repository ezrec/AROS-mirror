#include "Private.h"

STATIC FPreferenceScript Script[] =
{
//    { "$checkbox-back",    FV_TYPE_STRING, "Spec", "fill",0 },
//    { "$checkbox-frame",   FV_TYPE_STRING, "Spec", "<frame id='35' padding='1' />\n<frame id='35' padding='1' />",0 },
    { "$checkbox-back",    FV_TYPE_STRING, "Spec", NULL,0 },
    { "$checkbox-frame",   FV_TYPE_STRING, "Spec", NULL,0 },
    { "$checkbox-scheme",  FV_TYPE_STRING, "Spec", NULL,0 },
    { "$checkbox-image",   FV_TYPE_STRING, "Spec", "<image type='brush' src='defaults/checkmark.fb0' />",0 },
    { "$radio-back",       FV_TYPE_STRING, "Spec", NULL,0 },
    { "$radio-scheme",     FV_TYPE_STRING, "Spec", NULL,0 },
    { "$radio-image",      FV_TYPE_STRING, "Spec", "<image type='brush' src='defaults/radio.fb0' />",0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Images",

        "Script", Script,
        "XMLSource", "feelin/preference/images.xml",

    TAG_MORE,Msg);
}

