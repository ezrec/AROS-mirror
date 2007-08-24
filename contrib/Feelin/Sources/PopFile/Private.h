/*** Includes **************************************************************/

#include <proto/dos.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

#define DOSBase                                 FeelinBase -> DOS
#define UtilityBase                             FeelinBase -> Utility

/****************************************************************************
**** IDs ********************************************************************
****************************************************************************/

enum    {                                       // attributes

        FA_PopFile_Contents

        };

/****************************************************************************
**** Object *****************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         String;
    FObject                         Dialog;
    FObject                         Chooser;
};

