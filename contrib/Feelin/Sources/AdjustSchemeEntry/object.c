#include "Private.h"

STATIC STRPTR numeric_string_array[] =
{
    "shine", "halfshine", "fill", "halfshadow", "shadow", "halfdark", "dark", "text", "highlight", F_ARRAY_END
};

enum    {
    
        FV_ARRAY_SHINE,
        FV_ARRAY_HALFSHINE,
        FV_ARRAY_FILL,
        FV_ARRAY_HALFSHADOW,
        FV_ARRAY_SHADOW,
        FV_ARRAY_HALFDARK,
        FV_ARRAY_DARK,
        FV_ARRAY_TEXT,
        FV_ARRAY_HIGHLIGHT,
        
        ENTRIES_COUNT

        };

///Rectangle_
STATIC FObject Rectangle_(uint32 id)
{
    return  AreaObject,
            
            FA_ChainToCycle,  FALSE,
            FA_InputMode,     FV_InputMode_Immediate,
            FA_Back,          numeric_string_array[id],
            FA_MinWidth,      4,
            FA_MinHeight,     4,
            
            End;
}
//+

#define COMPATIBILITY

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD(FObject,Adjust_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    FObject entries[ENTRIES_COUNT];

    FObject entries_grp = HGroup,
        Child, HGroup, FA_Group_HSpacing,0, FA_Weight,70,
            Child, entries[0] = Rectangle_(FV_ARRAY_SHINE),
            Child, entries[1] = Rectangle_(FV_ARRAY_HALFSHINE),
            Child, entries[2] = Rectangle_(FV_ARRAY_FILL),
            Child, entries[3] = Rectangle_(FV_ARRAY_HALFSHADOW),
            Child, entries[4] = Rectangle_(FV_ARRAY_SHADOW),
            Child, entries[5] = Rectangle_(FV_ARRAY_HALFDARK),
            Child, entries[6] = Rectangle_(FV_ARRAY_DARK),
        End,
        
        Child, BarObject, FA_MinWidth,10, End,
        
        Child, HGroup, FA_Group_HSpacing,0, FA_Weight,20,
            Child, entries[7] = Rectangle_(FV_ARRAY_TEXT),
            Child, entries[8] = Rectangle_(FV_ARRAY_HIGHLIGHT),
        End,
    End;

    if (!entries_grp)
    {
        return NULL;
    }
 
    if (F_SuperDo(Class,Obj,Method,
        
        "PreviewTitle", "Feelin",
        "PreviewClass", "PreviewImage",
        "Separator",     FV_ImageDisplay_Separator,

        Child, VGroup,
            Child, entries_grp,

            Child, LOD -> slider = SliderObject,
                FA_Horizontal, TRUE,
                FA_SetMax, FV_SetHeight,
                "Min", FV_Pen_Text,
                "Max", FV_Pen_Highlight,
                "Value", 0,
                "StringArray", numeric_string_array,
            End,
        End,

        TAG_MORE,Msg))
    {
        uint32 i;
 
        F_Do(LOD -> slider, FM_Notify, "Value", FV_Notify_Always, Obj, F_IDM(FM_Adjust_ToString), 2, FV_Notify_Value, TRUE);

        for (i = 0 ; i < ENTRIES_COUNT ; i++)
        {
            F_Do(entries[i],FM_Notify, FA_Selected,TRUE, LOD -> slider, FM_Set,2, "Value",i);
        }

        return Obj;
    }

    return NULL;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
    if (Msg -> Spec)
    {
        switch (*Msg -> Spec)
        {
            case 'd':
            {
                if (F_StrCmp("dark",Msg -> Spec,4) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_DARK;
                }
            }
            break;

            case 'f':
            {
                if (F_StrCmp("fill",Msg -> Spec,4) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_FILL;
                }
            }
            break;

            case 'h':
            {
                int32 cmp;

                if ((cmp = F_StrCmp("halfdark",Msg -> Spec,8)) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_HALFDARK;
                }
                else if (cmp > 0) break;

                if ((cmp = F_StrCmp("halfshadow",Msg -> Spec,10)) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_HALFSHADOW;
                }
                else if (cmp > 0) break;

                if ((cmp = F_StrCmp("halfshine",Msg -> Spec,9)) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_HALFSHINE;
                }
                else if (cmp > 0) break;

                if ((cmp = F_StrCmp("highlight",Msg -> Spec,9)) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_HIGHLIGHT;
                }
                else if (cmp > 0) break;
            }
            break;

            case 's':
            {
#ifdef COMPATIBILITY
                if (Msg -> Spec[1] == ':')
                {
                    return 0xFFFF0000 | (uint32) atol(Msg -> Spec + 2);
                }
                else
                {
#endif
                    int32 cmp;

                    if ((cmp = F_StrCmp("shadow",Msg -> Spec,6)) == 0)
                    {
                        return 0xFFFF0000 | FV_ARRAY_SHADOW;
                    }
                    else if (cmp > 0) break;

                    if ((cmp = F_StrCmp("shine",Msg -> Spec,5)) == 0)
                    {
                        return 0xFFFF0000 | FV_ARRAY_SHINE;
                    }
                    else if (cmp > 0) break;
#ifdef COMPATIBILITY
                }
#endif
            }
            break;

            case 't':
            {
                if (F_StrCmp("text",Msg -> Spec,4) == 0)
                {
                    return 0xFFFF0000 | FV_ARRAY_TEXT;
                }
            }
        }
    }
    return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
    if (Msg -> Notify)
    {
        STATIC FDOCValue numeric_string_id[] =
        {
            { "shine",       FV_Pen_Shine       },
            { "halfshine",   FV_Pen_HalfShine   },
            { "fill",        FV_Pen_Fill        },
            { "halfshadow",  FV_Pen_HalfShadow  },
            { "shadow",      FV_Pen_Shadow      },
            { "halfdark",    FV_Pen_HalfDark    },
            { "dark",        FV_Pen_Dark        },
            { "text",        FV_Pen_Text        },
            { "highlight",   FV_Pen_Highlight   },

            F_ARRAY_END
        };
        
        uint32 rc;
 
        rc = F_SuperDo(Class,Obj,Method,numeric_string_id[(uint32)(Msg -> Spec)].Name,Msg -> Notify);

        return rc;
    }

    return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    uint32 value = F_Do(Obj,F_IDM(FM_Adjust_Query),Msg -> Spec);
    
    F_Do(LOD -> slider,FM_Set,FA_NoNotify,TRUE, "Value",0x0000FFFF & value, TAG_DONE);
}
//+
