/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <aros/debug.h>
#include <exec/types.h>
#include <utility/tagitem.h>
#include <libraries/mui.h>
#include <zune/customclasses.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include "asl.h"
#include "locale.h"
#include "support.h"
#include "toolbar.h"
#include "emulator.h"

/*** Macros *****************************************************************/
#define StopButton()       MakeToggleButton("\eI[1:20]", _(MSG_TB_STOP_SH))
#define PauseButton()      MakeToggleButton("\eI[1:19]", _(MSG_TB_PAUSE_SH))
#define StartButton()      MakeToggleButton("\eI[1:17]", _(MSG_TB_START_SH))
#define RestartButton()    MakePushButton("\eI[1:21]", _(MSG_TB_RESTART_SH))
#define LoadStateButton()  MakePushButton("\eI[1:10]", _(MSG_TB_LOAD_STATE_SH))
#define SaveStateButton()  MakePushButton("\eI[1:12]", _(MSG_TB_SAVE_STATE_SH))
#define FullscreenButton() MakePushButton("\eI[1:18] \eI[1:17]", _(MSG_TB_FULLSCREEN_SH))

#define ToolbarSpace()     HSpace(5)

/*** Private methods ********************************************************/
#define MUIM_Toolbar_ChangeMode (TAG_USER | 0x10000000)
#define MUIM_Toolbar_LoadState  (TAG_USER | 0x10000001)
#define MUIM_Toolbar_SaveState  (TAG_USER | 0x10000002)

struct  MUIP_Toolbar_ChangeMode {ULONG MethodID; ULONG State;};

/*** Instance data **********************************************************/
struct Toolbar_DATA
{
    Object *td_StopButton,
           *td_PauseButton,
           *td_StartButton,
           *td_RestartButton,
           *td_SaveStateButton,
           *td_LoadStateButton,
           *td_FullscreenButton;
};

/*** Methods ****************************************************************/
IPTR Toolbar__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct Toolbar_DATA *data = NULL;
    Object              *stopButton,
                        *pauseButton,
                        *startButton,
                        *restartButton,
                        *saveStateButton,
                        *loadStateButton,
                        *fullscreenButton;

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        ButtonFrame,
        MUIA_Group_Horiz, TRUE,
        MUIA_Background,  MUII_GroupBack,
                    
        Child, (IPTR) HGroup,
            GroupSpacing(2),
            MUIA_Weight,         0,
            MUIA_Group_SameSize, TRUE,
            
            Child, (IPTR) (stopButton  = StopButton()),
            Child, (IPTR) (pauseButton = PauseButton()),
            Child, (IPTR) (startButton = StartButton()),
            Child, (IPTR) ToolbarSpace(),
            Child, (IPTR) (restartButton = RestartButton()),
            Child, (IPTR) ToolbarSpace(),
            Child, (IPTR) (saveStateButton = SaveStateButton()),
            Child, (IPTR) (loadStateButton = LoadStateButton()),
            Child, (IPTR) ToolbarSpace(),
            Child, (IPTR) (fullscreenButton = FullscreenButton()),
        End,
        Child, (IPTR) HVSpace,

        TAG_DONE
    );
    
    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        data->td_StopButton       = stopButton;
        data->td_PauseButton      = pauseButton;
        data->td_StartButton      = startButton;
        data->td_RestartButton    = restartButton;
        data->td_SaveStateButton  = saveStateButton;
        data->td_LoadStateButton  = loadStateButton;
        data->td_FullscreenButton = fullscreenButton;        
    }
    
    return (IPTR) self;
}

