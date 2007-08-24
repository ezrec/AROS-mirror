/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <graphics/text.h>
#include <libraries/feelin.h>

#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    STRPTR                          Text;
    STRPTR                          Prep[2];
    
    FObject                         TextDisplay;

    bits32                          Flags;
};

#define FF_Text_HCenter                         (1 << 0)
#define FF_Text_VCenter                         (1 << 1)
#define FF_Text_Static                          (1 << 2)
#define FF_Text_Shortcut                        (1 << 3)

