#include "Private.h"

#include <feelin/preference.h>

STATIC FPreferenceScript Script[] =
{
    { "$button-back",   FV_TYPE_STRING, "Spec",     "fill;halfshadow",0         },
    { "$button-font",   FV_TYPE_STRING, "Contents", NULL,0                      },
    { "$button-frame",  FV_TYPE_STRING, "Spec",     "<frame id='#23' padding-width='6' />\n<frame id='#24' padding-left='7' padding-right='6' padding-top='1' />",0 },
    { "$button-scheme", FV_TYPE_STRING, "Spec",     NULL,0                      },
    
    { "$text-back",     FV_TYPE_STRING, "Spec",     "halfshadow",0              },
    { "$text-font",     FV_TYPE_STRING, "Contents", NULL,0                      },
    { "$text-frame",    FV_TYPE_STRING, "Spec",     "<frame id='#28' padding-width='2' padding-height='1' />\n<frame id='#27' padding-width='2' padding-height='1' />",0 },
    { "$text-scheme",   FV_TYPE_STRING, "Spec",     NULL,0                      },
    
    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Area",

        "Script", Script,
        "XMLSource", "feelin/preference/area.xml",
          
    TAG_MORE,Msg);
}

