/*** Includes **************************************************************/

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#define       GfxBase               FeelinBase -> Graphics

/*** Structures ************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    bits32                          Flags;
    STRPTR                          Info;
};

/*** Dynamic ***************************************************************/

enum    {                                       // Attributes

        FA_Gauge_Simple,
        FA_Gauge_Info

        }; 

enum    {                                       // Resolved

        FA_Numeric_Value,
        FA_Numeric_Min,
        FA_Numeric_Max

        };

#define FF_Gauge_Simple                         (1L << 0)

/***************************************************************************/
/*** Preferences ***********************************************************/
/***************************************************************************/

struct p_LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         Example;
    struct FeelinSignalHandler      SignalHandler;
    int32                           Val;
};

#define FM_Gauge_Update                         (FCCM_BASE + 1)

enum    {                                       // Preference Auto

        p_FA_Numeric_Min,
        p_FA_Numeric_Max,
        p_FM_Numeric_Increase

        };

