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

#include "locale.h"
#include "drive.h"
#include "drivelisteditor.h"
#include "drivelist.h"
#include "driveeditor.h"

/*** Private methods ********************************************************/
#define MUIM_DriveListEditor_UpdateButtons (TAG_USER | 0x11000000)
#define MUIM_DriveListEditor_Add           (TAG_USER | 0x11000001)
#define MUIM_DriveListEditor_Edit          (TAG_USER | 0x11000002)
#define MUIM_DriveListEditor_Remove        (TAG_USER | 0x11000003)
#define MUIM_DriveListEditor_MoveUp        (TAG_USER | 0x11000004)
#define MUIM_DriveListEditor_MoveDown      (TAG_USER | 0x11000005)

/*** Instance data **********************************************************/
struct DriveListEditor_DATA
{
    Object *dle_List,
           *dle_AddButton,
           *dle_EditButton,
           *dle_RemoveButton,
           *dle_UpButton,
           *dle_DownButton;
};

/*** Methods ****************************************************************/
IPTR DriveListEditor__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct DriveListEditor_DATA *data = NULL;
    Object *list, *addButton, *editButton, *removeButton, *upButton, *downButton;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        MUIA_Group_Horiz, TRUE,
                        
        Child, (IPTR) ListviewObject,
            MUIA_Weight,               50,
            MUIA_Listview_List, (IPTR) list = DriveListObject,
                InputListFrame,
            End,
        End,
        Child, (IPTR) VGroup,
            MUIA_Weight, 0,
            
            Child, (IPTR) VGroup,
                MUIA_Weight,         0,
                MUIA_Group_SameSize, TRUE,
                
                Child, (IPTR) (addButton    = SimpleButton(_(MSG_CFG_DRV_DLE_ADD))),
                Child, (IPTR) (editButton   = SimpleButton(_(MSG_CFG_DRV_DLE_EDIT))),
                Child, (IPTR) (removeButton = SimpleButton(_(MSG_CFG_DRV_DLE_REMOVE))),
                Child, (IPTR) HVSpace,
                Child, (IPTR) (upButton     = SimpleButton("\eI[1:0]")),
                Child, (IPTR) (downButton   = SimpleButton("\eI[1:1]")),
            End,
            Child, (IPTR) HVSpace,
        End,
        
        TAG_DONE
    );
    
    if (self != NULL)
    {
        /*-- Store important variables -------------------------------------*/
        data = INST_DATA(CLASS, self);
        data->dle_List = list;
        data->dle_AddButton    = addButton;
        data->dle_EditButton   = editButton;
        data->dle_RemoveButton = removeButton;
        data->dle_UpButton     = upButton;
        data->dle_DownButton   = downButton;
        
        /*-- Setup notifications -------------------------------------------*/
        DoMethod
        (
            list, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_DriveListEditor_UpdateButtons
        );
        
        DoMethod
        (
            addButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_DriveListEditor_Add
        );
        DoMethod
        (
            editButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_DriveListEditor_Edit
        );
        DoMethod
        (
            removeButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_DriveListEditor_Remove
        );
        DoMethod
        (
            upButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_DriveListEditor_MoveUp
        );
        DoMethod
        (
            downButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_DriveListEditor_MoveDown
        );
        
        /*-- Miscellanous initialization -----------------------------------*/
        SET(editButton, MUIA_Disabled, TRUE);
        SET(removeButton, MUIA_Disabled, TRUE);
        SET(upButton, MUIA_Disabled, TRUE);
        SET(downButton, MUIA_Disabled, TRUE);
    }

    return (IPTR) self;
}

IPTR DriveListEditor__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct DriveListEditor_DATA *data  = INST_DATA(CLASS, self);
    struct Drive                *drive = NULL;
    ULONG                        i;
    
    for (i = 0; i < XGET(data->dle_List, MUIA_List_Entries); i++)
    {
        DoMethod(data->dle_List, MUIM_List_Remove, i);
        DoMethod(data->dle_List, MUIM_List_GetEntry, i, (IPTR) &drive);
        
        if (drive != NULL) Drive_Destroy(drive);
    }
    
    return DoSuperMethodA(CLASS, self, message);
}

IPTR DriveListEditor__MUIM_DriveListEditor_UpdateButtons
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data = INST_DATA(CLASS, self);
    ULONG active                      = XGET(data->dle_List, MUIA_List_Active);
    
    if (active == MUIV_List_Active_Off)
    {
        SET(data->dle_EditButton, MUIA_Disabled, TRUE);
        SET(data->dle_RemoveButton, MUIA_Disabled, TRUE);
        SET(data->dle_UpButton, MUIA_Disabled, TRUE);
        SET(data->dle_DownButton, MUIA_Disabled, TRUE);
    }
    else
    {
        ULONG entries = XGET(data->dle_List, MUIA_List_Entries);
    
        SET(data->dle_EditButton, MUIA_Disabled, FALSE);
        SET(data->dle_RemoveButton, MUIA_Disabled, FALSE);
        SET(data->dle_UpButton, MUIA_Disabled, active == 0 ? TRUE : FALSE);
        SET(data->dle_DownButton, MUIA_Disabled, active == entries - 1 ? TRUE : FALSE);
    }
    
    return 0;
}

