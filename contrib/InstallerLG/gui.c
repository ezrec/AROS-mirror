//----------------------------------------------------------------------------
// gui.c:
//
// MUI based GUI. On non Amiga systems this is, except for some stdout prints
// to aid testing, a stub.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "gui.h"
#include "version.h"
#include "resource.h"

#ifdef AMIGA
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <proto/alib.h>
# ifndef __MORPHOS__
#include <proto/debug.h>
# else
#include <clib/debug_protos.h>
# endif
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#endif /* AMIGA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>

#ifdef AMIGA
//----------------------------------------------------------------------------
// MUI helper macros
//----------------------------------------------------------------------------
#ifdef __MORPHOS__
# define DISPATCH_GATE(C) &C ## Gate
# define DISPATCH_ARGS void
# define DISPATCH_HEAD \
  Class *cls = (Class *) REG_A0; \
  Object *obj = (Object *) REG_A2; \
  Msg msg = (Msg) REG_A1
# define CLASS_DEF(C) \
  static IPTR C ## Dispatch (void); \
  struct MUI_CustomClass * C ## Class; \
  const struct EmulLibEntry C ## Gate = \
  { TRAP_LIB, 0, (void (*) (void)) C ## Dispatch }; \
  struct C ## Data
#else
# define DoSuperNew(C,O,...) DoSuperNewTags(C,O,NULL,__VA_ARGS__)
# define DISPATCH_HEAD
# define DISPATCH_ARGS Class *cls, Object *obj, Msg msg
# define DISPATCH_GATE(C) C ## Dispatch
# define CLASS_DEF(C) \
  struct MUI_CustomClass * C ## Class; \
  struct C ## Data
#endif
#define DISPATCH(C) static IPTR C ## Dispatch (DISPATCH_ARGS)
#define CLASS_DATA(C) C ## Data
#define TAGBASE_LG (TAG_USER | 27<<16)
#define MUIDSP static inline __attribute__((always_inline))

//----------------------------------------------------------------------------
// Debug and logging macros
//----------------------------------------------------------------------------
#define GPUT(E,S) KPrintF((CONST_STRPTR)"%s:\t%s\t(%s)\n",E,S,__func__)
#define GERR(S) GPUT("ERR",S)
#define WARN(S) if(LLVL>0) GPUT("WARN",S)
#define INFO(S) if(LLVL>1) GPUT("INFO",S)

//----------------------------------------------------------------------------
// The installer window
//----------------------------------------------------------------------------
Object *Win;

//----------------------------------------------------------------------------
// InstallerGui - Class members
//----------------------------------------------------------------------------
CLASS_DEF(InstallerGui)
{
    // Timer.
    struct MUI_InputHandlerNode Ticker;

    // Widgets.
    Object *ExpertLevel, *UserLevel, *Progress,
           *Complete, *Pretend, *Bottom, *String,
           *Number, *Empty, *Text, *List,
           *Log, *Top, *Ask, *Yes, *No;

    // String buffer.
    char Buf[1 << 10];
};

//----------------------------------------------------------------------------
// InstallerGui - Public methods
//----------------------------------------------------------------------------
#define MUIM_InstallerGui_Init             (TAGBASE_LG + 101)
#define MUIM_InstallerGui_Welcome          (TAGBASE_LG + 102)
#define MUIM_InstallerGui_CopyFilesStart   (TAGBASE_LG + 104)
#define MUIM_InstallerGui_CopyFilesSetCur  (TAGBASE_LG + 105)
#define MUIM_InstallerGui_CopyFilesEnd     (TAGBASE_LG + 106)
#define MUIM_InstallerGui_Exit             (TAGBASE_LG + 107)
#define MUIM_InstallerGui_Complete         (TAGBASE_LG + 108)
#define MUIM_InstallerGui_Ticker           (TAGBASE_LG + 109)
#define MUIM_InstallerGui_CopyFilesAdd     (TAGBASE_LG + 110)
#define MUIM_InstallerGui_Confirm          (TAGBASE_LG + 111)
#define MUIM_InstallerGui_Message          (TAGBASE_LG + 112)
#define MUIM_InstallerGui_Abort            (TAGBASE_LG + 113)
#define MUIM_InstallerGui_Radio            (TAGBASE_LG + 114)
#define MUIM_InstallerGui_Bool             (TAGBASE_LG + 115)
#define MUIM_InstallerGui_String           (TAGBASE_LG + 116)
#define MUIM_InstallerGui_Number           (TAGBASE_LG + 117)
#define MUIM_InstallerGui_CheckBoxes       (TAGBASE_LG + 118)
#define MUIM_InstallerGui_AskFile          (TAGBASE_LG + 119)
#define MUIM_InstallerGui_PageSet          (TAGBASE_LG + 120)

//----------------------------------------------------------------------------
// InstallerGui - Init parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Init
{
    ULONG MethodID;
};

//----------------------------------------------------------------------------
// InstallerGui - Welcome parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Welcome
{
    ULONG MethodID;
    ULONG Message;
    ULONG Level;
    ULONG Log;
    ULONG Pretend;
    ULONG MinLevel;
    ULONG NoPretend;
    ULONG NoLog;
};

//----------------------------------------------------------------------------
// InstallerGui - CopyFilesStart parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_CopyFilesStart
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG List;
    ULONG Confirm;
};

//----------------------------------------------------------------------------
// InstallerGui - CopyFilesSetCur parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_CopyFilesSetCur
{
    ULONG MethodID;
    ULONG File;
    ULONG NoGauge;
};

//----------------------------------------------------------------------------
// InstallerGui - Exit parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Exit
{
    ULONG MethodID;
    ULONG Message;
};

//----------------------------------------------------------------------------
// InstallerGui - Complete parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Complete
{
    ULONG MethodID;
    ULONG Progress;
};

//----------------------------------------------------------------------------
// InstallerGui - CopyFilesAdd parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_CopyFilesAdd
{
    ULONG MethodID;
    ULONG File;
};

//----------------------------------------------------------------------------
// InstallerGui - Run parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Confirm
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
};

//----------------------------------------------------------------------------
// InstallerGui - Message parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Message
{
    ULONG MethodID;
    ULONG Message;
    ULONG Immediate;
};

//----------------------------------------------------------------------------
// InstallerGui - Abort parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Abort
{
    ULONG MethodID;
    ULONG Message;
};

//----------------------------------------------------------------------------
// InstallerGui - Radio parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Radio
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Names;
    ULONG Default;
    ULONG Halt;
};

//----------------------------------------------------------------------------
// InstallerGui - Bool parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Bool
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Yes;
    ULONG No;
};

//----------------------------------------------------------------------------
// InstallerGui - String parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_String
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Default;
    ULONG Halt;
};

//----------------------------------------------------------------------------
// InstallerGui - Number parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_Number
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Min;
    ULONG Max;
    ULONG Default;
    ULONG Halt;
};

//----------------------------------------------------------------------------
// InstallerGui - CheckBoxes parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_CheckBoxes
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Names;
    ULONG Default;
    ULONG Halt;
};

//----------------------------------------------------------------------------
// InstallerGui - AskFile parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_AskFile
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG NewPath;
    ULONG Disk;
    ULONG Assign;
    ULONG Default;
    ULONG Dir;
};

//----------------------------------------------------------------------------
// InstallerGui - PageSet parameters
//----------------------------------------------------------------------------
struct MUIP_InstallerGui_PageSet
{
    ULONG MethodID;
    ULONG Message;
    ULONG Help;
    ULONG Top;
    ULONG Bottom;
};

//----------------------------------------------------------------------------
// InstallerGui - private values
//----------------------------------------------------------------------------
#define MUIV_InstallerGui_FirstButton      (TAGBASE_LG + 200)
#define MUIV_InstallerGui_Proceed          (TAGBASE_LG + 200)
#define MUIV_InstallerGui_Abort            (TAGBASE_LG + 201)
#define MUIV_InstallerGui_Yes              (TAGBASE_LG + 202)
#define MUIV_InstallerGui_No               (TAGBASE_LG + 203)
#define MUIV_InstallerGui_Tick             (TAGBASE_LG + 204)
#define MUIV_InstallerGui_AbortOnly        (TAGBASE_LG + 205)
#define MUIV_InstallerGui_ProceedRun       (TAGBASE_LG + 206)
#define MUIV_InstallerGui_SkipRun          (TAGBASE_LG + 207)
#define MUIV_InstallerGui_AbortRun         (TAGBASE_LG + 208)
#define MUIV_InstallerGui_ProceedOnly      (TAGBASE_LG + 209)
#define MUIV_InstallerGui_LastButton       (TAGBASE_LG + 209)

