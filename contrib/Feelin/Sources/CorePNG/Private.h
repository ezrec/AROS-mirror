/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>

#if 0
#include <proto/mathieeedoubbas.h>
#include <proto/mathieeedoubtrans.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#endif

#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/coremedia.h>

#define DOSBase                     FeelinBase -> DOS

#ifdef __AROS__
#include <png.h>
#else
#include "libpng/png.h"
#endif

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    {

        FA_CoreMedia_Source,
        FA_CoreMedia_SourceType,
        FA_CoreMedia_Public

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    /* CorePicture */

    FCorePicturePublic              Public;

    /* CorePNG */

    BPTR                            handle;
};

