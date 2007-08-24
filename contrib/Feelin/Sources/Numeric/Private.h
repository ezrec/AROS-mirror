#include <proto/feelin.h>
#include <proto/utility.h>

#include <libraries/feelin.h>

#define UtilityBase                 FeelinBase -> Utility

/*** Structures ************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    int32                           Value;
    int32                           Min;
    int32                           Max;
    int32                           Default;
    uint32                          Steps;
    STRPTR                          String;
    STRPTR                         *StringArray;
    STRPTR                          Format;
    struct Hook                    *ComputeHook;
};

/*** Dynamic ***************************************************************/

enum    {                                       // Methods

        FM_Numeric_Increase,
        FM_Numeric_Decrease,
        FM_Numeric_Stringify,
        FM_Numeric_Reset

        };

enum    {                                       // Attributes

        FA_Numeric_Default,
        FA_Numeric_Value,
        FA_Numeric_Min,
        FA_Numeric_Max,
        FA_Numeric_Step,
        FA_Numeric_Steps,
        FA_Numeric_Format,
        FA_Numeric_Buffer,
        FA_Numeric_ComputeHook,
        FA_Numeric_StringArray

        };