ULONG EditDrive(Object *self, struct Drive *drive)
{
    Object *window = NULL;
    ULONG   rv     = 0L;
    
    if (drive == NULL) return 0;
    
    window = DriveEditorObject,
        MUIA_DriveEditor_Drive, (IPTR) drive,
    End;

    if (window != NULL)
    {
        BOOL  running = TRUE;
        ULONG signals = 0L;
        
        SET(_app(self), MUIA_Application_Sleep, TRUE);
        DoMethod(_app(self), OM_ADDMEMBER, (IPTR) window);
        SET(window, MUIA_Window_Open, TRUE);
        
        while (running)
        {
            switch
            (
                rv = DoMethod
                (
                    _app(self), MUIM_Application_NewInput, (IPTR) &signals
                )
            )
            {
                case MUIV_DriveEditor_ReturnID_Cancel:
                case MUIV_DriveEditor_ReturnID_OK:
                case MUIV_Application_ReturnID_Quit:
                    running = FALSE;
                    break;
            }
            
            if (signals != 0)
            {
                signals = Wait(signals | SIGBREAKF_CTRL_C);
                if (signals & SIGBREAKF_CTRL_C)
                {
                    running = FALSE;
                    rv      = MUIV_Application_ReturnID_Quit;
                }
            }
        }
        
        SET(window, MUIA_Window_Open, FALSE);
        DoMethod(_app(self), OM_REMMEMBER, (IPTR) window);
        SET(_app(self), MUIA_Application_Sleep, FALSE);
        MUI_DisposeObject(window);
        
        if (rv == MUIV_Application_ReturnID_Quit)
        {
            /* Forward the return ID to the parent NewInput loop.  */
            DoMethod
            (
                _app(self), MUIM_Application_ReturnID,
                MUIV_Application_ReturnID_Quit
            );
            
            rv = 0;
        }
    }
    
    return rv;
}

IPTR DriveListEditor__MUIM_DriveListEditor_Add
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data  = INST_DATA(CLASS, self);
    struct Drive                *drive = Drive_Create();
    
    if (drive != NULL)
    {
        if (EditDrive(self, drive) == MUIV_DriveEditor_ReturnID_OK)
        {
            ULONG position = MUIV_List_Insert_Active;
            
            if(XGET(data->dle_List, MUIA_List_Active) == MUIV_List_Active_Off)
            {
                position = MUIV_List_Insert_Bottom;
            }
            
            DoMethod
            (
                data->dle_List, MUIM_List_InsertSingle, 
                (IPTR) drive, position
            );
        }
        else
        {
            Drive_Destroy(drive);
        }
    }
    else
    {
        // FIXME: error message
    }
    
    return 0;
}

IPTR DriveListEditor__MUIM_DriveListEditor_Edit
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data   = INST_DATA(CLASS, self);
    struct Drive                *drive  = NULL;
    
    DoMethod
    (
        data->dle_List, MUIM_List_GetEntry, 
        MUIV_List_GetEntry_Active, (IPTR) &drive
    );
    
    if (drive != NULL)
    {
        if (EditDrive(self, drive) == MUIV_DriveEditor_ReturnID_OK)
        {
            DoMethod
            (
                data->dle_List, MUIM_List_Redraw, MUIV_List_Redraw_Active
            );
        }
    }
    
    return 0;
}

IPTR DriveListEditor__MUIM_DriveListEditor_Remove
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data   = INST_DATA(CLASS, self);
    struct Drive                *drive  = NULL;
    
    DoMethod
    (
        data->dle_List, MUIM_List_GetEntry, 
        MUIV_List_GetEntry_Active, (IPTR) &drive
    );
    
    if (drive != NULL)
    {   
        DoMethod(data->dle_List, MUIM_List_Remove, MUIV_List_Remove_Selected);
        Drive_Destroy(drive);
    }

    return 0;
}

IPTR DriveListEditor__MUIM_DriveListEditor_MoveUp
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data   = INST_DATA(CLASS, self);
    ULONG                        active = XGET(data->dle_List, MUIA_List_Active);
    
    if (active != 0)
    {
        DoMethod
        (
            data->dle_List, MUIM_List_Exchange,
            active, MUIV_List_Exchange_Previous
        );
        SET(data->dle_List, MUIA_List_Active, MUIV_List_Active_Up);
    }
    
    return 0;
}

IPTR DriveListEditor__MUIM_DriveListEditor_MoveDown
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveListEditor_DATA *data   = INST_DATA(CLASS, self);
    ULONG                        active = XGET(data->dle_List, MUIA_List_Active);
    
    if (active != XGET(data->dle_List, MUIA_List_Entries) - 1)
    {
        DoMethod
        (
            data->dle_List, MUIM_List_Exchange, 
            active, MUIV_List_Exchange_Next
        );
        SET(data->dle_List, MUIA_List_Active, MUIV_List_Active_Down);
    }
    
    return 0;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_8
(
    DriveListEditor, NULL, MUIC_Group, NULL,
    OM_NEW,                             struct opSet *,
    OM_DISPOSE,                         Msg,
    MUIM_DriveListEditor_UpdateButtons, Msg,
    MUIM_DriveListEditor_Add,           Msg,
    MUIM_DriveListEditor_Edit,          Msg,
    MUIM_DriveListEditor_Remove,        Msg,
    MUIM_DriveListEditor_MoveUp,        Msg,
    MUIM_DriveListEditor_MoveDown,      Msg
);