// Pages
#define P_WELCOME                          0
#define P_COPYFILES                        1
#define P_FILEDEST                         2
#define P_PRETEND_LOG                      3
#define P_MESSAGE                          4
#define P_STRING                           5
#define P_NUMBER                           6
#define P_ASKFILE                          7

// Buttons
#define B_PROCEED_ABORT                    0
#define B_YES_NO                           1
#define B_ABORT                            2
#define B_PROCEED_SKIP_ABORT               3
#define B_PROCEED                          4
#define B_NONE                             5

//----------------------------------------------------------------------------
// InstallerGuiWait - [PRIVATE] Wait for notification(s)
// Input:             ULONG notif:  Start notification value
//                    ULONG range:  Number of values to check for
// Return:            Notifcation val. / zero on return id quit
//----------------------------------------------------------------------------
MUIDSP ULONG InstallerGuiWait(Object *obj, ULONG notif, ULONG range)
{
    ULONG sig = 0,
          ret = 0, n;

    // Set cycle chain for all buttons
    // within the notification range.
    for(n = 0; n < range; n++)
    {
        // Filter out ticks.
        if(notif + n != MUIV_InstallerGui_Tick)
        {
            // Find current button.
            Object *but = (Object *) DoMethod
            (
                obj, MUIM_FindUData,
                notif + n
            );

            // Don't trust the caller.
            if(but)
            {
                // Add button to cycle chain.
                set(but, MUIA_CycleChain, TRUE);

                // Activate the first button.
                if(n == 0)
                {
                    set(Win, MUIA_Window_ActiveObject, but);
                }
            }
            else
            {
                // Button doesn't exist.
                GERR(tr(S_UNER));
            }
        }
    }

    // Get MUI input.
    ret = DoMethod(_app(obj), MUIM_Application_NewInput, &sig);

    // Enter the message loop.
    while(ret != (ULONG) MUIV_Application_ReturnID_Quit)
    {
        // Iterate over all signals that we're
        // waiting for.
        for(n = 0; n < range; n++)
        {
            // Did we get what we're waiting for?
            if(ret == notif + n)
            {
                // Remove buttons within the range
                // from the cycle chain.
                for(n = 0; n < range; n++)
                {
                    // Filter out ticks.
                    if(notif + n != MUIV_InstallerGui_Tick)
                    {
                        // Find current button.
                        Object *but = (Object *) DoMethod
                        (
                            obj, MUIM_FindUData,
                            notif + n
                        );

                        // Don't trust the caller.
                        if(but)
                        {
                            set(but, MUIA_CycleChain, FALSE);
                        }
                    }
                }

                // Return notifier value.
                return ret;
            }
        }

        // It wasn't for us. Go to sleep again.
        if(sig)
        {
            sig = Wait(sig | SIGBREAKF_CTRL_C);

            // Are we getting killed?
            if(sig & SIGBREAKF_CTRL_C)
            {
                break;
            }
        }

        // Get new input.
        ret = DoMethod(_app(obj), MUIM_Application_NewInput, &sig);
    }

    // Quit application.
    return 0;
}

//----------------------------------------------------------------------------
// InstallerGuiInit - Initialize all the things
// Input:             -
// Return:            On success TRUE, FALSE otherwise.
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiInit(Class *cls,
                             Object *obj)
{
    static ULONG i = MUIV_InstallerGui_FirstButton;

    // Have we already done this?
    if(i != MUIV_InstallerGui_LastButton)
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Set notifications on all buttons.
        while(i <= MUIV_InstallerGui_LastButton)
        {
            // Find current button.
            Object *but = (Object *) DoMethod(obj, MUIM_FindUData, i);

            // Set notification if it exists.
            if(but)
            {
                DoMethod
                (
                    but, MUIM_Notify,
                    MUIA_Pressed, FALSE, _app(obj), 2,
                    MUIM_Application_ReturnID, i
                );
            }

            // Next button.
            i++;
        }

        // Enter in string gadget = proceed.
        DoMethod
        (
            my->String, MUIM_Notify,
            MUIA_String_Acknowledge,
            MUIV_EveryTime, _app(obj), 2,
            MUIM_Application_ReturnID,
            MUIV_InstallerGui_Proceed
        );

        // Exit upon close request.
        DoMethod
        (
            obj, MUIM_Notify, MUIA_Window_CloseRequest,
            TRUE, _app(obj), 2, MUIM_Application_ReturnID,
            MUIV_Application_ReturnID_Quit
        );

        // All done.
        return TRUE;
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
        return FALSE;
    }
}

//----------------------------------------------------------------------------
// InstallerGuiPageSet - [PRIVATE] Set page / buttons and display text
// Input:                int top:   Top page
//                       int btm:   Button page
//                       ULONG msg: Top text message
// Return:               TRUE on success, FALSE otherwise
//----------------------------------------------------------------------------
//
MUIDSP IPTR InstallerGuiPageSet(Class *cls,
                                Object *obj,
                                struct MUIP_InstallerGui_PageSet *msg)
{
    struct InstallerGuiData *my = INST_DATA(cls, obj);

    // Always a valid message string.
    const char *src = msg->Message ? (const char *)
                      msg->Message : "";

    // Select top and buttons.
    set(my->Top, MUIA_Group_ActivePage, msg->Top);
    set(my->Bottom, MUIA_Group_ActivePage, msg->Bottom);

    // NULL will disable the help bubble.
    set(my->Text, MUIA_ShortHelp, msg->Help);

    // Wrap at 50 (less ad hoc?).
    size_t cnt = strlen(src), len = 50;

    // Cap the size of the message.
    cnt = cnt < (sizeof(my->Buf) - 1) ?
          cnt : (sizeof(my->Buf) - 1);

    // Copy for wrapping.
    memcpy(my->Buf, src, cnt);

    // Do we need to word wrap?
    if(cnt > len)
    {
        // Start from the first point
        // where we need to wrap.
        size_t cur = len;

        // As long we have characters.
        while(cur < cnt)
        {
            // Last wrapping point.
            size_t ref = cur - len;

            // Go backwards from where we are
            // to a point where we can wrap.
            while(cur > ref && my->Buf[cur] > 33)
            {
                cur--;
            }

            // If we ended up where we started
            // we failed. Wrap in the middle of
            // the word in that case. This will
            // only happen with words >= len.
            if(cur == ref)
            {
                // Restore the previous point.
                cur += len;
            }
            else
            {
                // If we found a wrapping point
                // we must make sure that no NL
                // exists before this point but
                // after the previous point.
                while(++ref < cur)
                {
                    if(my->Buf[ref] == '\n')
                    {
                        // Skip wrap.
                        cur = ref;
                        break;
                    }
                }
            }

            // Wrap and continue.
            my->Buf[cur] = '\n';
            cur += len;
        }
    }

    // Terminate string.
    my->Buf[cnt] = '\0';

    // Show the message.
    set(my->Text, MUIA_ShowMe, FALSE);
    set(my->Text, MUIA_Text_Contents, my->Buf);
    set(my->Text, MUIA_ShowMe, TRUE);

    // Always.
    return TRUE;
}

//----------------------------------------------------------------------------
// InstallerGuiWelcome - Show welcome message. The Level, Log, and Pretend
//                       parameters are int pointers that act as both input
//                       and output.
// Input:                Message - the welcome message text
//                       Level - User level
//                       Log - Log to file
//                       Pretend - Pretend mode
//                       MinLevel - Minimum user level
//                       NoPretend - Disable pretend mode
//                       NoLog - Disable logging
// Return:               1 on success, 0 on abort
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiWelcome(Class *cls,
                                Object *obj,
                                struct MUIP_InstallerGui_Welcome *msg)
{
    // Show welcome page.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                NULL, P_WELCOME, B_PROCEED_ABORT))
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // If the minimum user level is set to 'expert',
        // disable radio buttons to indicate that there
        // is no choice to be made.
        if(msg->MinLevel == 2)
        {
            set(my->UserLevel, MUIA_Disabled, TRUE);
        }
        // If the minimum user level is set to 'average',
        // show 'average' and 'expert' only.
        else if(msg->MinLevel == 1)
        {
            // Hide the full one.
            set(my->UserLevel, MUIA_ShowMe, FALSE);
            set(my->ExpertLevel, MUIA_ShowMe, TRUE);

            // Take minimum user level into account.
            set(my->ExpertLevel, MUIA_Radio_Active, *((int *) msg->Level) - 1);
        }

        // Set the current user level.
        set(my->UserLevel, MUIA_Radio_Active, *((int *) msg->Level));

