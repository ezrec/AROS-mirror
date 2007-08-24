/*** Includes **************************************************************/

#include <libraries/feelin.h>
#include <graphics/text.h>

#include <proto/exec.h>
#include <proto/feelin.h>

#include "_locale/enums.h"

/*** Structures ************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         String;
    FObject                         Dialog;
};

/*** Dynamics **************************************************************/

enum    {                                       // attributes

        FA_PopFont_Contents

        };

