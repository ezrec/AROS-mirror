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
#include "driveeditor.h"

/*** Strings ****************************************************************/
CONST_STRPTR DRV_Type[3];
CONST_STRPTR DRV_Mode[3];
const TEXT   DIGITS[] = "0123456789";

/*** Private methods ********************************************************/
#define MUIM_DriveEditor_UpdateGadgets (MUIB_DriveEditor | 0x00000000)
#define MUIM_DriveEditor_OK            (MUIB_DriveEditor | 0x00000001)
#define MUIM_DriveEditor_Cancel        (MUIB_DriveEditor | 0x00000002)

/*** Instance data **********************************************************/
struct DriveEditor_DATA
{
    struct Drive *ded_Drive;
    Object       *ded_Group,
                 *ded_TypeCycle,
                 *ded_ModeCycle, 
                 *ded_PathString, 
                 *ded_VolumeLabel,
                 *ded_VolumeString, 
                 *ded_SectorsLabel, 
                 *ded_SectorsString, 
                 *ded_HeadsLabel, 
                 *ded_HeadsString, 
                 *ded_ReservedLabel,
                 *ded_ReservedString,
                 *ded_OKButton,
                 *ded_CancelButton;
    ULONG         ded_Type;
};

/*** Methods ****************************************************************/
IPTR DriveEditor__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);
    Object *typeCycle, *modeCycle, *pathString, *volumeString, *volumeLabel,
           *sectorsString, *headsString, *reservedString, 
           *sectorsLabel, *headsLabel, *reservedLabel,
           *okButton, *cancelButton, *group;
    
    /*-- Localize strings --------------------------------------------------*/
    DRV_Type[0] = _(MSG_CFG_DRV_DE_TYPE_FILESYSTEM);
    DRV_Type[1] = _(MSG_CFG_DRV_DE_TYPE_HARDFILE);
    DRV_Type[2] = NULL;
    
    DRV_Mode[0] = _(MSG_CFG_DRV_DE_MODE_READWRITE);
    DRV_Mode[1] = _(MSG_CFG_DRV_DE_MODE_READONLY);
    DRV_Mode[2] = NULL;
    
    /*-- Create objects and layout -----------------------------------------*/
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
            
        MUIA_Window_Title,       __(MSG_CFG_DRV_DE_TITLE),
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_NoMenus,     TRUE,
        
        WindowContents, (IPTR) VGroup,
            Child, (IPTR) group = ColGroup(2),
                Child, (IPTR) Label2(_(MSG_CFG_DRV_DE_TYPE)),
                Child, (IPTR) typeCycle = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) DRV_Type),
                Child, (IPTR) Label2(_(MSG_CFG_DRV_DE_MODE)),
                Child, (IPTR) modeCycle = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) DRV_Mode),
                Child, (IPTR) Label2(_(MSG_CFG_DRV_DE_PATH)),
                Child, (IPTR) PopaslObject,
                    MUIA_Popstring_String, (IPTR) pathString = StringObject,
                        StringFrame,
                    End,
                    MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                End,
                Child, (IPTR) volumeLabel = Label2(_(MSG_CFG_DRV_DE_VOLUME)),
                Child, (IPTR) volumeString = StringObject,
                    StringFrame,
                End,
                Child, (IPTR) sectorsLabel = Label2(_(MSG_CFG_DRV_DE_SECTORS)),
                Child, (IPTR) sectorsString = StringObject,
                    StringFrame,
                    MUIA_String_Accept, (IPTR) DIGITS,
                End,
                Child, (IPTR) headsLabel = Label2(_(MSG_CFG_DRV_DE_HEADS)),
                Child, (IPTR) headsString = StringObject,
                    StringFrame,
                    MUIA_String_Accept, (IPTR) DIGITS,
                End,
                Child, (IPTR) reservedLabel = Label2(_(MSG_CFG_DRV_DE_RESERVED)),
                Child, (IPTR) reservedString = StringObject,
                    StringFrame,
                    MUIA_String_Accept, (IPTR) DIGITS,
                End,
            End,
            Child, (IPTR) HVSpace,
            Child, (IPTR) RectangleObject, 
                MUIA_Rectangle_HBar, TRUE, 
                MUIA_FixHeight,      2, 
            End,
            Child, (IPTR) HGroup,
                Child, (IPTR) HVSpace,
                Child, (IPTR) HGroup,
                    MUIA_Weight,         0,
                    MUIA_Group_SameSize, TRUE,
                    
                    Child, (IPTR) okButton     = SimpleButton(_(MSG_OK)),
                    Child, (IPTR) cancelButton = SimpleButton(_(MSG_CANCEL)),
                End,
            End,
        End,
        
        TAG_DONE
    );
    
    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        data->ded_Group          = group;
        data->ded_TypeCycle      = typeCycle;
        data->ded_ModeCycle      = modeCycle;
        data->ded_PathString     = pathString;
        data->ded_VolumeLabel    = volumeLabel;
        data->ded_VolumeString   = volumeString;
        data->ded_SectorsLabel   = sectorsLabel;
        data->ded_SectorsString  = sectorsString;
        data->ded_HeadsLabel     = headsLabel;
        data->ded_HeadsString    = headsString;
        data->ded_ReservedLabel  = reservedLabel;
        data->ded_ReservedString = reservedString;
        data->ded_OKButton       = okButton;
        data->ded_CancelButton   = cancelButton;
	data->ded_Type           = 0;
        
        /*-- Setup notifications -------------------------------------------*/
        DoMethod
        (
            typeCycle, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_DriveEditor_UpdateGadgets
        );

        /*-- Sane defaults -------------------------------------------------*/
        SET(volumeString, MUIA_String_Contents, __(MSG_CFG_DRV_DE_VOLUME_DEFAULT));
        SET(sectorsString,  MUIA_String_Integer, 32);
        SET(headsString,    MUIA_String_Integer,  1);
        SET(reservedString, MUIA_String_Integer,  2);

        /*-- Miscellanous initialization -----------------------------------*/
        DoMethod(group, OM_REMMEMBER, (IPTR) sectorsLabel);
        DoMethod(group, OM_REMMEMBER, (IPTR) sectorsString);
        DoMethod(group, OM_REMMEMBER, (IPTR) headsLabel);
        DoMethod(group, OM_REMMEMBER, (IPTR) headsString);
        DoMethod(group, OM_REMMEMBER, (IPTR) reservedLabel);
        DoMethod(group, OM_REMMEMBER, (IPTR) reservedString);
    
        /*-- Parse initial taglist -----------------------------------------*/
        SetAttrsA(self, message->ops_AttrList);
    }
    
    return (IPTR) self;
}

