/*** Includes **************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

enum    {                                       // methods

        FM_Adjust_Query,
        FM_Adjust_ToObject

        };

enum    {                                       // resolves

        FM_Adjust_ToString

        };

/***************************************************************************/

struct LocalObjectData
{
    FObject                         page;
    FObject                         page_bitmap;
    FObject                         color;
    FObject                         raster;
    FObject                         gradient;
    FObject                         picture;
    FObject                         brush;
};

