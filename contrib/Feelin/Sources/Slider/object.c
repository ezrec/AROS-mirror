#include "Private.h"

/*** Methods ***************************************************************/

///Slider_New
F_METHOD(FObject,Slider_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    STRPTR knob_font = "$slider-knob-font";

    BOOL horiz=FALSE;
  
    F_AREA_SAVE_PUBLIC;
    
    LOD -> Flags = FF_Slider_Buffer;
    
    LOD -> p_knob_preparse = "$slider-knob-preparse";

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Text_PreParse:  LOD -> p_knob_preparse = (STRPTR)(item.ti_Data); break;
        case FA_Slider_Knob:    LOD -> Knob = (FObject)(item.ti_Data); break;
        case FA_Font:           knob_font = (STRPTR) item.ti_Data; break;
        case FA_Horizontal:     horiz = item.ti_Data; break;
    }

    if (!LOD -> Knob)
    {
       LOD -> Knob = TextObject,
                     
          FA_Font,          knob_font,
          FA_Back,          (horiz) ? "$slider-knob-back" : "$slider-knob-back-vertical",
          FA_Frame,         "$slider-knob-frame",
          FA_ColorScheme,   "$slider-knob-scheme",
          FA_ChainToCycle,   FALSE,
       
       End;
    }

    _knob_area_public = (FAreaPublic *) F_Get(LOD -> Knob,FA_Area_PublicData);

    if (_knob_area_public)
    {
        F_Do(LOD -> Knob,FM_Connect,Obj);

        if (F_SuperDo(Class,Obj,Method,
           
            FA_Back,         (horiz) ? "$slider-back" : "$slider-back-vertical",
            FA_Frame,        "$slider-frame",
            FA_ColorScheme,  "$slider-scheme",
            FA_NoFill,        TRUE,
        
        TAG_MORE,Msg))
        {
            F_Do(LOD -> Knob,FM_Set,
   //                          FA_Bufferize,      TRUE,
                FA_Text_Static, TRUE,
                FA_Text, F_Get(Obj,F_IDR(FA_Numeric_Buffer)),
                TAG_DONE);

            return Obj;
        }
    }

    return NULL;
}
//+
///Slider_Dispose
F_METHOD(void,Slider_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> OwnBitMap)
    {
        FreeBitMap(LOD -> OwnBitMap); LOD -> OwnBitMap = NULL;
    }

    if (LOD -> Knob)
    {
        F_Do(LOD -> Knob,FM_Disconnect);
        F_DisposeObj(LOD -> Knob); LOD -> Knob = NULL; _knob_area_public = NULL;
    }

    F_SUPERDO();
}
//+

///Preferences
STATIC FPreferenceScript Script[] =
{
    { "$slider-frame",              FV_TYPE_STRING, "Spec",     "<frame id='18' />",0 },
    { "$slider-back",               FV_TYPE_STRING, "Spec",     "halfshadow",0 },
    { "$slider-back-vertical",      FV_TYPE_STRING, "Spec",     "halfshadow",0 },
    { "$slider-scheme",             FV_TYPE_STRING, "Spec",     NULL,0 },
    { "$slider-knob-font",          FV_TYPE_STRING, "Contents", NULL,0 },
    { "$slider-knob-preparse",      FV_TYPE_STRING, "Contents", NULL,0 },
    { "$slider-knob-frame",         FV_TYPE_STRING, "Spec",     "<frame id='23' padding-width='4' />\n<frame id='24' />",0 },
    { "$slider-knob-back",          FV_TYPE_STRING, "Spec",     "fill;halfshadow",0 },
    { "$slider-knob-back-vertical", FV_TYPE_STRING, "Spec",     "fill;halfshadow",0 },
    { "$slider-knob-scheme",        FV_TYPE_STRING, "Spec",     NULL,0 },

    F_ARRAY_END
};

F_METHOD(uint32,Prefs_New)
{
    return F_SuperDo(Class,Obj,Method,

        FA_Group_PageTitle, "Slider",

        "Script", Script,
        "XMLSource", "feelin/preference/slider.xml",

    TAG_MORE,Msg);
}
//+