IPTR Toolbar__MUIM_Setup
(
    Class *CLASS, Object *self, struct MUI_RenderInfo *message
)
{
    struct Toolbar_DATA *data = INST_DATA(CLASS, self);

    if (!DoSuperMethodA(CLASS, self, (Msg) message)) return FALSE;

    DoMethod(self, MUIM_Toolbar_ChangeMode, XGET(_app(self), MUIA_Emulator_State));

    DoMethod
    (
        data->td_StopButton, MUIM_Notify, MUIA_Selected, TRUE,
        (IPTR) _app(self), 3, MUIM_Set, MUIA_Emulator_State, MUIV_Emulator_State_Stopped
    );
    DoMethod
    (
        data->td_PauseButton, MUIM_Notify, MUIA_Selected, TRUE,
        (IPTR) _app(self), 3, MUIM_Set, MUIA_Emulator_State, MUIV_Emulator_State_Paused
    );
    DoMethod
    (
        data->td_StartButton, MUIM_Notify, MUIA_Selected, TRUE,
        (IPTR) _app(self), 3, MUIM_Set, MUIA_Emulator_State, MUIV_Emulator_State_Running
    );

    DoMethod
    (
        _app(self), MUIM_Notify, MUIA_Emulator_State, MUIV_EveryTime,
        (IPTR) self, 2, MUIM_Toolbar_ChangeMode, MUIV_TriggerValue
    );

    DoMethod
    (
        data->td_RestartButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) _app(self), 1, MUIM_Emulator_Restart
    );
    
    DoMethod
    (
        data->td_LoadStateButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) self, 1, MUIM_Toolbar_LoadState
    );
    DoMethod
    (
        data->td_SaveStateButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) self, 1, MUIM_Toolbar_SaveState
    );
    
    DoMethod
    (
        data->td_FullscreenButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) _app(self), 3, MUIM_Set, MUIA_Emulator_Fullscreen, TRUE
    );
    
    return TRUE;
}

IPTR Toolbar__MUIM_Toolbar_ChangeMode
(
    Class *CLASS, Object *self, struct MUIP_Toolbar_ChangeMode *message
)
{
    struct Toolbar_DATA *data = INST_DATA(CLASS, self);

    switch (message->State)
    {
        case MUIV_Emulator_State_Stopped:
            NNSET(data->td_StopButton, MUIA_Selected, TRUE);
            SET(data->td_PauseButton, MUIA_Selected, FALSE);
            SET(data->td_StartButton, MUIA_Selected, FALSE);

            SET(data->td_StopButton, MUIA_Disabled, TRUE);
            SET(data->td_PauseButton, MUIA_Disabled, TRUE);
            SET(data->td_StartButton, MUIA_Disabled, FALSE);
            break;

        case MUIV_Emulator_State_Paused:
            SET(data->td_StopButton, MUIA_Selected, FALSE);
            NNSET(data->td_PauseButton, MUIA_Selected, TRUE);
            SET(data->td_StartButton, MUIA_Selected, FALSE);

            SET(data->td_StopButton, MUIA_Disabled, FALSE);
            SET(data->td_PauseButton, MUIA_Disabled, TRUE);
            SET(data->td_StartButton, MUIA_Disabled, FALSE);
            break;
        
        case MUIV_Emulator_State_Running:
            SET(data->td_StopButton, MUIA_Selected, FALSE);
            SET(data->td_PauseButton, MUIA_Selected, FALSE);
            NNSET(data->td_StartButton, MUIA_Selected, TRUE);
            
            SET(data->td_StopButton, MUIA_Disabled, FALSE);
            SET(data->td_PauseButton, MUIA_Disabled, FALSE);
            SET(data->td_StartButton, MUIA_Disabled, TRUE);
            break;
    }
    
    DoMethod
    (
        self, MUIM_MultiSet, MUIA_Disabled,
        message->State == MUIV_Emulator_State_Running ? FALSE : TRUE,
        (IPTR) data->td_RestartButton,
        (IPTR) data->td_SaveStateButton,
        (IPTR) data->td_FullscreenButton,
        NULL
    );
    
    return NULL;
}

IPTR Toolbar__MUIM_Toolbar_LoadState(Class *CLASS, Object *self, Msg message)
{
    STRPTR filename = ASL_SelectFile(AM_LOAD);
    if (filename != NULL)
    {
        DoMethod(_app(self), MUIM_Emulator_LoadState, (IPTR) filename);
        FreeVec(filename);
    }
    
    return NULL;
}

IPTR Toolbar__MUIM_Toolbar_SaveState(Class *CLASS, Object *self, Msg message)
{
    STRPTR filename = ASL_SelectFile(AM_SAVE);
    if (filename != NULL)
    {
        DoMethod(_app(self), MUIM_Emulator_SaveState, (IPTR) filename);
        FreeVec(filename);
    }
    
    return NULL;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_5
(
    Toolbar, NULL, MUIC_Group, NULL,
    OM_NEW,                  struct opSet *,
    MUIM_Setup,              struct MUI_RenderInfo *,
    MUIM_Toolbar_ChangeMode, struct MUIP_Toolbar_ChangeMode *,
    MUIM_Toolbar_LoadState,  Msg,
    MUIM_Toolbar_SaveState,  Msg
);
