#include <proto/exec.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#include "_locale/enums.h"

#define       IntuitionBase         FeelinBase -> Intuition

typedef struct FeelinPGroup
{
    struct FeelinPGroup            *Next;
    struct FeelinPGroup            *Prev;
    FObject                         Object;
    STRPTR                          Name;
    uint32                          Position;
}
FPGroup;

/***/

struct LocalObjectData
{
    STRPTR                          Title;

    FObject                         Master;             // Master Group
    FObject                         GroupTitle;
    FObject                         list;
    FObject                         Application;
    FObject                         Preferences;
    FObject                         active_page;
    FObject                         MList;

    FList                           GroupList;

    struct Hook                     h_Disp;
};

/*** Dynamic ***************************************************************/

enum    {                                       // Methods

        FM_PreferenceEditor_Load,
        FM_PreferenceEditor_Save,
        FM_PreferenceEditor_Use,
        FM_PreferenceEditor_Test,
        FM_PreferenceEditor_Cancel,
        FM_PreferenceEditor_Change

        };

struct  FS_PreferenceEditor_Change              { uint32 Which; };

enum    {                                       // AutoResolve

        FM_List_GetEntry,
        FM_Preference_Add,
        FM_Preference_AddLong,
        FM_Preference_AddString,
        FM_Preference_Resolve,
        FM_Preference_ResolveInt,
        FM_Preference_Read,
        FM_Preference_Write,
        FM_Preference_Clear,
        FM_PreferenceGroup_Load,
        FM_PreferenceGroup_Save

        };

#define FASTLIST_NAME                           "ENV:Feelin/CLS_PREFERENCEEDITOR_FASTLIST"
#ifndef PATH_MAX
#define PATH_MAX                                1024
#endif