        // Wait for proceed or abort.
        if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
           == MUIV_InstallerGui_Proceed)
        {
            // Get the selected user level value. If we have
            // a minimum user level of 'average' we're using
            // a different set of radio buttons. Fetch from
            // the right object and adjust the return value.
            if(msg->MinLevel == 1)
            {
                // Minimum user level 'average'.
                get(my->ExpertLevel, MUIA_Radio_Active, (int *) msg->Level);
                (*((int *) msg->Level))++;
            }
            else
            {
                // Minimum user level 'novice' or 'expert'.
                get(my->UserLevel, MUIA_Radio_Active, (int *) msg->Level);
            }

            // Disable the pretend choice if the NOPRETEND
            // tooltype is used. The default behaviour is
            // to install for real, not to pretend.
            SetAttrs
            (
                my->Pretend,
                MUIA_Radio_Active,
                msg->NoPretend ? 0 : *((int *) msg->Pretend),
                MUIA_Disabled, msg->NoPretend ? TRUE : FALSE,
                TAG_END
            );

            // Disable the 'log to file' choice if the NOLOG
            // tooltype is used. The default behaviour is to
            // not write to a log file.
            SetAttrs
            (
                my->Log,
                MUIA_Radio_Active,
                msg->NoLog ? 0 : *((int *) msg->Log),
                MUIA_Disabled, msg->NoLog ? TRUE : FALSE,
                TAG_END
            );

            // Don't show logging and pretend mode settings
            // to 'Novice' users.
            if(*((int *) msg->Level))
            {
                // Show pretend / log page.
                if(DoMethod(obj, MUIM_InstallerGui_PageSet, NULL,
                            NULL, P_PRETEND_LOG, B_PROCEED_ABORT))
                {
                    // Wait for proceed or abort.
                    if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
                       == MUIV_InstallerGui_Proceed)
                    {
                        // Get pretend and log settings.
                        get(my->Pretend, MUIA_Radio_Active, (int *) msg->Pretend);
                        get(my->Log, MUIA_Radio_Active, (int *) msg->Log);
                    }
                    else
                    {
                        // Abort.
                        return 0;
                    }
                }
                else
                {
                    // Unknown error.
                    GERR(tr(S_UNER));
                    return 0;
                }
            }

            // Done.
            return 1;
        }
        else
        {
            // Abort.
            return 0;
        }
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return 0;
}

//----------------------------------------------------------------------------
// InstallerGuiAskFile - Show file / directory requester
// Input:                Message - The prompt
//                       Help - Help text
//                       NewPath - Allow non-existent defaults
//                       Disk - Show drive list first
//                       Assign - Logical assign OK as dir
//                       Default - Default file / directory
//                       Dir - Ask for directory
// Return:               File / directory name or NULL
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiAskFile(Class *cls,
                                Object *obj,
                                struct MUIP_InstallerGui_AskFile *msg)
{
    // Show file requester page.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_ASKFILE, B_PROCEED_ABORT))
    {
        // Create ASL file requester
        Object *str, *pop = (Object *) MUI_NewObject
        (
            MUIC_Popasl,
            MUIA_Popasl_Type, ASL_FileRequest,
            ASLFR_DrawersOnly, msg->Dir,
            ASLFR_InitialShowVolumes, msg->Disk,
            ASLFR_TitleText, msg->Dir ? tr(S_SDIR) : tr(S_SFLE),
            MUIA_Popstring_String, str = (Object *) MUI_MakeObject(MUIO_String, NULL, PATH_MAX),
            MUIA_Popstring_Button, (Object *) MUI_MakeObject(MUIO_PopButton, MUII_PopDrawer),
            TAG_END
        );

        // Open the requester after adding it
        // to the current group.
        if(pop)
        {
            struct InstallerGuiData *my = INST_DATA(cls, obj);

            // Set default file / dir.
            set(str, MUIA_String_Contents, msg->Default);

            // Prepare before adding requester.
            if(DoMethod(my->Ask, MUIM_Group_InitChange))
            {
                char *ret = NULL;

                // Add pop up requester.
                DoMethod(my->Ask, OM_ADDMEMBER, pop);

                // We're done adding things.
                DoMethod(my->Ask, MUIM_Group_ExitChange);

                // Wait for proceed or abort.
                if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
                   == MUIV_InstallerGui_Proceed)
                {
                    // Get filename from requester.
                    get(str, MUIA_String_Contents, &ret);

                    if(ret)
                    {
                        // We need to create a copy of the filename
                        // string since we're about to free the pop
                        // up requester.
                        int n = snprintf(my->Buf, sizeof(my->Buf), "%s", ret);

                        // Make sure that we succeded in creating a
                        // copy of the filename.
                        if(n >= 0 && ((size_t) n < sizeof(my->Buf)))
                        {
                            ret = my->Buf;
                        }
                    }

                    if(!ret)
                    {
                        // Unknown error.
                        GERR(tr(S_UNER));
                    }
                }

                // Prepare before removing requester.
                if(DoMethod(my->Ask, MUIM_Group_InitChange))
                {
                    // Remove pop up requester.
                    DoMethod(my->Ask, OM_REMMEMBER, pop);

                    // We're done removing things.
                    DoMethod(my->Ask, MUIM_Group_ExitChange);

                    // Free ASL requester.
                    MUI_DisposeObject(pop);

                    // Return filename.
                    return (IPTR) ret;
                }
            }

            // Free ASL requester.
            MUI_DisposeObject(pop);
        }
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return (IPTR) NULL;
}

//----------------------------------------------------------------------------
// InstallerGuiCopyFilesStart - Show file copy requester
// Input:                       Message - The prompt
//                              List - File / dir list
//                              Confirm - User confirmation
// Return:                      TRUE to start copy, FALSE to cancel
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCopyFilesStart(Class *cls,
                                       Object *obj,
                                       struct MUIP_InstallerGui_CopyFilesStart *msg)
{

    struct InstallerGuiData *my = INST_DATA(cls, obj);

    int n = 0;
    pnode_p cur = (pnode_p) msg->List,
            lst = cur;

    // For all files and directories to be copied; count
    // the files, and if confirmation is needed, add them
    // to the selection / deselection list.
    while(cur)
    {
        // Is this a file?
        if(cur->type == 1)
        {
            // Do we need confirmation?
            if(msg->Confirm)
            {
                // Add file to the selection / deselection list.
                DoMethod(Win, MUIM_InstallerGui_CopyFilesAdd, cur->name);
            }

            // Increase the total file count.
            n++;
        }

        // Next file / directory.
        cur = cur->next;
    }