IPTR DriveEditor__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);
    
    int     i      = 0;
    Object *objs[] =
    {
        data->ded_VolumeLabel,
        data->ded_VolumeString, 
        data->ded_SectorsLabel, 
        data->ded_SectorsString, 
        data->ded_HeadsLabel, 
        data->ded_HeadsString, 
        data->ded_ReservedLabel,
        data->ded_ReservedString,
        NULL
    };

    while (objs[i] != NULL)
    {
        if (XGET(objs[i], MUIA_Parent) == NULL)
        {
            MUI_DisposeObject(objs[i]);
        }
    }
    
    return DoSuperMethodA(CLASS, self, message);
}

IPTR DriveEditor__OM_SET(Class *CLASS, Object *self, struct opSet *message)
{
    struct DriveEditor_DATA *data   = INST_DATA(CLASS, self);
    struct TagItem          *tstate = message->ops_AttrList,
                            *tag;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_DriveEditor_Drive:
                data->ded_Drive = (struct Drive *) tag->ti_Data;
                
                SET(data->ded_TypeCycle, MUIA_Cycle_Active, data->ded_Drive->d_Type);
                SET(data->ded_ModeCycle, MUIA_Cycle_Active, data->ded_Drive->d_Mode);
                SET(data->ded_PathString, MUIA_String_Contents, data->ded_Drive->d_Path);
                
                if (data->ded_Drive->d_Type == DT_FILESYSTEM)
                {
                    if (data->ded_Drive->d_Parameters.FS.VolumeName != NULL)
                    {
                        SET
                        (
                            data->ded_VolumeString, MUIA_String_Contents,
                            data->ded_Drive->d_Parameters.FS.VolumeName
                        );
                    }
                }
                else if (data->ded_Drive->d_Type == DT_HARDFILE)
                {
                    if (data->ded_Drive->d_Parameters.HF.Sectors != 0)
                    {
                        SET
                        (
                            data->ded_SectorsString, MUIA_String_Integer,
                            data->ded_Drive->d_Parameters.HF.Sectors
                        );
                    }
                    
                    if (data->ded_Drive->d_Parameters.HF.Heads != 0)
                    {
                        SET
                        (
                            data->ded_HeadsString, MUIA_String_Integer,
                            data->ded_Drive->d_Parameters.HF.Heads
                        );
                    }
                    
                    if (data->ded_Drive->d_Parameters.HF.Reserved != 0)
                    {
                        SET
                        (
                            data->ded_ReservedString, MUIA_String_Integer,
                            data->ded_Drive->d_Parameters.HF.Reserved
                        );
                    }
                }
                
                break;
        }
    }
    
    return DoSuperMethodA(CLASS, self, (Msg) message);
}

IPTR DriveEditor__OM_GET(Class *CLASS, Object *self, struct opGet *message)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);
    IPTR                    *store = message->opg_Storage;
    IPTR                     rv    = TRUE;
    
    switch (message->opg_AttrID)
    {
        case MUIA_DriveEditor_Drive:
            *store = (IPTR) data->ded_Drive;
            break;
            
        default:
            rv = DoSuperMethodA(CLASS, self, (Msg) message);
    }
    
    return rv;
}

