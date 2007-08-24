/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/feelin.h>
#include <proto/iffparse.h>
#include <proto/utility.h>

#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/notify.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <libraries/feelin.h>
#include <libraries/iffparse.h>
#include <feelin/preference.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

enum    {

        FM_Meta_Debug

        };

#define TEMPLATE                    "READ/S,WRITE/S,RESOLVE/S"
#define DEBUGVARNAME                "ENV:Feelin/DBG_PREFERENCE"

enum    {

        OPT_READ,
        OPT_WRITE,
        OPT_RESOLVE

        };

struct ClassUserData
{
    FList                           PreferenceList;
    FObject                         ListSemaphore;
    bits32                          Flags;
    
    FObject                         db_Notify;
    int32                           db_Read;
    int32                           db_Write;
    int32                           db_Resolve;
};

#define db_Array                                db_Read

/****************************************************************************
*** Attributes * Methods ****************************************************
****************************************************************************/

enum    {                                       // Methods

        FM_Preference_Find,
        FM_Preference_Add,
        FM_Preference_AddLong,
        FM_Preference_AddString,
        FM_Preference_Remove,
        FM_Preference_Clear,
        FM_Preference_Resolve,
        FM_Preference_ResolveInt,
        FM_Preference_Read,
        FM_Preference_Write

        };

enum    {                                       // Attributes

        FA_Preference_Name,
        FA_Preference_Reference,
        FA_Preference_Update

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

typedef struct FeelinPreferenceItem
{
    struct FeelinPreferenceItem    *Next;
    STRPTR                          Key;
    uint32                          KeyLength;
    APTR                            Data;
    uint16                          NameSize;
    uint16                          DataSize;
/* long aligned name bellow */
/* data bellow */
}
FPItem;

struct LocalObjectData
{
    APTR                            Pool;
    FHashTable                     *Table;
    FObject                         Notify;
    STRPTR                          Name;
    STRPTR                          FileName;
    FObject                         Reference;
};

/*** Prototypes ************************************************************/

STRPTR      p_resolve_name      (FClass *Class,FObject Obj,STRPTR Name);
APTR        p_obtain_storage    (FClass *Class,FObject Obj);
int32       p_remove_item       (FClass *Class,FObject Obj,FPItem *Item,uint32 Hash);

