/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <dos/dostags.h>
#include <libraries/feelin.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility
#define GfxBase                     FeelinBase -> Graphics

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

enum    { // attributes

        FA_Thread_Entry,
        FA_Thread_StackSize,
        FA_Thread_Priority,
        FA_Thread_Name,
        FA_Thread_Process,
        FA_Thread_UserData

        };

enum    { // methods

        FM_Thread_Send

        };

enum    { // commands

        FV_Thread_Init = FV_Thread_Reserved1,
        FV_Thread_Exit = FV_Thread_Reserved2,

        };

struct FS_Thread_Init
{
    FClass                         *Class;
    FObject                         Object;
    F_THREAD_ENTRY_PROTO         ((*UserEntry));
    APTR                            UserData;
    struct FeelinBase              *Feelin;
};

struct LocalObjectData
{
    struct Process                 *process;
    struct SignalSemaphore          arbitrer;
};
