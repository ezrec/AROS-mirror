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
#include "drivelist.h"

/*** Instance data **********************************************************/
struct DriveList_DATA
{
    BYTE dld_DUMMY;
};

/*** Hooks ******************************************************************/
AROS_UFH3
(   
    void, DriveList_DisplayFunction,
    AROS_UFHA(struct Hook *,  hook,    A0),
    AROS_UFHA(char **,        strings, A2),
    AROS_UFHA(struct Drive *, entry,   A1)
)
{
    if (entry)
    {
        strings[0] = entry->d_Path;
        strings[1] = entry->d_Type == DT_HARDFILE ? 
              _(MSG_CFG_DRV_DL_TYPE_HARDFILE)
            : _(MSG_CFG_DRV_DL_TYPE_FILESYSTEM);
        strings[2] = entry->d_Mode == DM_READONLY ?
              _(MSG_CFG_DRV_DL_MODE_RO)
            : _(MSG_CFG_DRV_DL_MODE_RW);
    }
    else
    {
        strings[0] = _(MSG_CFG_DRV_DL_PATH);
        strings[1] = _(MSG_CFG_DRV_DL_TYPE);
        strings[2] = _(MSG_CFG_DRV_DL_MODE);
    }
}

struct Hook DriveList_DisplayHook = { { 0 }, 0 };

/*** Methods ****************************************************************/
IPTR DriveList__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    DriveList_DisplayHook.h_Entry = (HOOKFUNC) DriveList_DisplayFunction;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        MUIA_List_Title,              TRUE,
        MUIA_List_Format,      (IPTR) ",,",
        MUIA_List_DisplayHook, (IPTR) &DriveList_DisplayHook,
        
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    return (IPTR) self;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_1
(
    DriveList, NULL, MUIC_List, NULL,
    OM_NEW, struct opSet *
);
