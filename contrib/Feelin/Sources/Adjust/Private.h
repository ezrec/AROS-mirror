/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

/****************************************************************************
*** object ******************************************************************
****************************************************************************/

enum    {                                       // Attributes

        FA_Adjust_Spec,
        FA_Adjust_Couple,
        FA_Adjust_Separator,
        FA_Adjust_Preview,
        FA_Adjust_PreviewTitle,
        FA_Adjust_PreviewClass

        };
        
enum    {                                       // Methods

        FM_Adjust_Query,
        FM_Adjust_ParseXML,
        FM_Adjust_ToString,
        FM_Adjust_ToObject,

        /* private */

        FM_Adjust_TogglePreview

        };

enum    {                                       // autos

        FA_Preview_Spec,

        FA_Document_Source,
        FA_Document_SourceType,
        FM_Document_AddIDs,
        FM_Document_Resolve,

        FA_XMLDocument_Markups

        };
        
enum    {

        FV_TEXT_NONE,
        FV_TEXT_RENDER,
        FV_TEXT_SELECT,
        FV_TEXT_NO_RENDER,
        FV_TEXT_NO_SELECT

        };

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
 
    STRPTR                          spec;
    STRPTR                          render;
    STRPTR                          select;
    FObject                         preview;
    FObject                         text;
    FObject                         reset;
    uint8                           last_text;
    uint8                           separator;
    uint16                          _free0;
    FObject                         XMLDocument;
};

/* 'text' is only available when adjusting a double thing. */
