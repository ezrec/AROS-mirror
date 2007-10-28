/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

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

#include "emulator.h"
#include "emulatorwindow.h"

/*** Instance data **********************************************************/
struct Emulator_DATA
{
    ULONG   ed_State;
    BOOL    ed_Fullscreen;
    Object *ed_Window;
};

/*** Methods ****************************************************************/
IPTR Emulator__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct Emulator_DATA *data;
    Object               *window;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        MUIA_Application_Title,          (IPTR) "UAE",
        MUIA_Application_Version_Number, (IPTR) "0.8.22",
        MUIA_Application_Version_Date,   (IPTR) "2003-11-25",
        MUIA_Application_Copyright,      (IPTR) "Copyright © 1995-2003, various authors (see below).",
        MUIA_Application_Description,    (IPTR) "Amiga emulator.",

        SubWindow, (IPTR) (window = EmulatorWindowObject,
        End),
        
        TAG_DONE
    );
    
    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        data->ed_Window = window;
        
        DoMethod
        ( 
            window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, 
            (IPTR) self, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
        );
    }

    return (IPTR) self;
}

IPTR Emulator__OM_SET(Class *CLASS, Object *self, struct opSet *message)
{
    struct Emulator_DATA *data   = INST_DATA(CLASS, self);
    const struct TagItem *tstate = message->ops_AttrList;
    struct TagItem       *tag;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Emulator_State:
                data->ed_State = tag->ti_Data;
                break;
                
            case MUIA_Emulator_Fullscreen:
                data->ed_Fullscreen = tag->ti_Data;
                break;
        }
    }

    return DoSuperMethodA(CLASS, self, (Msg) message);
}

IPTR Emulator__OM_GET(Class *CLASS, Object *self, struct opGet *message)
{
    struct Emulator_DATA *data  = INST_DATA(CLASS, self);
    IPTR                 *store = message->opg_Storage;
    IPTR                  rv    = TRUE;

    switch (message->opg_AttrID)
    {
        case MUIA_Emulator_State:
            *store = data->ed_State;
            break;

        case MUIA_Emulator_Fullscreen:
            *store = data->ed_Fullscreen;
            break;

	case MUIA_Emulator_Window:
	    *store = (IPTR)data->ed_Window;

	default:
            rv = DoSuperMethodA(CLASS, self, (Msg) message);
    }

    return rv;
}

IPTR Emulator__MUIM_Emulator_Restart(Class *CLASS, Object *self, Msg message)
{
    // FIXME: Implement
    
    return FALSE;
}

IPTR Emulator__MUIM_Emulator_LoadState
(
    Class *CLASS, Object *self, struct MUIP_Emulator_LoadState *message
)
{
    // FIXME: Implement
    
    return FALSE;
}

IPTR Emulator__MUIM_Emulator_SaveState
(
    Class *CLASS, Object *self, struct MUIP_Emulator_SaveState *message
)
{
    // FIXME: Implement
    
    return FALSE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_6
(
    Emulator, NULL, MUIC_Application, NULL,
    OM_NEW,                   struct opSet *,
    OM_SET,                   struct opSet *,
    OM_GET,                   struct opGet *,
    MUIM_Emulator_Restart,    Msg,
    MUIM_Emulator_LoadState,  struct MUIP_Emulator_LoadState *,
    MUIM_Emulator_SaveState,  struct MUIP_Emulator_SaveState *
);
