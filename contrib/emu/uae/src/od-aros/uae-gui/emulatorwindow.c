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

#include <zune/aboutwindow.h>

#include "locale.h"
#include "emulatorwindow.h"
#include "emulator.h"
#include "devicecontrol.h"
#include "toolbar.h"
#include "configurationeditor.h"
#include "uaedisplay.h"
   
/*** Private methods ********************************************************/
enum
{
    MUIM_EmulatorWindow_ChangePage = TAG_USER | 0x01000000,
    MUIM_EmulatorWindow_ShowAbout,
    MUIM_EmulatorWindow_HideAbout
};

struct  MUIP_EmulatorWindow_ChangePage  {ULONG MethodID; ULONG State;};

/*** Instance data **********************************************************/
struct EmulatorWindow_DATA
{
    Object *ewd_PageGroup,
           *ewd_AboutWindow,
	   *ewd_DisplayObject;
};

/*** Utility functions ******************************************************/
Object *makeMenuitem(CONST_STRPTR text)
{
    CONST_STRPTR title    = NULL, 
                 shortcut = NULL;
    
    if (text[1] == '\0')
    {
        title    = text + 2;
        shortcut = text;
    }
    else
    {
        title    = text;
        shortcut = NULL;
    }
    
    return MenuitemObject,
        MUIA_Menuitem_Title,      (IPTR) title,
        shortcut != NULL       ?
        MUIA_Menuitem_Shortcut :
        TAG_IGNORE,               (IPTR) shortcut,
    End;
}

/*** Methods ****************************************************************/
IPTR EmulatorWindow__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct EmulatorWindow_DATA *data = NULL;
    Object *pageGroup, *aboutMI, *quitMI, *uaedisplay;

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        MUIA_Window_Menustrip, (IPTR) MenustripObject,
            Child, (IPTR) MenuObject,
                MUIA_Menu_Title, __(MSG_MENU_PROJECT),
                
                Child, (IPTR) (aboutMI = makeMenuitem(_(MSG_MENU_PROJECT_ABOUT))),
                Child, (IPTR) (quitMI  = makeMenuitem(_(MSG_MENU_PROJECT_QUIT))),
            End,
        End,
            
        WindowContents, (IPTR) VGroup,
            InnerSpacing(0, 0),
            GroupSpacing(0),
            
            Child, (IPTR) ToolbarObject, End,
            Child, (IPTR) pageGroup = PageGroup,
                InnerSpacing(0, 0),
                GroupSpacing(0),

                Child, (IPTR) ConfigurationEditorObject, End,
                Child, (IPTR) VGroup,
                    InnerSpacing(0, 0),
                    GroupSpacing(0),
                    Child, (IPTR) uaedisplay = UAEDisplayObject, End,
                    Child, (IPTR) HGroup,
                        ButtonFrame,
                        MUIA_Background, MUII_GroupBack,

                        Child, (IPTR) HVSpace,
                        Child, (IPTR) HGroup,
                            GroupSpacing(2),
                            MUIA_Weight,         0,
                            MUIA_Group_SameSize, TRUE,

                            Child, (IPTR) HVSpace,
                            // FIXME: these should be added dynamically
                            Child, (IPTR) DeviceControlObject,
                                MUIA_DeviceControl_Name, (IPTR) "DF0",
                                MUIA_DeviceControl_Type,        MUIV_DeviceControl_Type_FloppyDrive,
                            End,
                            Child, (IPTR) DeviceControlObject,
                                MUIA_DeviceControl_Name, (IPTR) "DH0",
                                MUIA_DeviceControl_Type,        MUIV_DeviceControl_Type_HardDrive,
                            End,
                        End,
                    End,
                End,
            End,
        End,

	TAG_MORE, (IPTR)message->ops_AttrList
    );

    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        data->ewd_PageGroup     = pageGroup;
        data->ewd_DisplayObject = uaedisplay;

	DoMethod
        (
            aboutMI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_EmulatorWindow_ShowAbout
        );
        DoMethod
        (
            quitMI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (IPTR) self, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE
        );
    }

    return (IPTR) self;
}

IPTR EmulatorWindow__OM_GET(Class *CLASS, Object *self, struct opGet *message)
{
    struct EmulatorWindow_DATA *data = INST_DATA(CLASS, self);

    switch (message->opg_AttrID)
    {
	case MUIA_EmulatorWindow_DisplayObject:
	    *(message->opg_Storage) = (IPTR)data->ewd_DisplayObject;
	    break;

	default:
            return DoSuperMethodA(CLASS, self, (Msg)message);
    }

    return TRUE;
}