IPTR DriveEditor__MUIM_Window_Setup
(
    Class *CLASS, Object *self, struct MUIP_Window_Setup *message
)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);

    if (!DoSuperMethodA(CLASS, self, (Msg) message)) return FALSE;
    
    DoMethod
    (
        self, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        (IPTR) self, 1, MUIM_DriveEditor_Cancel
    );
    DoMethod
    (
        data->ded_CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) self, 1, MUIM_DriveEditor_Cancel
    );
    
    DoMethod
    (
        data->ded_OKButton, MUIM_Notify, MUIA_Pressed, FALSE,
        (IPTR) self, 1, MUIM_DriveEditor_OK
    );

    return TRUE;
}

IPTR DriveEditor__MUIM_DriveEditor_UpdateGadgets
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);
    ULONG                    type = XGET(data->ded_TypeCycle, MUIA_Cycle_Active);
    ULONG                    hfAction, fsAction;

    if (type == data->ded_Type) return 0;

    data->ded_Type = type;

    if (type != 0)
    {
        hfAction = OM_ADDMEMBER;
	fsAction = OM_REMMEMBER;
    }
    else
    {
        hfAction = OM_REMMEMBER;
	fsAction = OM_ADDMEMBER;
    }

    DoMethod(data->ded_Group, MUIM_Group_InitChange);

    /*-- Hardfile parameters -----------------------------------------------*/
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_SectorsLabel);
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_SectorsString);
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_HeadsLabel);
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_HeadsString);
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_ReservedLabel);
    DoMethod(data->ded_Group, hfAction, (IPTR) data->ded_ReservedString);

    /*-- Filesystem parameters ---------------------------------------------*/
    DoMethod(data->ded_Group, fsAction, (IPTR) data->ded_VolumeLabel);
    DoMethod(data->ded_Group, fsAction, (IPTR) data->ded_VolumeString);

    DoMethod(data->ded_Group, MUIM_Group_ExitChange);

    return 0;
}

IPTR DriveEditor__MUIM_DriveEditor_OK
(
    Class *CLASS, Object *self, Msg message
)
{
    struct DriveEditor_DATA *data = INST_DATA(CLASS, self);
    
    /*-- Close the dialog window -------------------------------------------*/
    SET(self, MUIA_Window_Open, FALSE);
    
    /*-- Update the drive data, if possible --------------------------------*/
    if (data->ded_Drive != NULL)
    {
        if
        (
               data->ded_Drive->d_Type                     == DT_FILESYSTEM
            && data->ded_Drive->d_Parameters.FS.VolumeName != NULL
        )
        {
            FreeVec(data->ded_Drive->d_Parameters.FS.VolumeName);
        }
        
        if (data->ded_Drive->d_Path != NULL)
        {
            FreeVec(data->ded_Drive->d_Path);
        }
        
        data->ded_Drive->d_Type = XGET(data->ded_TypeCycle, MUIA_Cycle_Active);
        data->ded_Drive->d_Mode = XGET(data->ded_ModeCycle, MUIA_Cycle_Active);
                
        data->ded_Drive->d_Path = StrDup
        (
            (STRPTR) XGET(data->ded_PathString, MUIA_String_Contents)
        );
        
        if (data->ded_Drive->d_Type == DT_FILESYSTEM)
        {
            data->ded_Drive->d_Parameters.FS.VolumeName = StrDup
            (
                (STRPTR) XGET(data->ded_VolumeString, MUIA_String_Contents)
            );
        }
        else if (data->ded_Drive->d_Type == DT_HARDFILE)
        {
            data->ded_Drive->d_Parameters.HF.Sectors = XGET
            (
                data->ded_SectorsString, MUIA_String_Integer
            );
            
            data->ded_Drive->d_Parameters.HF.Heads = XGET
            (
                data->ded_HeadsString, MUIA_String_Integer
            );
            
            data->ded_Drive->d_Parameters.HF.Reserved = XGET
            (
                data->ded_ReservedString, MUIA_String_Integer
            );
        }
    }
    
    /*-- Notify the parent -------------------------------------------------*/
    DoMethod
    (
        _app(self), MUIM_Application_ReturnID, MUIV_DriveEditor_ReturnID_OK
    );

    return 0;
}

IPTR DriveEditor__MUIM_DriveEditor_Cancel
(
    Class *CLASS, Object *self, Msg message
)
{
    /*-- Close the dialog window -------------------------------------------*/
    SET(self, MUIA_Window_Open, FALSE);
    
    /*-- Notify the parent -------------------------------------------------*/
    DoMethod
    (
        _app(self), MUIM_Application_ReturnID, MUIV_DriveEditor_ReturnID_Cancel
    );

    return 0;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_7
(
    DriveEditor, NULL, MUIC_Window, NULL,
    OM_NEW,                         struct opSet *,
    OM_SET,                         struct opSet *,
    OM_GET,                         struct opGet *,
    MUIM_Window_Setup,              struct MUIP_Window_Setup *,
    MUIM_DriveEditor_UpdateGadgets, Msg,
    MUIM_DriveEditor_OK,            Msg,
    MUIM_DriveEditor_Cancel,        Msg
);
