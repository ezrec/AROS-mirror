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
#include <dos/dos.h>

#include "Scalos.h"
#include "DOSListIconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** DOSListIcon.scalos/--background *************************************
*
*   This class is the parent of all DOS classes. It's a list of all available
*   DOS devices, assigns, volumes.
*
*****************************************************************************
*/
// /

/****** DOSListIcon.scalos/SCCM_Icon_Entry **********************************
*
*  NOTES
*   Not all available values from SCCM_Icon_Entry are implemented in this
*   class. Available values are:
*       SCCV_Icon_Entry_First - initiales the list and selectes the first
*                               entry
*       SCCV_Icon_Entry_Next - selectes the next entry
*       SCCV_Icon_Entry_None - release the lock of the doslist and frees
*                              temporary memory
*
*   After SCCV_Icon_Entry_First the DosList is locked. You should quickly
*   parse through the list and release the lock with SCCV_Icon_Entry_None.
*
*  SEE ALSO
*   Icon.scalos/SCCM_Icon_Entry
*
*****************************************************************************
*/
static void GenerateVolDevList(struct MinList *list,struct DosList *dolentry)
{
        struct DosList *curnode = dolentry;
        struct DevListNode *devnode;
        BOOL found;

                // generate a list of all volumes
        while ((curnode = NextDosEntry(curnode,LDF_VOLUMES)))
        {
                if ((devnode = (struct DevListNode *) AllocNode((struct MinList *) list,sizeof(struct DevListNode))))
                        devnode->volume = curnode;
        }

                // now parse the doslist again to find devices which belongs to volumes
        curnode = dolentry;
        while ((curnode = NextDosEntry(curnode,LDF_DEVICES)))
        {
                found = FALSE;

                for (devnode = (struct DevListNode *) list->mlh_Head; ((struct MinNode *) devnode)->mln_Succ; devnode = (struct DevListNode *) ((struct MinNode *) devnode)->mln_Succ)
                {
                        if (devnode->volume && (devnode->volume->dol_Task == curnode->dol_Task))
                        {
                                devnode->device = curnode;
                                found = TRUE;
                                break;
                        }
                }

                if (!found && (devnode = (struct DevListNode *) AllocNode((struct MinList *) list,sizeof(struct DevListNode))))
                                devnode->device = curnode;
        }
}


static ULONG DOSListIcon_Entry(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Entry *msg, struct DOSListIconInst *inst)
{

        if (msg->Pos == SCCV_Icon_Entry_First)
        {
                inst->entry = LockDosList(LDF_ALL | LDF_READ);

                GenerateVolDevList(&inst->devlist,inst->entry);

                inst->devnode = (struct DevListNode *) inst->devlist.mlh_Head;

                if (inst->devnode->node.mln_Succ == NULL)
                        inst->devnode = NULL;

                if (!inst->devnode)
                {
                        if ((inst->entry = NextDosEntry(inst->entry,LDF_ASSIGNS)))
                                return(TRUE);
                        else
                                return(FALSE);
                }

                return(TRUE);
        }


        if (msg->Pos == SCCV_Icon_Entry_Next)
        {
                if (inst->devnode)
                {
                        inst->devnode = (struct DevListNode *) inst->devnode->node.mln_Succ;
                        if (inst->devnode->node.mln_Succ == NULL)
                                inst->devnode = NULL;
                }

                if (!inst->devnode)
                {
                        if ((inst->entry = NextDosEntry(inst->entry,LDF_ASSIGNS)))
                                return(TRUE);
                        else
                                return(FALSE);
                }

                return(TRUE);
        }

        if (msg->Pos == SCCV_Icon_Entry_None)
        {
                FreeAllNodes((struct MinList *) &inst->devlist);
                UnLockDosList(LDF_ALL | LDF_READ);
                return(TRUE);
        }
        return(FALSE);
}

// /

/** Icon_GetObject
*/

static BOOL devstrcmp(BSTR bname1, char *name2)
{
        char *name1 = &((char *) BADDR(bname1))[1];
        int i;

        for(i = 0; (i == ((char *) BADDR(bname1))[0]) || (name1[i] == ':') || (name1[i] == 0) || (name2[i] == ':') || (name2[i] == 0);i++)
        {
                if (ToUpper(name1[i]) != ToUpper(name2[i]))
                        return(FALSE);
        }
        return(TRUE);
}


