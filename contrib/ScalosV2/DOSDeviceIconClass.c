// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <string.h>
#include <exec/memory.h>

#include "Scalos.h"
#include "DOSDeviceIconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** DOSDeviceIcon.scalos/--background ***********************************
*
*   This class is used for DOS devices, volumes and assigns. An object of
*   this class will not hold a lock on the device.
*   The SCCA_Icon_Name will return the volume or assign name. To get the name
*   of a device use SCCA_DOSDevice_DeviceName.
*
*****************************************************************************
*/
// /

/****** DOSDeviceIcon.scalos/SCCA_DOSDevice_DeviceName **********************
*
*  NAME
*   SCCA_DOSDevice_DeviceName -- (V40) I.G (char *)
*
*  FUNCTION
*   Getting this attribute will return the name of the device if one is
*   available.
*
*****************************************************************************
*/
// /
/****** DOSDeviceIcon.scalos/SCCA_DOSDevice_AssignType **********************
*
*  NAME
*   SCCA_DOSDevice_AssignType -- (V40) I.G (char *)
*
*  FUNCTION
*   This attribute can be used to find out the type of an assign. It will
*   be a translated string. e.g. late, lock or path
*   If you create an object and you'll specify this attribute you have to
*   use the struct DosList dol_Type values. When creating an assign this
*   attribute must be specified.
*
*****************************************************************************
*/
// /

/** Init a object
*/

static ULONG DOSDeviceIcon_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct DOSDeviceIconInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                char *string;

                inst->assigntype = GetTagData(SCCA_DOSDeviceIcon_AssignType,0,msg->ops_AttrList);
                inst->handler = (struct MsgPort *) GetTagData(SCCA_DOSDeviceIcon_Handler,0,msg->ops_AttrList);

                if ((string = (char *) GetTagData(SCCA_Icon_Name,0,msg->ops_AttrList)))
                {
                        if ((inst->volname = AllocVec(strlen(string) + 2,MEMF_ANY)))
                        {
                                strcpy(inst->volname,string);
                                if (!inst->assigntype)
                                        strcat(inst->volname,":");
                        }
                }

                if ((string = (char *) GetTagData(SCCA_DOSDeviceIcon_DeviceName,0,msg->ops_AttrList)))
                {
                        if ((inst->devname = AllocVec(strlen(string) + 2,MEMF_ANY)))
                        {
                                strcpy(inst->devname,string);
                                strcat(inst->devname,":");
                        }
                }
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG DOSDeviceIcon_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct DOSDeviceIconInst *inst )
{
        if (msg->opg_AttrID == SCCA_Icon_Name)
        {
                *(msg->opg_Storage) = (LONG) inst->volname;
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_DOSDeviceIcon_DeviceName)
        {
                *(msg->opg_Storage) = (LONG) inst->devname;
                return( TRUE );
        }

        if (msg->opg_AttrID == SCCA_DOSDeviceIcon_AssignType)
        {
                *(msg->opg_Storage) = inst->assigntype;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_DOSIcon_Lock)
        {
                if ((inst->assigntype == DLT_DEVICE) || (inst->assigntype == DLT_VOLUME))
                {
                                // maybe we have to check if the handler exists first
                                if (inst->handler)
                                {
                                        *(msg->opg_Storage) = (LONG) DoPkt2(inst->handler,ACTION_LOCATE_OBJECT,NULL,NULL);
                                        return(TRUE);
                                }
                }
                else
                {
                        if (inst->volname)
                        {
                                *(msg->opg_Storage) = (LONG) Lock(inst->volname,SHARED_LOCK);
                                return(TRUE);
                        }
                        if (inst->devname)
                        {
                                *(msg->opg_Storage) = (LONG) Lock(inst->devname,SHARED_LOCK);
                                return(TRUE);
                        }
                }

                return(FALSE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/** Get a string of an attribute
*/
static char *DOSDeviceIcon_GetString(struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetString *msg, struct DOSDeviceIconInst *inst)
{
        if (msg->Attr == SCCA_DOSDeviceIcon_AssignType)
        {
                char *typestr;

                switch(inst->assigntype)
                {
                  case DLT_DIRECTORY:
                        typestr = "lock";
                        break;
                  case DLT_LATE:
                        typestr = "late";
                        break;
                  case DLT_NONBINDING:
                        typestr = "path";
                        break;
                  default:
                        typestr = "device";
                }
                return(AllocCopyString(typestr));
        }

        return(NULL);
}
// /

/** exit routine
*/
static void DOSDeviceIcon_Exit( struct SC_Class *cl, Object *obj, Msg msg, struct DOSDeviceIconInst *inst )
{
        if (inst->volname)
                FreeVec(inst->volname);
        if (inst->devname)
                FreeVec(inst->devname);

        SC_DoSuperMethodA(cl,obj,msg);
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData DOSDeviceIconMethods[] =
{
        { SCCM_Init,(ULONG) DOSDeviceIcon_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) DOSDeviceIcon_Exit, 0, 0, NULL },
        { OM_GET,(ULONG) DOSDeviceIcon_Get, 0, 0, NULL },
        { SCCM_Icon_GetString,(ULONG) DOSDeviceIcon_GetString, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

