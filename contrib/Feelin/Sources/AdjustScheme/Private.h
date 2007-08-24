/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {                                       // methods

        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject
        
        };

enum    {                                       // resolves

        FM_Adjust_ParseXML

        };

enum    {                                       // autos

        FA_Preview_Spec

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                         contrast;
    FObject                         saturation;
    FObject                         edit;
    FObject                         adjust;

    FObject                         shine;
    FObject                         fill;
    FObject                         dark;
    FObject                         text;
    FObject                         high;
};

