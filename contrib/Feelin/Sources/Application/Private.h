/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <stdlib.h>

#include <proto/exec.h>
#include <proto/keymap.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/rexxsyslib.h>
#include <proto/commodities.h>
#include <proto/iffparse.h>
#include <proto/feelin.h>

#include <intuition/intuition.h>
#include <libraries/commodities.h>
#include <libraries/iffparse.h>
#include <rexx/storage.h>
#include <workbench/workbench.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

/****************************************************************************
*** Shared variables ********************************************************
****************************************************************************/

#define DOSBase                     FeelinBase -> DOS
#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

#define TEMPLATE                    "PUSH/S,EVENTS/S"

enum  {

      OPT_PUSH,
      OPT_EVENTS

      };

struct ClassUserData
{
    struct Library                 *IFFParseBase;
    struct Library                 *CxBase;
    struct RxsLib                  *RexxSysBase;
    struct Library                 *DiskfontBase;
    struct Library                 *KeymapBase;

    FObject                         db_Notify;

    int32                           db_Push;
    int32                           db_Events;
};

#define db_Array                                db_Push

#define IFFParseBase                            CUD -> IFFParseBase
#define CxBase                                  CUD -> CxBase
#define RexxSysBase                             CUD -> RexxSysBase
#define DiskfontBase                            CUD -> DiskfontBase
#define KeymapBase                              CUD -> KeymapBase

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

typedef struct FeelinTimeHeader
{
    struct FeelinTimeHeader        *Next;
    struct FeelinTimeHeader        *Prev;
/*  end of FeelinNode header */
    struct FeelinSignalHandler     *Handler;
}
FTimeHeader;

typedef struct FeelinTimeNode
{
    struct FeelinTimeHeader        *Next;
    struct FeelinTimeHeader        *Prev;
    struct FeelinSignalHandler     *Handler;
/*  end of FeelinTimeHeader header */
    struct timerequest              TReq;
}
FTimeNode;

typedef struct FeelinAppPush
{
    struct FeelinAppPush           *Next;
    struct FeelinAppPush           *Prev;
//  end of FeelinNode header
    FObject                         Target;
    uint32                          Method;
    APTR                            Msg;
//  message data below
}
FPush;

struct LocalObjectData
{
    FObject                         AppServer;
    FObject                         Dataspace;
    FObject                         Preferences;
    FObject                         menu;
    
    FAppResolveMap                 *ResolveMap;
    FNotifyHandler                 *PreferencesNH;

    bits32                          Flags;

    STRPTR                          Title;
    STRPTR                          Version;
    STRPTR                          Copyright;
    STRPTR                          Author;
    STRPTR                          Description;
    STRPTR                          Base;
    
    APTR                            Broker;
    struct NewBroker               *BrokerInit;
    struct MsgPort                 *AppPort;
    struct MsgPort                 *WindowPort;
    struct MsgPort                 *TimersPort;
    struct MsgPort                 *BrokerPort;

    int32                           SleepCount;     // when == 0 application is awaken
    bits32                          WaitSignals;
    bits32                          UserSignals;    // FA_Application_UserSignals

    FList                           PushedMethodList;
    
    struct timerequest             *TimeRequest;
    FList                           Timers;         // FeelinTime
    FList                           Signals;        // FeelinSignalHandler

    FClass                         *DisplayClass;
    FObject                         Display;
    FPalette                       *Scheme;
    FPalette                       *DisabledScheme;

// double_click
    
    FObject                         ev_Window;
    uint8                           ev_Button;
    uint16                          ev_MouseX;
    uint16                          ev_MouseY;
    uint32                          ev_Secs;
    uint32                          ev_Micros;

    uint8                           _pad0;
    uint16                          _pad1;
 
// Resolved
    IX                              Keys[FV_KEY_COUNT];
// Preferences
    STRPTR                          p_Keys;
    STRPTR                          p_Scheme;
    STRPTR                          p_DisabledScheme;
};

#define FF_Application_Run                      (1 << 0)

/* The FF_Application_Run flags is set by  the  method  FM_Application_Run.
The  flag is checked within the main loop of the application. When the flag
is cleared the method ends (the application quits the main loop. */

#define FF_Application_Update                   (1 << 1)

/* The FF_Application_Update flag is set by the  notification  set  on  the
FA_Preference_Update  attribute  of the FC_Preference object. When the flag
is  set  the  application  invoke  the  methods  FM_Application_Sleep   and
FM_Application_Awake to read new preferences */

#define FF_Application_Setup                    (1 << 2) // setup done
#define FF_APPLICATION_INHERITED_PREFS          (1 << 3)

/*** Dynamics **************************************************************/

enum    { // autos

        FM_Dataspace_Add,
        FM_Dataspace_Find,
        FM_Dataspace_Clear,
        FM_Dataspace_ReadIFF,
        FM_Dataspace_WriteIFF,
        FM_Preference_Read,
        FM_Preference_Resolve,
        FM_Preference_ResolveInt

        };

bits32 app_collect_signals(FClass *Class,FObject Obj);

#define DEF_DISABLEDSCHEME                      "<scheme shine='halfshine' dark='shadow' text='halfshadow' highlight='fill' />"
