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
#include "devicecontrol.h"

/*** Private methods ********************************************************/
#define MUIM_DeviceControl_HandleMenu   (TAG_USER | 0x10000000)
struct  MUIP_DeviceControl_HandleMenu   {ULONG MethodID; Object *item;};

/*** Instance data **********************************************************/
struct DeviceControl_DATA
{
    Object *dcd_Image,
           *dcd_InsertMI,
           *dcd_EjectMI;
    ULONG   dcd_Type;
    BOOL    dcd_Activity;
    STRPTR  dcd_Path,
            dcd_Name;
};

/*** Methods ****************************************************************/
IPTR DeviceControl__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct DeviceControl_DATA *data   = INST_DATA(CLASS, self);
    Object                    *image;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        TextFrame,
        InnerSpacing(0, 0),
        Child, (IPTR) (image = ImageObject,
            MUIA_Background, (IPTR) NULL,
            MUIA_Image_Spec, (IPTR) NULL,
        End),
        
        TAG_DONE
    );
    
    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        data->dcd_Image    = image;
        
        SetAttrsA(self, message->ops_AttrList);
        
        if (data->dcd_Type == MUIV_DeviceControl_Type_None)
        {
            SET
            (
                self, MUIA_DeviceControl_Type, 
                MUIV_DeviceControl_Type_FloppyDrive
            );
        }
        
        DoMethod
        (
            data->dcd_Image, MUIA_ContextMenuTrigger, MUIV_EveryTime,
            (IPTR) self, 2, MUIM_DeviceControl_HandleMenu, MUIV_TriggerValue
        );
    }
    
    return (IPTR) self;
}

IPTR DeviceControl__OM_SET(Class *CLASS, Object *self, struct opSet *message)
{
    struct DeviceControl_DATA *data   = INST_DATA(CLASS, self);
    const struct TagItem      *tstate = message->ops_AttrList;
    struct TagItem            *tag;

    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_DeviceControl_Type:
                if (data->dcd_Type != tag->ti_Data)
                {
                    Object *menu = (Object *) XGET(data->dcd_Image, MUIA_ContextMenu);
                    data->dcd_Type = tag->ti_Data;
                    
                    if (menu != NULL)
                    {
                        SET(data->dcd_Image, MUIA_ContextMenu, NULL);
                        data->dcd_InsertMI = NULL;
                        data->dcd_EjectMI  = NULL;
                        DisposeObject(menu);
                    }
                    
                    switch (data->dcd_Type)
                    {
                        case MUIV_DeviceControl_Type_FloppyDrive:
                            SET
                            (
                                data->dcd_Image, MUIA_Image_Spec, 
                                "3:PROGDIR:Images/FloppyDrive"
                            );
                            
                            /* FIXME: Enable when context menus work in Zune.
                            SET
                            (
                                data->dcd_Image, MUIA_ContextMenu,
                                (MenuitemObject,
                                    Child, (IPTR) (data->dcd_InsertMI = MenuitemObject,
                                        MUIA_Menuitem_Title,   __(MSG_DC_INSERT),
                                    End),
                                    Child, (IPTR) (data->dcd_EjectMI = MenuitemObject,
                                        MUIA_Menuitem_Title,   __(MSG_DC_EJECT),
                                        MUIA_Menuitem_Enabled, data->dcd_Path != NULL,
                                    End),
                                End)
                            );
                            */
                            break;
                        
                        case MUIV_DeviceControl_Type_HardDrive:
                            SET
                            (
                                data->dcd_Image, MUIA_Image_Spec,
                                "3:PROGDIR:Images/HardDrive"
                            );
                            break;
                    }
                }
                break;
                
            case MUIA_DeviceControl_Activity:
                if (data->dcd_Activity != tag->ti_Data)
                {
                    data->dcd_Activity = tag->ti_Data;
                    
                    if (data->dcd_Activity)
                    {
                        SET
                        (
                            data->dcd_Image, MUIA_Background, 
                            "2:00000000,FFFFFFFF,00000000"
                        );
                    }
                    else
                    {
                        SET(data->dcd_Image, MUIA_Background, NULL);
                    }
                }
                break;

            case MUIA_DeviceControl_Path:
                if (((CONST_STRPTR) tag->ti_Data) != NULL)
                {
                    if (data->dcd_Path != NULL) FreeVec(data->dcd_Path);
                    data->dcd_Path = StrDup((CONST_STRPTR) tag->ti_Data);
                
                    SET(data->dcd_EjectMI, MUIA_Menuitem_Enabled, TRUE);
                }
                else
                {
                    SET(data->dcd_InsertMI, MUIA_Menuitem_Enabled, FALSE);
                }
                break;
                
            case MUIA_DeviceControl_Name:
                if (((CONST_STRPTR) tag->ti_Data) != NULL)
                {
                    if (data->dcd_Name != NULL) FreeVec(data->dcd_Name);
                    data->dcd_Name = StrDup((CONST_STRPTR) tag->ti_Data);
                    
                    SET(data->dcd_Image, MUIA_ShortHelp, data->dcd_Name);
                }
                break;
        }
    }
    
    return DoSuperMethodA(CLASS, self, (Msg) message);
}

IPTR DeviceControl__OM_GET(Class *CLASS, Object *self, struct opGet *message)
{
    struct DeviceControl_DATA *data = INST_DATA(CLASS, self);
    IPTR                      *store = message->opg_Storage;
    IPTR                       rv    = TRUE;
    
    switch (message->opg_AttrID)
    {
        case MUIA_DeviceControl_Type:
            *store = data->dcd_Type;
            break;
            
        case MUIA_DeviceControl_Activity:
            *store = data->dcd_Activity;
            break;
            
        case MUIA_DeviceControl_Path:
            *store = (IPTR) data->dcd_Path;
            break;
            
        case MUIA_DeviceControl_Name:
            *store = (IPTR) data->dcd_Name;
            break;
            
        default:
            rv = DoSuperMethodA(CLASS, self, (Msg) message);
    }
    
    return rv;
}

IPTR DeviceControl__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct DeviceControl_DATA *data = INST_DATA(CLASS, self);
    Object                    *menu = NULL;
    
    if (data->dcd_Path != NULL) FreeVec(data->dcd_Path);
    if (data->dcd_Name != NULL) FreeVec(data->dcd_Name);
    
    menu = (Object *) XGET(data->dcd_Image, MUIA_ContextMenu);
    if (menu != NULL) DisposeObject(menu);
    
    return DoSuperMethodA(CLASS, self, message);
}

IPTR DeviceControl__MUIM_DeviceControl_HandleMenu
(
    Class *CLASS, Object *self, struct MUIP_DeviceControl_HandleMenu *message
)
{
    //struct DeviceControl_DATA *data = INST_DATA(CLASS, self);
    
    // FIXME: implement... however, context menus don't seem to be fully implemented in Zune
    bug("*** context menu!\n");
    
    return TRUE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_5
(
    DeviceControl, NULL, MUIC_Group, NULL,
    OM_NEW,                        struct opSet *,
    OM_DISPOSE,                    Msg,
    OM_SET,                        struct opSet *,
    OM_GET,                        struct opGet *,
    MUIM_DeviceControl_HandleMenu, struct MUIP_DeviceControl_HandleMenu *
);
