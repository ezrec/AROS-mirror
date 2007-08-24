/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <libraries/feelin.h>
#include <feelin/document.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

#define       DOSBase               FeelinBase -> DOS
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** Attributes * Methods * Values *******************************************
****************************************************************************/

enum    {

        FA_Document_Pool,
        FA_Document_Source,
        FA_Document_SourceType,
        FA_Document_Version,
        FA_Document_Revision

        };

enum    {

        FM_Document_Read,
        FM_Document_Parse,
        FM_Document_AddIDs,
        FM_Document_Resolve,
        FM_Document_FindName,
        FM_Document_ObtainName,
        FM_Document_Log,
        FM_Document_Clear

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    APTR                            Pool;
    FHashTable                     *HTable;
    uint8                           version;
    uint8                           revision;
    uint16                          _reserved0;
    STRPTR                          sourcename;
};

