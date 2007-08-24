/*** Includes **************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

/*** Structures ************************************************************/

struct LocalObjectData
{
    FObject                         Chooser;
};

/*** Dynamics **************************************************************/

enum    {                                       // attributes

        FA_FontDialog_Spec

        };

enum    {

        FM_FontDialog_Choosed

        };
