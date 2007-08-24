/*** Includes **************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

/*** class *****************************************************************/

enum    {

        FM_PreferenceGroup_Load,
        FM_PreferenceGroup_Save,
        FM_PreferenceGroup_FindObject

        };

enum    {

        FA_PreferenceGroup_XMLSource,
        FA_PreferenceGroup_XMLDefinitions,
        FA_PreferenceGroup_XMLReferences,
        FA_PreferenceGroup_XMLObject,
        FA_PreferenceGroup_Script

        };
        
struct LocalObjectData
{
    FObject                         XMLObject;
    FPreferenceScript              *Script;
    FXMLReference                  *References;
    FObject                        *Objects;
};

