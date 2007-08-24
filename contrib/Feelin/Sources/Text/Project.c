/*

$VER: 04.00 (2005/08/10)

*/

#include "Project.h"

///METHODS
F_METHOD_PROTO(void,Text_New);
F_METHOD_PROTO(void,Text_Dispose);
F_METHOD_PROTO(void,Text_Set);
F_METHOD_PROTO(void,Text_Get);
F_METHOD_PROTO(void,Text_Setup);
F_METHOD_PROTO(void,Text_Cleanup);
F_METHOD_PROTO(void,Text_AskMinMax);
F_METHOD_PROTO(void,Text_Draw);
//+

F_QUERY()
{
    switch (Which)
    {
///Class
        case FV_Query_ClassTags:
        {
            STATIC F_ATTRIBUTES_ARRAY =
            {
                F_ATTRIBUTES_ADD_BOTH("Text",          FV_TYPE_STRING,  FA_Text),
                F_ATTRIBUTES_ADD_BOTH("PreParse",      FV_TYPE_STRING,  FA_Text_PreParse),
                F_ATTRIBUTES_ADD_BOTH("AltPreParse",   FV_TYPE_STRING,  FA_Text_AltPreParse),
                F_ATTRIBUTES_ADD_BOTH("Shortcut",      FV_TYPE_BOOLEAN, FA_Text_Shortcut),
                F_ATTRIBUTES_ADD_BOTH("HCenter",       FV_TYPE_BOOLEAN, FA_Text_HCenter),
                F_ATTRIBUTES_ADD_BOTH("VCenter",       FV_TYPE_BOOLEAN, FA_Text_VCenter),
                F_ATTRIBUTES_ADD_BOTH("Static",        FV_TYPE_BOOLEAN, FA_Text_Static),
                
                F_ARRAY_END
            };

            STATIC F_METHODS_ARRAY =
            {
                F_METHODS_ADD_STATIC(Text_New,        FM_New),
                F_METHODS_ADD_STATIC(Text_Dispose,    FM_Dispose),
                F_METHODS_ADD_STATIC(Text_Get,        FM_Get),
                F_METHODS_ADD_STATIC(Text_Set,        FM_Set),
                F_METHODS_ADD_STATIC(Text_Setup,      FM_Setup),
                F_METHODS_ADD_STATIC(Text_Cleanup,    FM_Cleanup),
                F_METHODS_ADD_STATIC(Text_AskMinMax,  FM_AskMinMax),
                F_METHODS_ADD_STATIC(Text_Draw,       FM_Draw),
                
                F_ARRAY_END
            };

            STATIC F_TAGS_ARRAY =
            {
                F_TAGS_ADD_SUPER(Area),
                F_TAGS_ADD_LOD,
                F_TAGS_ADD_METHODS,
                F_TAGS_ADD_ATTRIBUTES,
                
                F_ARRAY_END
            };

            return F_TAGS;
        }
//+
    }
    return NULL;
}
