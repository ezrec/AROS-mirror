/*** Includes **************************************************************/

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define GfxBase                     FeelinBase -> Graphics

/*** Structures ************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    int16                           Height;
    int16                           MinWidth;
};
