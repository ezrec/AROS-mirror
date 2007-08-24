/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#define UtilityBase             FeelinBase -> Utility
#define GfxBase                 FeelinBase -> Graphics

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FA_Bar_Title,
        FA_Bar_PreParse

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    bits32                          Flags;

    FAreaPublic                    *AreaPublic;

    FObject                         TD;
    STRPTR                          Title;
    STRPTR                          PreParse;
    FColor                         *ColorUp;
    FColor                         *ColorDown;

// Preferences

    STRPTR                          p_PreParse;
    STRPTR                          p_ColorUp;
    STRPTR                          p_ColorDown;
};

#define FF_Bar_Vertical                         (1L << 0)