    if(msg->Confirm && n)
    {
        // Show the file selection page.
        if(DoMethod(Win, MUIM_InstallerGui_PageSet, msg->Message,
                    msg->Help, P_FILEDEST, B_PROCEED_SKIP_ABORT))
        {
            ULONG b;
            LONG id = MUIV_List_NextSelected_Start;

            // Wait for confirmation / skip / abort.
            b = InstallerGuiWait(Win, MUIV_InstallerGui_ProceedRun, 3);

            // Did the user confirm?
            if(b == MUIV_InstallerGui_ProceedRun)
            {
                // For all files in the selection / deselection
                // list, tag the ones that we're going to copy.
                for(;;)
                {
                    // Get the first element in the list.
                    DoMethod(my->List, MUIM_List_NextSelected, &id);

                    // Are we done with all of the files?
                    if(id != MUIV_List_NextSelected_End)
                    {
                        // Get name of current file?
                        char *ent = NULL;
                        DoMethod(my->List, MUIM_List_GetEntry, id, &ent);

                        if(ent)
                        {
                            // Find the corresponding file node
                            // in 'our' list.
                            for(cur = lst; cur;
                                cur = cur->next)
                            {
                                // If we find it, and it's a file,
                                // give it a 'copy' tag.
                                if(cur->type == 1 &&
                                   !strcmp(ent, cur->name))
                                {
                                    cur->type = -1;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        // We're done with all files in the
                        // selection / deselection list.
                        break;
                    }
                }

                // Iterate over nodes in 'our' list and tag
                // everything that we're NOT going to copy.
                for(cur = lst; cur;
                    cur = cur->next)
                {
                    // Is this a file that's not going to be
                    // copied?
                    if(cur->type == 1)
                    {
                        // Give it an 'ignore' tag and decrease
                        // the number of files to be copied.
                        n--;
                        cur->type = 0;
                    }
                    // Is this a file that's going to be copied?
                    else if(cur->type == -1)
                    {
                        // Restore the file type so that the
                        // rest of the world understands that
                        // this is a file.
                        cur->type = 1;
                    }
                }
            }
            else
            {
                // Are we going to skip the file copy or are
                // we going to abort?
                return b == MUIV_InstallerGui_SkipRun ?
                       0 : -1;
            }
        }
    }

    // Show file copy page.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                NULL, P_COPYFILES, B_ABORT))
    {
        // Install a timer to create a time slice
        // where the user has a chance to abort.
        DoMethod(_app(obj), MUIM_Application_AddInputHandler, &my->Ticker);

        // Configure gauge so that one tick == one file.
        SetAttrs(my->Progress, MUIA_Gauge_Max, n,
                 MUIA_Gauge_Current, 0, TAG_END);

        // Always true.
        return (IPTR) TRUE;
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
        return FALSE;
    }
}

//----------------------------------------------------------------------------
// InstallerGuiCopyFilesSetCur - Show current file
// Input:                        File - filename
//                               NoGauge - Hide file gauge
// Return:                       TRUE to continue, FALSE to abort
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCopyFilesSetCur(Class *cls,
                                        Object *obj,
                                        struct MUIP_InstallerGui_CopyFilesSetCur *msg)
{
    if(msg->File)
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);
        char *file = (char *) msg->File;
        size_t len = strlen(file);
        struct TextExtent ext;

        // Get the number of characters that
        // can be shown given the width of the
        // gauge and the contents of the string.
        ULONG max = TextFit
        (
            _rp(my->Progress),
            file, len, &ext, NULL, 1,
            _mwidth(my->Progress),
            _mheight(my->Progress)
        );

        // Do we need to truncate the file
        // string to not wreck the gauge?
        if(len > max)
        {
            // The gauge might have room for more
            // than the temporary my->Buffer.
            len = max < sizeof(my->Buf) ? max : sizeof(my->Buf);

            // Truncate and switch.
            snprintf(my->Buf, len, "%s", file);
            file = my->Buf;
        }

        // Update progress bar. Text and number.
        if(!msg->NoGauge)
        {
            ULONG cur = 0;

            // Get current progress.
            get(my->Progress, MUIA_Gauge_Current, &cur);

            // Add another file on top of the current
            // progress (one file = one tick).
            SetAttrs(my->Progress, MUIA_Gauge_InfoText, file,
                     MUIA_Gauge_Current, ++cur, TAG_END);
        }
        else
        {
            // Update text only, ignore the gauge.
            set(my->Progress, MUIA_Gauge_InfoText, file);
        }
    }
    else
    {
        // Inv. count.
        static int n;

        // Give the user a chance to abort every
        // 64:th block.
        if(++n >> 6)
        {
            // Wait for the next tick (or abort).
            if(InstallerGuiWait(obj, MUIV_InstallerGui_Tick, 2) !=
               MUIV_InstallerGui_Tick)
            {
                // User abort.
                return (IPTR) FALSE;
            }

            // Start all over again.
            n = 0;
        }
    }

    // Next block.
    return (IPTR) TRUE;
}

//----------------------------------------------------------------------------
// InstallerGuiCopyFilesEnd - End of file copy
// Input:                     -
// Return:                    TRUE
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCopyFilesEnd(Class *cls,
                                     Object *obj)
{
    struct InstallerGuiData *my = INST_DATA(cls, obj);

    // Uninstall timer created to establish a time
    // slice where the user has a chance to abort
    // file copy operations. Things will continue
    // to work even if we don't, but by doing it
    // this way we make less noise.
    DoMethod
    (
        _app (obj),
        MUIM_Application_RemInputHandler, &my->Ticker
    );

    // Leave the file list the way we found it (empty).
    DoMethod(my->List, MUIM_List_Clear);

    // Always true.
    return (IPTR) TRUE;
}

//----------------------------------------------------------------------------
// InstallerGuiCopyFilesAdd - Add files to (expert) file (de)selector
// Input:                     File - Name of file (or dir)
// Return:                    TRUE
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCopyFilesAdd(Class *cls,
                                     Object *obj,
                                     struct MUIP_InstallerGui_CopyFilesAdd *msg)
{
    struct InstallerGuiData *my = INST_DATA(cls, obj);

    // Insert filename.
    DoMethod
    (
        my->List, MUIM_List_Insert,
        &(msg->File), 1,
        MUIV_List_Insert_Bottom
    );

    // The lister must be visible.
    set(my->List, MUIA_ShowMe, TRUE);

    // Always true.
    return (IPTR) TRUE;
}


//----------------------------------------------------------------------------
// InstallerGuiExit - Say goodbye and close GUI
// Input:             Message - The prompt
// Return:            TRUE
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiExit(Class *cls,
                             Object *obj,
                             struct MUIP_InstallerGui_Exit *msg)
{
    // Silence.
    (void) cls;
    (void) obj;
    (void) msg;

    // Do nothing.
    return TRUE;
}

//----------------------------------------------------------------------------
// InstallerGuiMessage - Show message
// Input:                Message - The prompt
//                       Immediate - No button
// Return:               TRUE on success, FALSE otherwise
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiMessage(Class *cls,
                                Object *obj,
                                struct MUIP_InstallerGui_Message *msg)
{
    // Silence.
    (void) cls;

    // Setup the correct page and button combination.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message, NULL,
                P_MESSAGE, msg->Immediate ? B_NONE : B_PROCEED))
    {
        // Wait for user input unless we're in immediate mode.
        if(!msg->Immediate)
        {
            // Wait for proceed or abort.
            if(InstallerGuiWait(obj, MUIV_InstallerGui_ProceedOnly, 1)
               != MUIV_InstallerGui_ProceedOnly)
            {
                // User abort.
                return FALSE;
            }
        }

        // Proceed.
        return TRUE;
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
        return FALSE;
    }
}

//----------------------------------------------------------------------------
// InstallerGuiAbort - Show message and abort
// Input:              Message - The message to be shown
// Return:             TRUE on success, FALSE otherwise
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiAbort(Class *cls,
                              Object *obj,
                              struct MUIP_InstallerGui_Abort *msg)
{
    // Silence.
    (void) cls;

    // Setup the correct page and button combination.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                NULL, P_MESSAGE, B_ABORT))
    {
        // Wait for abort.
        if(InstallerGuiWait(obj, MUIV_InstallerGui_AbortOnly, 1)
           == MUIV_InstallerGui_AbortOnly)
        {
            // User abort.
            return TRUE;
        }
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
    }

    // Escape or error.
    return FALSE;
}

//----------------------------------------------------------------------------
// InstallerGuiRadio - Show radio buttons
// Input:              Message - The prompt
//                     Help - Help text
//                     Names - Array of choices (strings)
//                     Default - Default selection
//                     Halt - Halt return value
// Return:             Zero index representing the selected button
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiRadio(Class *cls,
                              Object *obj,
                              struct MUIP_InstallerGui_Radio *msg)
{
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_MESSAGE, B_PROCEED_ABORT))
    {
        char **nms = (char **) msg->Names;

        if(nms && *nms)
        {
            struct InstallerGuiData *my = INST_DATA(cls, obj);
            int def = msg->Default;

            // Make sure that the default value is a
            // valid choice.
            while(def && *nms)
            {
                def--;
                nms++;
            }

            // Don't fail if it isn't, use a fallback
            // of 0 instead.
            def = !def ? msg->Default : 0;

            // Unlike most other pages, this one is
            // partly generated on the fly, we have
            // no choice.
            Object *r = (Object *) MUI_NewObject
            (
                MUIC_Radio,
                MUIA_Radio_Active, def, //i,
                MUIA_Radio_Entries, msg->Names, //c,
                TAG_END
            );

            if(r)
            {
                // Prepare before adding radio buttons.
                if(DoMethod(my->Empty, MUIM_Group_InitChange))
                {
                    // Add radio buttons.
                    DoMethod(my->Empty, OM_ADDMEMBER, r);

                    // We're done adding things.
                    DoMethod(my->Empty, MUIM_Group_ExitChange);

                    // Wait for proceed or abort.
                    if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
                       != MUIV_InstallerGui_Proceed)
                    {
                        // On abort return HALT.
                        *((int *) msg->Halt) = 1;
                    }

                    // Prepare before removing radio buttons.
                    if(DoMethod(my->Empty, MUIM_Group_InitChange))
                    {
                        ULONG ret;

                        // Remove radio buttons.
                        DoMethod(my->Empty, OM_REMMEMBER, r);

                        // We're done removing things.
                        DoMethod(my->Empty, MUIM_Group_ExitChange);

                        // Get value from buttons and then kill them.
                        // A halt above will not make any difference.
                        GetAttr(MUIA_Radio_Active, r, (IPTR *) &ret);
                        MUI_DisposeObject(r);

                        // Success.
                        return ret;
                    }
                }

                // The GUI is broken.
                MUI_DisposeObject(r);
            }
        }
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return 0;
}

//----------------------------------------------------------------------------
// InstallerGuiBool - Get boolean value from user
// Input:             Message - The prompt
//                    Help - Help text
//                    Yes - True string value
//                    No - False string value
// Return:            0 = no, 1 = yes
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiBool(Class *cls,
                             Object *obj,
                             struct MUIP_InstallerGui_Bool *msg)
{
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_MESSAGE, B_YES_NO))
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Set values of true and false.
        set(my->No, MUIA_Text_Contents, msg->No);
        set(my->Yes, MUIA_Text_Contents, msg->Yes);

        // Wait for yes, no or abort.
        switch(InstallerGuiWait(obj, MUIV_InstallerGui_Yes, 2))
        {
            case MUIV_InstallerGui_Yes:
                return 1;

            case MUIV_InstallerGui_No:
                return 0;
        }
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
    }

    // Abort or broken GUI.
    return -1;
}