IPTR EmulatorWindow__MUIM_Window_Setup
(
    Class *CLASS, Object *self, struct MUIP_Window_Setup *message
)
{
    struct EmulatorWindow_DATA *data = INST_DATA(CLASS, self);
    
    if (!DoSuperMethodA(CLASS, self, (Msg) message)) return FALSE;

    SET(self, MUIA_Window_DefaultObject, data->ewd_DisplayObject);

    DoMethod
    (
        _app(self), MUIM_Notify, MUIA_Emulator_State, MUIV_EveryTime,
        (IPTR) self, 2, MUIM_EmulatorWindow_ChangePage, MUIV_TriggerValue
    );

    return TRUE;
}

IPTR EmulatorWindow__MUIM_EmulatorWindow_ChangePage
(
    Class *CLASS, Object *self, struct MUIP_EmulatorWindow_ChangePage *message
)
{
    struct EmulatorWindow_DATA *data = INST_DATA(CLASS, self);

    if (message->State == MUIV_Emulator_State_Running)
    {
        SET(data->ewd_PageGroup, MUIA_Group_ActivePage, 1);
        SET(self, MUIA_Window_Title, __(MSG_WINDOW_TITLE_DISPLAY));
    }
    else
    {
        SET(data->ewd_PageGroup, MUIA_Group_ActivePage, 0);
        SET(self, MUIA_Window_Title, __(MSG_WINDOW_TITLE_CFG));
    }
    
    return NULL;
}

IPTR EmulatorWindow__MUIM_EmulatorWindow_ShowAbout
(
    Class *CLASS, Object *self, Msg message
)
{
    struct EmulatorWindow_DATA *data = INST_DATA(CLASS, self);
    
    if (data->ewd_AboutWindow != NULL)
    {
        SET(data->ewd_AboutWindow, MUIA_Window_Open, TRUE);
        SET(data->ewd_AboutWindow, MUIA_Window_Activate, TRUE);
    }
    else
    {
        data->ewd_AboutWindow = AboutWindowObject,
            MUIA_AboutWindow_Authors, (IPTR) TAGLIST
            (
                SECTION
                (
                    SID_PROGRAMMING,
                    NAME("Adam Chodorowski"),
                    NAME("Fabio Alemagna"),
                    NAME("Herman ten Brugge"),
                    NAME("Bernd Schmidt"),
                    NAME("Marcus Sundberg"),
                    NAME("Manfred Thole"),
                    NAME("Ed Hanway"),
                    NAME("Mathias Ortmann"),
                    NAME("Brian King"),
                    NAME("Alessandro Bissacco"),
                    NAME("Stefan Reinauer"),
                    NAME("Christian Schmitt"),
                    NAME("Toni Wilen"),
                    NAME("Hannu Rummukainen"),
                    NAME("Michael Krause"),
                    NAME("Samuel Devulder"),
                    NAME("Patrick Ohly"),
                    NAME("Ian Stephenson"),
                    NAME("Andre Beck"),
                    NAME("Jim Cooper"),
                    NAME("Bernd Lachner"),
                    NAME("Samuel Devulder"),
                    NAME("Bruno Coste"),
                    NAME("Tim Gunn"),
                    NAME("Krister Walfridsson")
                ),
                SECTION
                (
                    SID_TRANSLATING,
                    NAME("Adam Chodorowski"),
                    NAME("Fabio Alemagna")
                )
            ),
        End;
        
        if (data->ewd_AboutWindow != NULL)
        {
            DoMethod(_app(self), OM_ADDMEMBER, (IPTR) data->ewd_AboutWindow);
            
            DoMethod
            (
                data->ewd_AboutWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                (IPTR) self, 1, MUIM_EmulatorWindow_HideAbout
            );
            
            SET(data->ewd_AboutWindow, MUIA_Window_Open, TRUE);
        }
    }
    
    return NULL;
}

IPTR EmulatorWindow__MUIM_EmulatorWindow_HideAbout
(
    Class *CLASS, Object *self, Msg message
)
{
    struct EmulatorWindow_DATA *data   = INST_DATA(CLASS, self);
    Object                     *window = data->ewd_AboutWindow;
    
    data->ewd_AboutWindow = NULL;
    SET(window, MUIA_Window_Open, FALSE);
    DoMethod(_app(self), OM_REMMEMBER, (IPTR) window);
    MUI_DisposeObject(window);
    
    return NULL;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_6
(
    EmulatorWindow, NULL, MUIC_Window, NULL,
    OM_NEW,                         struct opSet *,
    OM_GET,                         struct opGet *,
    MUIM_Window_Setup,              struct MUIP_Window_Setup *,
    MUIM_EmulatorWindow_ChangePage, struct MUIP_EmulatorWindow_ChangePage *,
    MUIM_EmulatorWindow_ShowAbout,  Msg,
    MUIM_EmulatorWindow_HideAbout,  Msg
);
