/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

/****************************************************************************
*** IDs *********************************************************************
****************************************************************************/

enum    {                                       // resolves

        FM_Adjust_ParseXML

        };

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {
        
        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject
 
        };
         
struct LocalObjectData
{
    FObject                         angle;
    FObject                         cycle_angle;
    FObject                         cycle_type;

    FObject                         group1;
    FObject                         group2;
    FObject                         group3;
    FObject                         group4;

    FObject                         spec1;
    FObject                         spec2;
    FObject                         spec3;
    FObject                         spec4;

    FObject                         sep;

    FObject                         label1;
    FObject                         label2;
    FObject                         label3;
    FObject                         label4;

};