//----------------------------------------------------------------------------
// InstallerGuiString - Get string value from user.
// Input:               Message - The prompt
//                      Help - Help text
//                      Default - Default string value
//                      Halt - Halt return value
// Return:              A string value
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiString(Class *cls,
                               Object *obj,
                               struct MUIP_InstallerGui_String *msg)
{
    // Result.
    ULONG r = 0;

    // Show string widget page.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_STRING, B_PROCEED_ABORT))
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Set initial value of string.
        set(my->String, MUIA_String_Contents, msg->Default);

        // Wait for proceed or abort.
        if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
           == MUIV_InstallerGui_Proceed)
        {
            // On proceed get string value.
            get(my->String, MUIA_String_Contents, &r);
        }
        else
        {
            // On abort return HALT.
            *((int *) msg->Halt) = 1;
        }
    }
    else
    {
        // Unknown error.
        GERR(tr(S_UNER));
    }

    // Always return a valid string.
    return r ? r : (ULONG) "";
}

//----------------------------------------------------------------------------
// InstallerGuiNumber - Get numerical value from user
// Input:               Message - The prompt
//                      Help - Help text
//                      Min - Minimum allowed value
//                      Max - Maximum allowed value
//                      Default - Default value
//                      Halt - Halt return value
// Return:              Numerical value
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiNumber(Class *cls,
                               Object *obj,
                               struct MUIP_InstallerGui_Number *msg)
{
    // Show slider.
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_NUMBER, B_PROCEED_ABORT))
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Set min, max and default value.
        set(my->Number, MUIA_Numeric_Min, msg->Min);
        set(my->Number, MUIA_Numeric_Max, msg->Max);
        set(my->Number, MUIA_Numeric_Value, msg->Default);

        // Wait for proceed or abort.
        if(InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2)
           == MUIV_InstallerGui_Proceed)
        {
            ULONG res = 0;

            // On proceed get and return numerical value.
            get(my->Number, MUIA_Numeric_Value, &res);
            return res;
        }
        else
        {
            // On abort return HALT.
            *((int *) msg->Halt) = 1;
            return 0;
        }
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return 0;
}

//----------------------------------------------------------------------------
// InstallerGuiCheckBoxes - Show list of radiobuttons
// Input:                   Message - The prompt
//                          Help - Help text
//                          Names - Array of choices (strings)
//                          Default - Default bitmask
//                          Halt - Halt return value
// Return:                  A bitmask representing the selected button(s)
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCheckBoxes(Class *cls,
                                   Object *obj,
                                   struct MUIP_InstallerGui_CheckBoxes *msg)
{
    if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                msg->Help, P_MESSAGE, B_PROCEED_ABORT))
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Unlike most other pages, this one is
        // partly generated on the fly, we have
        // no choice.
        if(DoMethod(my->Empty, MUIM_Group_InitChange))
        {
            ULONG id;
            size_t i = 0;
            static Object * cb[33];
            const char **cs = (const char **) msg->Names;

            // The maximum number of choices is 32.
            while(*cs && i < 32)
            {
                // Default selection.
                LONG sel = (msg->Default & (1u << i)) ? TRUE : FALSE;

                // New checkbox with default selection.
                Object *c = (Object *) MUI_NewObject
                (
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_ShowMe, *(*cs) ? TRUE : FALSE,
                    MUIA_InputMode, MUIV_InputMode_Toggle,
                    MUIA_Selected, *(*cs) ? sel : FALSE,
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Image,
                        MUIA_Frame, MUIV_Frame_ImageButton,
                        MUIA_Image_Spec, MUII_CheckMark,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_Image_FreeVert, TRUE,
                        MUIA_ShowSelState, FALSE,
                        MUIA_Selected, sel,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Text,
                        MUIA_Text_Contents, *cs,
                        TAG_END),
                    TAG_END
                );

                // On success, add it to the group and
                // save the adress.
                if(c)
                {
                    DoMethod(my->Empty, OM_ADDMEMBER, c);
                    cb[i++] = c;
                    cs++;
                }
                else
                {
                    // On error, free all the previous ones
                    // and bail out.
                    GERR(tr(S_UNER));

                    while(i--)
                    {
                        DoMethod(my->Empty, OM_REMMEMBER, cb[i]);
                        MUI_DisposeObject(cb[i]);
                    }

                    // We're done modifying the group.
                    DoMethod(my->Empty, MUIM_Group_ExitChange);
                    return 0;
                }
            }

            // We're done modifying the group.
            DoMethod(my->Empty, MUIM_Group_ExitChange);

            // Wait for proceed or abort.
            id = InstallerGuiWait(obj, MUIV_InstallerGui_Proceed, 2);

            // Remove all dynamic objects in group.
            if(DoMethod(my->Empty, MUIM_Group_InitChange))
            {
                // Bitmask.
                IPTR ret = 0;

                while(i--)
                {
                    ULONG sel = 0;

                    get(cb[i], MUIA_Selected, &sel);
                    ret |= (sel ? (1 << i) : 0);

                    DoMethod(my->Empty, OM_REMMEMBER, cb[i]);
                    MUI_DisposeObject(cb[i]);
                }

                DoMethod(my->Empty, MUIM_Group_ExitChange);

                if(id != MUIV_InstallerGui_Proceed)
                {
                    // On abort return HALT.
                    *((int *) msg->Halt) = 1;
                    ret = 0;
                }

                // Bitmap or zero on abort.
                return ret;
            }
        }
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return 0;
}

//----------------------------------------------------------------------------
// InstallerGuiComplete - Show progress
// Input:                 Progress - Installer progress in %
// Return:                On success TRUE, FALSE otherwise.
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiComplete(Class *cls,
                                 Object *obj,
                                 struct MUIP_InstallerGui_Complete *msg)
{
    struct InstallerGuiData *my = INST_DATA(cls, obj);
    int p = msg->Progress > 100 ? 100 : msg->Progress;

    // Set (caped) value and show the gauge.
    set(my->Complete, MUIA_Gauge_Current, p);
    set(my->Complete, MUIA_ShowMe, TRUE);

    // Always.
    return TRUE;
}

//----------------------------------------------------------------------------
// InstallerGuiConfirm - Get user confirmation before running shell command
// Input:                Message - The prompt
//                       Help - Help text
// Return:               '1' = proceed, '0' = skip, '-1' = abort
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiConfirm(Class *cls,
                                Object *obj,
                                struct MUIP_InstallerGui_Confirm *msg)
{
    struct InstallerGuiData *my = INST_DATA(cls, obj);
    ULONG top = 0, btm = 0, str = 0;

    // Save the current state of whatever we're
    // showing before we ask for confirmation.
    if(get(my->Top, MUIA_Group_ActivePage, &top) &&
       get(my->Bottom, MUIA_Group_ActivePage, &btm) &&
       get(my->Text, MUIA_Text_Contents, &str))
    {
        // Allocate memory to hold a copy of the
        // current message.
        size_t osz = strlen((char *) str) + 1;
        char *ost = calloc(osz, 1);

        if(ost)
        {
            // Copy the current message.
            memcpy(ost, (void *) str, osz);

            // Prompt for confirmation.
            if(DoMethod(obj, MUIM_InstallerGui_PageSet, msg->Message,
                        msg->Help, P_MESSAGE, B_PROCEED_SKIP_ABORT))
            {
                // Sleep until we get valid input.
                ULONG b = InstallerGuiWait(obj, MUIV_InstallerGui_ProceedRun, 3);

                // Restore everything so that things
                // look the way they did before the
                // confirmation dialog was shown.
                set(my->Top, MUIA_Group_ActivePage, top);
                set(my->Bottom, MUIA_Group_ActivePage, btm);
                set(my->Text, MUIA_Text_Contents, ost);

                // We no longer need the old message.
                free(ost);

                // Take care of the user input.
                switch(b)
                {
                    case MUIV_InstallerGui_ProceedRun:
                        return 1;

                    case MUIV_InstallerGui_SkipRun:
                        return 0;

                    case MUIV_InstallerGui_AbortRun:
                        return -1;
                }
            }

            // We never did show the new message so
            // we can get rid of the old (current).
            free(ost);
        }

        // OOM / broken GUI.
    }

    // Unknown error.
    GERR(tr(S_UNER));
    return -1;
}

//----------------------------------------------------------------------------
// InstallerGuiNew - Overloading OM_NEW
// Input:            See BOOPSI docs
// Return:           See BOOPSI docs
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiNew(Class *cls,
			                Object *obj,
			                struct opSet *msg)
{
    // Silence.
    (void) msg;