static Object *DOSListIcon_GetObject(struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetObject *msg, struct DOSListIconInst *inst)
{
        char *volname = NULL;
        char *devname = NULL;
        struct DosList *dlist;
        struct DevListNode *devnode;
        struct DevListNode *devnodevol = NULL;
        struct DevListNode *devnodedev = NULL;
        struct DevListNode *devnodeall = NULL;
        struct MinList list;
        Object *retobj = NULL;

        if ((msg->taglist == NULL) && (inst->entry || inst->devnode))
        {
                if (inst->devnode)
                {
                                // volume/device
                        struct MsgPort *handler;

                        if (inst->devnode->volume)
                        {
                                volname = &((char *) BADDR(inst->devnode->volume->dol_Name))[1];
                                handler = inst->devnode->volume->dol_Task;
                        }
                        if (inst->devnode->device)
                        {
                                devname = &((char *) BADDR(inst->devnode->device->dol_Name))[1];
                                handler = inst->devnode->volume->dol_Task;
                        }

                        return(SC_NewObject(NULL,SCC_DOSDEVICEICON_NAME,
                                                                SCCA_Icon_Name,volname,
                                                                SCCA_DOSDeviceIcon_DeviceName,devname,
                                                                SCCA_DOSDeviceIcon_Handler,handler,
                                                                TAG_DONE));
                }
                else
                {
                                // assign
                        if (inst->entry)
                                return(SC_NewObject(NULL,SCC_DOSDEVICEICON_NAME,
                                                                        SCCA_Icon_Name,&((char *) BADDR(inst->entry->dol_Name))[1],
                                                                        SCCA_DOSDeviceIcon_AssignType,inst->entry->dol_Type,
                                                                        TAG_DONE));
                        else
                                return(NULL);
                }
        }
        else
        {
                volname = (char *) GetTagData(SCCA_Icon_Name,0,msg->taglist);
                devname = (char *) GetTagData(SCCA_DOSDeviceIcon_DeviceName,0,msg->taglist);

                if ((volname == NULL) && (devname == NULL))
                        return(NULL);

                dlist = LockDosList(LDF_ALL | LDF_READ);

                NewList((struct List *) &list);

                GenerateVolDevList(&list,dlist);

                for (devnode = (struct DevListNode *) list.mlh_Head; ((struct MinNode *) devnode)->mln_Succ; devnode = (struct DevListNode *) ((struct MinNode *) devnode)->mln_Succ)
                {
                        if (volname && devnode->volume && devstrcmp(devnode->volume->dol_Name,volname))
                        {
                                devnodevol = devnode;
                                if (devname && devnode->device && devstrcmp(devnode->device->dol_Name,devname))
                                {
                                        devnodeall = devnode;
                                }
                        }
                        else
                        {
                                if (devname && devnode->device && devstrcmp(devnode->device->dol_Name,devname))
                                {
                                        devnodedev = devnode;
                                }
                        }
                }

                if (!devnodeall)
                {
                        if (devnodedev)
                                devnodeall = devnodedev;
                        else
                                devnodeall = devnodevol;
                }

                if (devnodeall)
                {
                        struct MsgPort *handler;
                        volname = devname = NULL;

                        if (devnodeall->volume)
                        {
                                volname = &((char *) BADDR(devnodeall->volume->dol_Name))[1];
                                handler = devnodeall->volume->dol_Task;
                        }
                        if (devnodeall->device)
                        {
                                devname = &((char *) BADDR(devnodeall->device->dol_Name))[1];
                                handler = devnodeall->volume->dol_Task;
                        }

                        retobj = SC_NewObject(NULL,SCC_DOSDEVICEICON_NAME,
                                                                  SCCA_Icon_Name,volname,
                                                                  SCCA_DOSDeviceIcon_DeviceName,devname,
                                                                  SCCA_DOSDeviceIcon_Handler,handler,
                                                                  TAG_DONE);

                        UnLockDosList(LDF_ALL | LDF_READ);
                        FreeAllNodes(&list);
                        return(retobj);

                }
                else
                {
                        while ((dlist = NextDosEntry(dlist,LDF_ASSIGNS)))
                        {
                                if (devstrcmp(dlist->dol_Name,volname))
                                {
                                        retobj = SC_NewObject(NULL,SCC_DOSDEVICEICON_NAME,
                                                                                  SCCA_Icon_Name,&((char *) BADDR(dlist->dol_Name))[1],
                                                                                  SCCA_DOSDeviceIcon_AssignType,dlist->dol_Type,
                                                                                  TAG_DONE);
                                        UnLockDosList(LDF_ALL | LDF_READ);
                                        return(retobj);
                                }
                        }
                }
        }

        return(NULL);
}
// /

/** Init the object
*/
static ULONG DOSListIcon_Init(struct SC_Class *cl, Object *obj, struct opSet *msg, struct DOSListIconInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                NewList((struct List *) &inst->devlist);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Get one attribute
*/
static ULONG DOSListIcon_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct DOSListIconInst *inst )
{
        if (msg->opg_AttrID == SCCA_Icon_IsList)
        {
                *(msg->opg_Storage) = TRUE;
                return(FALSE);
        }

        if (msg->opg_AttrID == SCCA_Icon_Table)
        {
                Object *table;

                if ((table = SC_NewObject(NULL,SCC_TABLE_NAME,TAG_DONE)))
                {
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_Icon_Name,"Name",SCCV_Table_Type_String,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSDeviceIcon_DeviceName,"Device Name",SCCV_Table_Type_String,0);
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_DOSDeviceIcon_AssignType,"Assign Type",SCCV_Table_Type_Long,0);
                        *(msg->opg_Storage) = (LONG) table;
                        return(TRUE);
                }
        }

        return( SC_DoSuperMethodA(cl, obj, (Msg) msg) );
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData DOSListIconMethods[] =
{
        { SCCM_Icon_Entry,(ULONG) DOSListIcon_Entry, 0, 0, NULL },
        { SCCM_Icon_GetObject,(ULONG) DOSListIcon_GetObject, 0, 0, NULL },
        { SCCM_Init,(ULONG) DOSListIcon_Init, 0, 0, NULL },
        { OM_GET,(ULONG) DOSListIcon_Get, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

