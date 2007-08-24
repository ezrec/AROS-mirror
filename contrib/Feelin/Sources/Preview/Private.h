/****************************************************************************
*** includes ****************************************************************
****************************************************************************/

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#include "_locale/enums.h"
 
#define       GfxBase               FeelinBase -> Graphics

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

enum    {                                       // attributes

        FA_Preview_Spec,
        FA_Preview_Alive,
        FA_Preview_Couple

        };
        
enum    {                                       // methods
        
        FM_Preview_Query,
        FM_Preview_ParseXML,
        FM_Preview_ToString,
        FM_Preview_Adjust,

        /* private */

        FM_Preview_Update
 
        };

struct  FS_Preview_Update                       { int32 Response; };

enum    {                                       // autos

        FA_Document_Source,
        FA_Document_SourceType,
        FM_Document_AddIDs,
        FM_Document_Resolve,

        FA_XMLDocument_Markups

        };
/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    FObject                         TD;
    FObject                         Adjust;
    FObject                         AdjustWindow;
    FObject                         XMLDocument;
 
    STRPTR                          spec;
    bits32                          flags;
};

#define FF_PREVIEW_ALIVE                        (1 << 0)
#define FF_PREVIEW_COUPLE                       (1 << 1)
#define FF_PREVIEW_CLICKED                      (1 << 2)
