/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    { // methods

        FM_Adjust_Query,
        FM_Adjust_ToString,
        FM_Adjust_ToObject
        
        };

enum    { // resolves

        FM_Adjust_ParseXML

        };
         
struct LocalObjectData
{
    FObject                         file;
    FObject                         mode;
    FObject                         filter;
    FObject                         back;
};

