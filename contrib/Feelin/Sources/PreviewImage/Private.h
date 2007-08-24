/***************************************************************************/
/*** includes **************************************************************/
/***************************************************************************/

#include <string.h>

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define       GfxBase               FeelinBase -> Graphics

/***************************************************************************/
/*** class *****************************************************************/
/***************************************************************************/

enum    {                                       // resolved

        FA_Preview_Spec
 
        };
 
struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         preview;
    FBox                            preview_box;
};
