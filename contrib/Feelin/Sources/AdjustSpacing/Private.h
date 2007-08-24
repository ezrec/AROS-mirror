/***************************************************************************/
/*** includes **************************************************************/
/***************************************************************************/

#include <proto/exec.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

/***************************************************************************/
/*** object ****************************************************************/
/***************************************************************************/

enum    {

        FA_AdjustSpacing_Spec                   // [ISG]

        };

struct LocalObjectData
{
    FObject                         LSpacing;
    FObject                         RSpacing;
    FObject                         TSpacing;
    FObject                         BSpacing;

    FInner                          spacing_spec;
};

