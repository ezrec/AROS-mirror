/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {                                       // methods
        
        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject
 
        };

enum    {                                       // resolves

        FM_Adjust_ParseXML,
        FA_Adjust_Preview,
        FA_Adjust_Spec

        };

#define SAMPLE_NUM                              16
         
struct LocalObjectData
{
    FObject                         spacing;
    FObject                         slider;
    FObject                         sample[SAMPLE_NUM + 1];
};

struct in_FeelinSample
{
    uint16                          render_id;
    uint16                          select_id;
    STRPTR                          framespec;
    STRPTR                          backspec;
};
