/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#define GfxBase                     FeelinBase -> Graphics
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {                           // Attributes

        FA_Image_Spec

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         ImageDisplay;
    STRPTR                          image_spec;
    FBox                            Origin;
};

