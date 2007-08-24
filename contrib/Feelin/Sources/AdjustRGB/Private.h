/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>
#include <stdlib.h>

#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <intuition/intuition.h>

#include "_locale/enums.h"

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {
        
        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject,

        FM_AdjustRGB_PickBegin,
        FM_AdjustRGB_PickEnd
 
        };

enum    {                                       // autos

        FA_Numeric_Value,
        FA_String_Contents,
        FA_String_Integer

        };
         
struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    FObject                         r,g,b;
    FObject                         h,s,v;
    FObject                         string_r;
    FObject                         string_g;
    FObject                         string_b;
    FObject                         string_h;
    FObject                         string_s;
    FObject                         string_v;
    FObject                         html;
    FObject                         pick;

    STRPTR                          pick_spec;
};