    // Temp widgets.
    Object *el, *ul, *fp, *cm, *pr,
           *st, *nm, *bp, *em, *rt,
           *tx, *ls, *lg, *tp, *af,
           *ys, *no;

    // Radio button strings.
    static const char *lev[4],
                      *pre[3],
                      *log[3];

    // Clear to enable check.
    el = ul = fp = cm = pr = st =
    nm = bp = em = rt = tx = ls =
    lg = tp = af = ys = no = NULL;

    // User level.
    lev[0] = tr(S_ULNV); // Novice
    lev[1] = tr(S_ULIN); // Intermediate
    lev[2] = tr(S_ULEX); // Expert

    // Pretend mode.
    pre[0] = tr(S_INRL); // For real
    pre[1] = tr(S_INDR); // Dry run

    // Logging mode.
    log[0] = tr(S_NOLG); // No logging
    log[1] = tr(S_SILG); // Log to file

    // The GUI is, as far as possible, a static
    // construct. We're not constructing things
    // on the fly, instead we use paging to let
    // widgets become visible / disappear.
    obj = (Object *) DoSuperNew
    (
        cls, obj,
        MUIA_Window_Title, tr(S_INST),
        MUIA_Window_AppWindow, TRUE,
        MUIA_Window_RootObject, MUI_NewObject(
            MUIC_Group,
            MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
            /* Top text */
            MUIA_Group_Child, tx = (Object *) MUI_NewObject(
                MUIC_Text,
                MUIA_Text_Contents, "\n\n\n\n\n\n\n\n\n\n",
                MUIA_Text_SetMax, FALSE,
                MUIA_Text_PreParse, "\33c",
                TAG_END),
            /* Top pager */
            MUIA_Group_Child, tp = MUI_NewObject(
                MUIC_Group,
                MUIA_Group_PageMode, TRUE,
                /* Page 0 - P_WELCOME */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Rectangle,
                        MUIA_Rectangle_HBar, TRUE,
                        MUIA_Rectangle_BarTitle, tr(S_INMD),
                        TAG_END),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Group,
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 32),
                        MUIA_Group_Child, ul = (Object *) MUI_NewObject(
                            MUIC_Radio,
                            MUIA_Radio_Entries, lev,
                            TAG_END),
                        MUIA_Group_Child, el = (Object *) MUI_NewObject(
                            MUIC_Radio,
                            MUIA_ShowMe, FALSE,
                            MUIA_Radio_Entries, lev + 1,
                            TAG_END),
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 32),
                        TAG_END),
                    TAG_END),
                /* Page 1 - P_COPYFILES */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Rectangle,
                        MUIA_Rectangle_HBar, TRUE,
                        MUIA_Rectangle_BarTitle, tr(S_CPYF),
                        TAG_END),
                    MUIA_Group_Child, fp = MUI_NewObject(
                        MUIC_Gauge,
                        MUIA_ShowMe, TRUE,
                        MUIA_Gauge_Horiz, TRUE,
                        MUIA_Gauge_InfoText, "-",
                        TAG_END),
                    TAG_END),
                /* Page 2 - P_FILEDEST */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Rectangle,
                        MUIA_Rectangle_HBar, TRUE,
                        MUIA_Rectangle_BarTitle, tr(S_F2IN),
                        TAG_END),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Listview,
                        MUIA_Listview_List, ls = MUI_NewObject(
                            MUIC_List,
                            MUIA_ShowMe, FALSE,
                            TAG_END),
                        MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Always,
                        TAG_END),
                    TAG_END),
                /* Page 3 - P_PRETEND_LOG */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_NewObject(
                        MUIC_Rectangle,
                        MUIA_Rectangle_HBar, TRUE,
                        MUIA_Rectangle_BarTitle, tr(S_IMAL),
                        TAG_END),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Group,
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                        MUIA_Group_Child, pr = (Object *) MUI_NewObject(
                            MUIC_Radio,
                            MUIA_Radio_Entries, pre,
                            TAG_END),
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                        MUIA_Group_Child, lg = (Object *) MUI_NewObject(
                            MUIC_Radio,
                            MUIA_Radio_Entries, log,
                            TAG_END),
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                        TAG_END),
                    TAG_END),
                /* Page 4 - P_MESSAGE */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    MUIA_Group_Child, em = (Object *) MUI_NewObject(
                        MUIC_Group,
                        MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    TAG_END),
                /* Page 5 - P_STRING */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                    MUIA_Group_Child, st = (Object *) MUI_NewObject(
                        MUIC_String,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_MaxLen, BUFSIZ,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                    TAG_END),
                /* Page 6 - P_NUMBER */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                    MUIA_Group_Child, nm = (Object *) MUI_NewObject(
                        MUIC_Slider,
                        MUIA_Slider_Horiz, TRUE,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
                    TAG_END),
                /* Page 7 - P_ASKFILE */
                MUIA_Group_Child, af = MUI_NewObject(
                    MUIC_Group,
                    TAG_END),
                TAG_END),
            /* Progress bar */
            MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_VSpace, 0),
            MUIA_Group_Child, cm = MUI_NewObject(
                MUIC_Gauge,
                MUIA_ShowMe, FALSE,
                MUIA_Gauge_Horiz, TRUE,
                MUIA_Gauge_InfoText, tr(S_PGRS),
                TAG_END),
            /* Bottom pager */
            MUIA_Group_Child, bp = MUI_NewObject(
                MUIC_Group,
                MUIA_Group_PageMode, TRUE,
                /* Page 0 - B_PROCEED_ABORT */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_Proceed,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_PRCD),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_Abort,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_ABRT),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    TAG_END),
                /* Page 1 - B_YES_NO */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, ys = (Object *) MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_Yes,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_AYES),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    MUIA_Group_Child, no = (Object *) MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_No,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_NONO),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    TAG_END),
                /* Page 2 - B_ABORT */
                MUIA_Group_Child, (Object *) MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_AbortOnly,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_ABRT),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    TAG_END),
                /* Page 3 - B_PROCEED_SKIP_ABORT */
                MUIA_Group_Child, (Object *) MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_ProceedRun,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_PRCD),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_SkipRun,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_SKIP),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_AbortRun,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_ABRT),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    TAG_END),
                /* Page 4 - B_PROCEED */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    MUIA_Group_Child, MUI_NewObject(
                        MUIC_Text,
                        MUIA_UserData, MUIV_InstallerGui_ProceedOnly,
                        MUIA_Frame, MUIV_Frame_Button,
                        MUIA_Text_Contents, tr(S_PRCD),
                        MUIA_Text_PreParse, "\33c",
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        TAG_END),
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    TAG_END),
                /* Page 5 - B_NONE */
                MUIA_Group_Child, MUI_NewObject(
                    MUIC_Group,
                    MUIA_Group_Horiz, TRUE,
                    MUIA_Group_Child, (Object *) MUI_MakeObject(MUIO_HSpace, 0),
                    TAG_END),
                TAG_END),
            TAG_END),
        TAG_END
    );

    // Initialize the rest if the parent is OK.
    if(obj)
    {
        struct InstallerGuiData *my = INST_DATA(cls, obj);

        // Initialize timer struct.
        my->Ticker.ihn_Object = obj;
        my->Ticker.ihn_Flags = MUIIHNF_TIMER;
        my->Ticker.ihn_Method = MUIM_InstallerGui_Ticker;
        my->Ticker.ihn_Millis = 10;

        // Save widgets.
        if(el && ul && fp && cm && pr &&
           st && nm && bp && em && tx &&
           ls && lg && tp && af && ys &&
           no)
        {
            my->ExpertLevel = el;
            my->UserLevel = ul;
            my->Progress = fp;
            my->Complete = cm;
            my->Pretend = pr;
            my->String = st;
            my->Number = nm;
            my->Bottom = bp;
            my->Empty = em;
            my->Text = tx;
            my->List = ls;
            my->Log = lg;
            my->Top = tp;
            my->Ask = af;
            my->Yes = ys;
            my->No = no;
        }
        else
        {
            // Forgot something.
            MUI_DisposeObject(obj);
            obj = NULL;
        }
    }

    // Done.
    return (IPTR) obj;
}

//----------------------------------------------------------------------------
// InstallerGuiSetup - Overloading MUIM_Setup
// Input:              See MUI docs
// Return:             See MUI docs
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiSetup(Class *cls,
                              Object *obj,
                              struct MUI_RenderInfo *msg)
{
    // Let our parent set itself up first
    if(!DoSuperMethodA (cls, obj, (Msg) msg))
    {
        GERR(tr(S_STFL));
        return FALSE;
    }
    else
    {
        // Nothing to do.
        return TRUE;
    }
}

