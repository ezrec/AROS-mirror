#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <dos/dos.h>
#include <libraries/feelin.h>
#include <feelin/xmlobject.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** class *******************************************************************
****************************************************************************/

enum    {

        FA_XMLObject_Definitions,
        FA_XMLObject_References,
        FA_XMLObject_Tags

        };

enum    {

        FM_Document_Read,
        FM_Document_Resolve,
        FM_XMLObject_Find,
        FM_XMLObject_Build,
        FM_XMLObject_Create,

        };

enum    {                                       // resolved

        FM_Document_AddIDs,
        FM_Document_Log

        };

/*** object ****************************************************************/

struct LocalObjectData
{
    FHashTable                     *HTable;
    FXMLDefinition                 *Definitions;                // see FA_XMLObject_Definitions
    FXMLReference                  *References;                 // see FA_XMLObject_References
    struct TagItem                 *InitTags;                   // see FA_XMLObject_Tags
    struct TagItem                 *BuiltTags;
};
