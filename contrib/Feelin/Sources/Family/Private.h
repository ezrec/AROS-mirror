/****************************************************************************
*** includes ****************************************************************
****************************************************************************/

#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <libraries/feelin.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility
extern struct ClassUserData        *CUD;

/****************************************************************************
*** Meta ********************************************************************
****************************************************************************/

enum    {

        FM_Meta_Debug

        };

#define TEMPLATE                    "ADD/S,REM=REMOVE/S"
#define DEBUGVARNAME                "ENV:Feelin/DBG_FAMILY"

enum  {

      OPT_ADD,
      OPT_REM

      };

struct ClassUserData
{
    APTR                            NodePool;
    FObject                         db_Notify;
    int32                           db_AddMember;
    int32                           db_RemMember;
};

#define db_Array                                db_AddMember

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    FList                           list;
    FObject                         owner;
    struct Hook                    *create_node_hook;
    struct Hook                    *delete_node_hook;
};