//----------------------------------------------------------------------------
// InstallerGuiDispose - Overloading OM_DISPOSE
// Input:                See BOOPSI docs
// Return:               See BOOPSI docs
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiDispose (Class *cls,
                                 Object *obj,
                                 Msg msg)
{
    return (IPTR) DoSuperMethodA(cls, obj, msg);
}

//----------------------------------------------------------------------------
// InstallerGuiCleanup - Overloading MUIM_Cleanup
// Input:                See MUI docs
// Return:               See MUI docs
//----------------------------------------------------------------------------
MUIDSP IPTR InstallerGuiCleanup (Class *cls,
                                 Object *obj,
                                 Msg msg)
{
    return (IPTR) DoSuperMethodA(cls, obj, (Msg) msg);
}

//----------------------------------------------------------------------------
// InstallerGuiDispatch - MUI custom class dispatcher
// Input:                 pass through
// Return:                pass through
//----------------------------------------------------------------------------
DISPATCH(InstallerGui)
{
    DISPATCH_HEAD;

    // Dispatch according to MethodID
    switch (msg->MethodID)
    {
        case OM_NEW:
            return InstallerGuiNew (cls, obj, (struct opSet *) msg);

        case MUIM_Setup:
            return InstallerGuiSetup (cls, obj, (struct MUI_RenderInfo *) msg);

        case OM_DISPOSE:
            return InstallerGuiDispose (cls, obj, msg);

        case MUIM_Cleanup:
            return InstallerGuiCleanup (cls, obj, msg);

        case MUIM_InstallerGui_Init:
            return InstallerGuiInit(cls, obj);

        case MUIM_InstallerGui_Welcome:
            return InstallerGuiWelcome(cls, obj, (struct MUIP_InstallerGui_Welcome *) msg);

        case MUIM_InstallerGui_AskFile:
            return InstallerGuiAskFile(cls, obj, (struct MUIP_InstallerGui_AskFile *) msg);

        case MUIM_InstallerGui_PageSet:
            return InstallerGuiPageSet(cls, obj, (struct MUIP_InstallerGui_PageSet *) msg);

        case MUIM_InstallerGui_CopyFilesStart:
            return InstallerGuiCopyFilesStart(cls, obj, (struct MUIP_InstallerGui_CopyFilesStart *) msg);

        case MUIM_InstallerGui_CopyFilesSetCur:
            return InstallerGuiCopyFilesSetCur(cls, obj, (struct MUIP_InstallerGui_CopyFilesSetCur *) msg);

        case MUIM_InstallerGui_CopyFilesEnd:
            return InstallerGuiCopyFilesEnd(cls, obj);

        case MUIM_InstallerGui_CopyFilesAdd:
            return InstallerGuiCopyFilesAdd(cls, obj, (struct MUIP_InstallerGui_CopyFilesAdd *) msg);

        case MUIM_InstallerGui_Exit:
            return InstallerGuiExit(cls, obj, (struct MUIP_InstallerGui_Exit *) msg);

        case MUIM_InstallerGui_Message:
            return InstallerGuiMessage(cls, obj, (struct MUIP_InstallerGui_Message *) msg);

        case MUIM_InstallerGui_Abort:
            return InstallerGuiAbort(cls, obj, (struct MUIP_InstallerGui_Abort *) msg);

        case MUIM_InstallerGui_Radio:
            return InstallerGuiRadio(cls, obj, (struct MUIP_InstallerGui_Radio *) msg);

        case MUIM_InstallerGui_Bool:
            return InstallerGuiBool(cls, obj, (struct MUIP_InstallerGui_Bool *) msg);

        case MUIM_InstallerGui_String:
            return InstallerGuiString(cls, obj, (struct MUIP_InstallerGui_String *) msg);

        case MUIM_InstallerGui_Number:
            return InstallerGuiNumber(cls, obj, (struct MUIP_InstallerGui_Number *) msg);

        case MUIM_InstallerGui_CheckBoxes:
            return InstallerGuiCheckBoxes(cls, obj, (struct MUIP_InstallerGui_CheckBoxes *) msg);

        case MUIM_InstallerGui_Complete:
            return InstallerGuiComplete(cls, obj, (struct MUIP_InstallerGui_Complete *) msg);

        case MUIM_InstallerGui_Ticker:
            return DoMethod(_app(obj), MUIM_Application_ReturnID, MUIV_InstallerGui_Tick);

        case MUIM_InstallerGui_Confirm:
            return InstallerGuiConfirm(cls, obj, (struct MUIP_InstallerGui_Confirm *) msg);
    }

    // Unknown method, promote to parent.
    return DoSuperMethodA (cls, obj, msg);
}
#endif /* AMIGA */


//____________________________________________________________________________
//############################################################################
//############################################################################
//############################################################################
//****************************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//............................................................................
//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//.   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .  .
//
//
// The gui_* functions below serve as glue between the platform independent
// parts of InstallerNG and the Amiga specific Zune / MUI parts. On non Amiga
// systems, arguments are written to stdout to facilitate testing.
//
//
//.   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .  .
//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//****************************************************************************
//############################################################################
//############################################################################
//############################################################################


