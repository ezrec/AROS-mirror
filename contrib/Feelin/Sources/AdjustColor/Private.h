/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <proto/feelin.h>

#include <libraries/feelin.h>

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {                                       // methods

        FM_Adjust_Query,
        FM_Adjust_ToObject

        };
        
enum    {                                       // attributes

        FA_AdjustColor_AddScheme,
        FA_AdjustColor_AddPen,
        FA_AdjustColor_AddRGB

        };

enum    {                                       // resolves

        FM_Adjust_ToString

        };
        
struct LocalObjectData
{
    FObject                         page;
    FObject                         sch;
    FObject                         pen;
    FObject                         rgb;
};

