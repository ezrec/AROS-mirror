#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <libraries/iffparse.h>
#include <libraries/feelin.h>

#define DOSBase                     FeelinBase->DOS
#define UtilityBase                 FeelinBase->Utility
extern struct Library              *IFFParseBase;

/*** Dynamics **************************************************************/

enum    {                                       // Methods

        FM_Dataspace_Add,
        FM_Dataspace_Remove,
        FM_Dataspace_Find,
        FM_Dataspace_Clear,
        FM_Dataspace_Merge,
        FM_Dataspace_ReadIFF,
        FM_Dataspace_WriteIFF

        };

enum    {                                       // Attributes

        FA_Dataspace_Pool

        };

/*** Structures ************************************************************/

struct LocalObjectData
{
    bits32                          Flags;
    APTR                            Pool;
    FList                           Chunks;
};

struct FeelinChunkData
{
    struct FeelinChunkData         *Next;
    struct FeelinChunkData         *Prev;
/* end of FeelinNode header */
    uint32                          ID;
    uint32                          Size;
/* data bellow */
};

#define FF_Dataspace_Allocated                  (1L << 0)