//----------------------------------------------------------------------------
// Name:        gui_init
// Description: Initialize and show GUI.
// Input:       -
// Return:      int:    TRUE on success, FALSE otherwise.
//----------------------------------------------------------------------------
int gui_init(void)
{
    // Amiga style version string.
    static char version[] __attribute__((used)) = VERSION_STRING;

    #ifdef AMIGA
    Object *App;

    // Create our GUI class.
    InstallerGuiClass = (struct MUI_CustomClass *) MUI_CreateCustomClass
    (
	    NULL, MUIC_Window, NULL,
        sizeof (struct CLASS_DATA(InstallerGui)),
        (APTR) DISPATCH_GATE (InstallerGui)
    );

    // Bail out on error.
    if(!InstallerGuiClass)
    {
        GERR(tr(S_FMCC));
        return FALSE;
    }

    // Create application using our newly created class.
    App = (Object *) MUI_NewObject(
        MUIC_Application,
        MUIA_Application_Title, tr(S_INST),
        MUIA_Application_Version, version + 1,
        MUIA_Application_Description, "App installation utility",
        MUIA_Application_HelpFile, "Installer.guide",
        MUIA_Application_Window, Win = (Object *) NewObject(
            InstallerGuiClass->mcc_Class, NULL,
            TAG_END),
        TAG_END
    );

    // Bail out on error.
    if(!App)
    {
        GERR(tr(S_MAPP));
        return FALSE;
    }

    // Internal initialization.
    if(!DoMethod(Win, MUIM_InstallerGui_Init))
    {
        // Bail out on error.
        GERR(tr(S_FINT));
        gui_exit();

        return FALSE;
    }

    // Open the window to finish setup.
    set(Win, MUIA_Window_Open, TRUE);
    DoMethod(Win, MUIM_Show);

    return TRUE;
    #else
    // Testing purposes.
    return strlen(version + 1) ? 1 : 0;
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_exit
// Description: Close GUI and free GUI resources.
// Input:       -
// Return:      -
//----------------------------------------------------------------------------
void gui_exit(void)
{
    #ifdef AMIGA
    // Close window and destroy app
    set(Win, MUIA_Window_Open, FALSE);
    MUI_DisposeObject(_app(Win));

    // Destroy custom class. Must check for NULL.
    if(InstallerGuiClass)
    {
        MUI_DeleteCustomClass(InstallerGuiClass);
    }
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_message
// Description: Show message.
// Input:       const char *msg:    Message shown to the user.
//              int imm:            No proceed button.
// Return:      int:                1 on proceed, 0 on abort.
//----------------------------------------------------------------------------
int gui_message(const char *msg, int imm)
{
    #ifdef AMIGA
    return (int) DoMethod
    (
        Win, MUIM_InstallerGui_Message,
        msg, imm
    );
    #else
    // Testing purposes.
    if(imm)
    {
        printf("%d:%s", imm, msg);
    }
    else
    {
        fputs(msg, stdout);
    }

    return 1;
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_abort
// Description: Show message and abort.
// Input:       const char *msg:    Message shown to the user.
// Return:      -
//----------------------------------------------------------------------------
void gui_abort(const char *msg)
{
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_Abort,
        msg
    );
    #else
    // Testing purposes.
    fputs(msg, stdout);
    #endif
}


//----------------------------------------------------------------------------
// Name:        gui_choice
// Description: Get user selecton of a single string out of a list of strings.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              const char **nms:   List of strings.
//              int def:            Default choice (0-index)
//              int *hlt:           Halt return value.
// Return:      int:                The choice (0-index).
//----------------------------------------------------------------------------
int gui_choice(const char *msg,
               const char *hlp,
               const char **nms,
               int def,
               int *hlt)
{
    int ret = (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_Radio,
        msg, hlp, nms, def, hlt
    );
    #else
    // Testing purposes.
    printf("%s%s%s%d\n", msg, hlp, *nms, def) ? def : 0;
    // Don't halt on non Amiga systems.
    *hlt = 0;
    #endif

    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_options
// Description: Get user selection of any number / combination of strings
//              from a list.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              const char **nms:   List of strings from which to choose.
//              int def:            Default bitmap.
//              int *hlt:           Halt return value.
// Return:      int:                A bitmap representing the selection.
//----------------------------------------------------------------------------
int gui_options(const char *msg,
                const char *hlp,
                const char **nms,
                int def,
                int *hlt)
{
    int ret = (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_CheckBoxes,
        msg, hlp, nms, def, hlt
    );
    #else
    // Testing purposes.
    printf("%s%s%s%d%d\n", msg, hlp, *nms, def, *hlt) ? def : 0;
    #endif

    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_bool
// Description: Get boolean value from user, e.g 'Yes' / 'No'.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              const char *yes:    True string.
//              const char *no:     False string.
// Return:      int:                '1' or '0'.
//----------------------------------------------------------------------------
int gui_bool(const char *msg,
             const char *hlp,
             const char *yes,
             const char *no)
{
    int ret = (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_Bool,
        msg, hlp, yes, no
    );
    #else
    // Testing purposes.
    printf("%s%s%s%s\n", msg, hlp, yes, no) ? 1 : 0;
    #endif
    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_string
// Description: Get string value from user.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              const char *def:    Default string value.
//              int *hlt:           Halt return value.
// Return:      const char *:       String value from user.
//----------------------------------------------------------------------------
const char *gui_string(const char *msg,
                       const char *hlp,
                       const char *def,
                       int *hlt)
{
    const char *ret = (const char *)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_String,
        msg, hlp, def, hlt
    );
    #else
    // Testing purposes.
    (printf("%s%s%s\n", msg, hlp, def) ? def : "");

    // Don't halt on non Amiga systems.
    *hlt = 0;
    #endif
    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_number
// Description: Get numerical value from user.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              int min:            Minimum value.
//              int max:            Maximum value.
//              int def:            Default value.
//              int *hlt:           Halt return value.
// Return:      int:                Numerical value from user.
//----------------------------------------------------------------------------
int gui_number(const char *msg,
               const char *hlp,
               int min,
               int max,
               int def,
               int *hlt)
{
    int ret = (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_Number,
        msg, hlp, min, max, def, hlt
    );
    #else
    // Testing purposes.
    (printf("%s%s%d%d\n", msg, hlp, min, max) ? def : 0);

    // Don't halt on non Amiga systems.
    *hlt = 0;
    #endif
    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_welcome
// Description: Show welcome message and prompt for user level / installer
//              mode. Note that the 'lvl', 'lgf', and 'prt' parameters act
//              as both input and output.
// Input:       const char *msg:    Welcome message.
//              int *lvl:           User level return and input value.
//              int *lgf:           Log settings return and input value.
//              int *prt:           Pretend mode return and input value.
//              int min:            Minimum user level.
//              int npr:            Disable pretend mode.
//              int nlg:            Disable logging.
// Return:      int:                1 on proceed, 0 on abort.
//----------------------------------------------------------------------------
int gui_welcome(const char *msg,
                int *lvl,
                int *lgf,
                int *prt,
                int min,
                int npr,
                int nlg)
{
    int ret = (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_Welcome,
        msg, lvl, lgf, prt, min, npr, nlg
    );
    #else
    // Testing purposes.
    printf
    (
        "%s%d%d%d%d%d%d\n",
        msg, *lvl, *lgf, *prt,
        min, npr, nlg
    );
    #endif

    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_askdir
// Description: Get directory name from user.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              int pth:            Allow non-existing default.
//              int dsk:            Show drive list first.
//              int asn:            Assigns can satisfy the request as well.
//              const char *def:    Default value.
// Return:      const char*:        Dir name on success, NULL otherwise.
//----------------------------------------------------------------------------
const char *gui_askdir(const char *msg,
                       const char *hlp,
                       int pth,
                       int dsk,
                       int asn,
                       const char *def)
{
    const char *ret = (const char *)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_AskFile,
        msg, hlp, pth, dsk, asn, def, TRUE
    );
    #else
    // Testing purposes.
    (printf("%s%s%d%d%d%s\n", msg, hlp, pth, dsk, asn, def) ? def : "");
    #endif

    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_askfile
// Description: Get filename from user.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
//              int pth:            Allow non-existing default.
//              int dsk:            Show drive list first.
//              const char *def:    Default value.
// Return:      const char*:        Filename on success, NULL otherwise.
//----------------------------------------------------------------------------
const char *gui_askfile(const char *msg,
                        const char *hlp,
                        int pth,
                        int dsk,
                        const char *def)
{
    const char *ret = (const char *)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_AskFile,
        msg, hlp, pth, dsk, FALSE, def, FALSE
    );
    #else
    // Testing purposes.
    (printf("%s%s%d%d\n", msg, hlp, pth, dsk) ? def : "");
    #endif

    return ret;
}

//----------------------------------------------------------------------------
// Name:        gui_copyfiles_start
// Description: Show file copy confirmation requester with a populated file
//              list.
// Input:       const char *msg:    Message to be shown.
//              const char *hlp:    Help text.
//              pnode_p lst:        List of files / directories.
//              int cnf:            Confirmation.
// Return:      int:                '1' - proceed, '0' - skip, '-1' abort.
//----------------------------------------------------------------------------
int gui_copyfiles_start(const char *msg, const char *hlp, pnode_p lst, int cnf)
{
    return (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_CopyFilesStart,
        msg, hlp, lst, cnf
    );
    #else
    // Testing purposes.
    (lst ? (cnf ? ((fputs(msg, stdout) != EOF &&
                    fputs(hlp, stdout) != EOF) ? 0 : -1) : 1) : -1);
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_copyfiles_setcur
// Description: Update progress gauge and show current filename.
// Input:       const char *cur:    Filename.
//              int nogauge:        Hide gauge.
// Return:      int:                TRUE on success, FALSE otherwise.
//----------------------------------------------------------------------------
int gui_copyfiles_setcur(const char *cur, int nogauge)
{
    return (int)
    #ifdef AMIGA
    DoMethod
    (
        Win, MUIM_InstallerGui_CopyFilesSetCur,
        cur, nogauge
    );
    #else
    // Testing purposes.
    (!cur ? 1 + nogauge : 3);
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_copyfiles_end
// Description: End file copy. Must be invoked after gui_copyfiles_start().
// Input:       -
// Return:      -
//----------------------------------------------------------------------------
void gui_copyfiles_end(void)
{
    #ifdef AMIGA
    DoMethod(Win, MUIM_InstallerGui_CopyFilesEnd);
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_complete
// Description: Show progress gauge.
// Input:       int com:            Progress in percent.
// Return:      int:                TRUE on success, FALSE otherwise.
//----------------------------------------------------------------------------
int gui_complete(int com)
{
    return (int)
    #ifdef AMIGA
    DoMethod(Win, MUIM_InstallerGui_Complete, com);
    #else
    // Testing purposes.
    com;
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_confirm
// Description: Get user confirmation.
// Input:       const char *msg:    Message shown to the user.
//              const char *hlp:    Help text.
// Return:      int:                '1' = proceed, '0' = skip, '-1' = abort
//----------------------------------------------------------------------------
int gui_confirm(const char *msg, const char *hlp)
{
    return (int)
    #ifdef AMIGA
    DoMethod(Win, MUIM_InstallerGui_Confirm, msg, hlp);
    #else
    // Testing purposes.
    printf("%s%s\n", msg, hlp) ? 1 : 0;
    #endif
}

//----------------------------------------------------------------------------
// Name:        gui_error
// Description: Show error message.
// Input:       int id:             Line number.
//              const char *type:   Error description.
//              const char *info:   Extra info, e.g. filename.
// Return:      int:                1
//----------------------------------------------------------------------------
int gui_error(int id,
              const char *type,
              const char *info)
{
    #ifdef AMIGA
    static char err[BUFSIZ];
    static struct EasyStruct es =
    {
        .es_TextFormat = (UBYTE *) &err,
        .es_StructSize = sizeof(struct EasyStruct),
    };

    es.es_Title = (UBYTE *) tr(S_ERRS);
    es.es_GadgetFormat = (UBYTE *) tr(S_OKEY);
    snprintf(err, sizeof(err), tr(S_LERR), id, type, info);

    // We don't have any way of knowing
    // whether this really works out...
    EasyRequest(NULL, &es, NULL);

    // Ignore the user feedback.
    return 1;
    #else
    // Testing purposes.
    return (id && type && info) ? 0 : 1;
    #endif
}
