/****************************************************************************
*** includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define GfxBase                                 FeelinBase -> Graphics

/****************************************************************************
*** class *******************************************************************
****************************************************************************/

enum    {                                       // attributes

        FA_PreviewFrame_Hatching

        };

enum    {                                       // resolved

        FA_Preview_Spec,
        FA_Preview_Couple

        };

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         Frame;
    FFramePublic                   *FramePublic;
    int32                           hatching;
};

