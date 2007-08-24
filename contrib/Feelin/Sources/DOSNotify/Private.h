/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <dos/notify.h>
#include <libraries/feelin.h>

#define       DOSBase               FeelinBase -> DOS
#define       UtilityBase           FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

struct ClassUserData
{
    FObject                         Thread;
    uint32                          id_Send;
    uint32                          id_Update;
};

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {                                       // Attributes

        FA_DOSNotify_Name,
        FA_DOSNotify_Update

        }; 

enum    {

        FM_Thread_Send

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    struct NotifyRequest            Notify;
};

/****************************************************************************
*** Thread ******************************************************************
****************************************************************************/

F_THREAD_ENTRY_PROTO(Thread_Main);

enum    {

        FV_Thread_AddNotify = FV_Thread_Dummy,
        FV_Thread_RemNotify

        };

struct  FS_Thread_AddNotify                     { struct NotifyRequest *NReq; FObject Object; };
struct  FS_Thread_RemNotify                     { struct NotifyRequest *NReq; };

