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
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <string.h>

#include "Scalos.h"
#include "AmigaIconClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

static struct MinList AmigaIconList;
static struct SignalSemaphore AmigaIconSem;

/****** AmigaIcon.scalos/--background ***************************************
*
*   AmigaIcon class objects are the roots of all other icons. Icon tree:
*
*   AmigaIcon
*     +--AmigaDOSIcon (Devices, Assigns, Volumnes)
*     |  +--DOSDeviceIcon
*     |     +--DOSDirIcon
*     |     +--DOSFileIcon
*     +--AppServerIcon
*        +--AppIcon
*
*
*
*****************************************************************************
*/
// /

/****** AmigaIcon.scalos/SCCM_AmigaIcon_Add *********************************
*
*  NAME
*   SCCM_AmigaIcon_Add
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_AmigaIcon_Add, struct SC_Class *class, char *classname, char *name);
*   ULONG SC_DoClassMethod(NULL,SCC_AMIGAICON_NAME,SCCM_AmigaIcon_Add, struct SC_Class *class, char *classname, char *name);
*
*  FUNCTION
*   With this method a new child of AmigaIcon can be added. The class will be
*   used to generate child objects. Adding a new entry will automatically
*   select entry "None".
*
*  INPUTS
*   class - pointer to a SC_Class structure or NULL if classname is valid
*   classname - pointer to a name of a class or NULL if class is valid
*   name - a name to identify the child
*
*  RESULT
*   TRUE if the class was successfully added
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG AmigaIcon_Add(struct SC_Class *cl, Object *obj, struct SCCP_AmigaIcon_Add *msg, struct AmigaIconInst *inst)
{
        struct AmigaIconNode *node;
        ULONG ret = FALSE;

        if (obj)
                SC_DoMethod(obj,SCCM_Icon_Entry,SCCV_Icon_Entry_None);

        ObtainSemaphore(&AmigaIconSem);
        if (node = (struct AmigaIconNode *) AllocNode((struct MinList *) &AmigaIconList,sizeof(struct AmigaIconNode)))
        {
                 node->class = msg->class;
                 node->classname = msg->classname;
                 node->name = msg->name;
                 ret = TRUE;
        }
        ReleaseSemaphore(&AmigaIconSem);
        return(ret);
}
// /

/****** AmigaIcon.scalos/SCCM_AmigaIcon_Remove ******************************
*
*  NAME
*   SCCM_AmigaIcon_Remove
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_AmigaIcon_Remove, struct SC_Class *class, char *classname);
*   ULONG SC_DoClassMethod(NULL,SCC_AMIGAICON_NAME,SCCM_AmigaIcon_Remove, struct SC_Class *class, char *classname);
*
*  FUNCTION
*   This method will remove a child from AmigaIcon. Removing an entry will
*   automatically select entry "None".
*
*  INPUTS
*   class - pointer to a SC_Class structure or NULL if classname is valid
*   classname - pointer to a name of a class or NULL if class is valid
*
*  RESULT
*   TRUE if the class was successfully removed
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG AmigaIcon_Remove(struct SC_Class *cl, Object *obj, struct SCCP_AmigaIcon_Remove *msg, struct AmigaIconInst *inst)
{
        struct AmigaIconNode *node;

        if (obj)
                SC_DoMethod(obj,SCCM_Icon_Entry,SCCV_Icon_Entry_None);

        ObtainSemaphore(&AmigaIconSem);

        for (node = (struct AmigaIconNode *) AmigaIconList.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct AmigaIconNode *) ((struct MinNode *) node)->mln_Succ)
        {
                if ((msg->class && (msg->class == node->class))
                        || (msg->classname && (strcmp(msg->classname,node->classname) == 0)))
                {
                        FreeNode((struct MinNode *) node);
                        ReleaseSemaphore(&AmigaIconSem);
                        return(TRUE);
                }

        }
        ReleaseSemaphore(&AmigaIconSem);
        return(FALSE);
}
// /

/** Icon_Entry
*/
static ULONG AmigaIcon_Entry(struct SC_Class *cl, Object *obj, struct SCCP_Icon_Entry *msg, struct AmigaIconInst *inst)
{
        if (!(IsListEmpty((struct List *) &AmigaIconList)))
        {
                if ((inst->entry == NULL) && (msg->Pos != SCCV_Icon_Entry_None))
                        ObtainSemaphoreShared(&AmigaIconSem);

                if (msg->Pos == SCCV_Icon_Entry_First)
                {
                        inst->entry = (struct AmigaIconNode *) AmigaIconList.mlh_Head;
                        return(TRUE);
                }

                if (msg->Pos == SCCV_Icon_Entry_Last)
                {
                        inst->entry = (struct AmigaIconNode *) AmigaIconList.mlh_Tail;
                        return(TRUE);
                }

                if (inst->entry)
                {
                        if (msg->Pos == SCCV_Icon_Entry_Next)
                        {
                                inst->entry = (struct AmigaIconNode *) inst->entry->node.mln_Succ;
                                if (inst->entry->node.mln_Succ != 0)
                                        return(TRUE);
                                else
                                {
                                        inst->entry = NULL;
                                        ReleaseSemaphore(&AmigaIconSem);
                                        return(FALSE);
                                }
                        }

                        if (msg->Pos == SCCV_Icon_Entry_Previous)
                        {
                                inst->entry = (struct AmigaIconNode *) inst->entry->node.mln_Pred;
                                if (inst->entry->node.mln_Pred != 0)
                                        return(TRUE);
                                else
                                {
                                        inst->entry = NULL;
                                        ReleaseSemaphore(&AmigaIconSem);
                                        return(FALSE);
                                }
                        }
                }

                if (msg->Pos == SCCV_Icon_Entry_None)
                {
                        inst->entry = NULL;
                        ReleaseSemaphore(&AmigaIconSem);
                        return(TRUE);
                }
        }
        return(FALSE);

}
// /

/** Icon_GetObject
*/
static Object *AmigaIcon_GetObject(struct SC_Class *cl, Object *obj, struct SCCP_Icon_GetObject *msg, struct AmigaIconInst *inst)
{
        char *name;

        if (name = (char *) GetTagData(SCCA_Icon_Name,0,msg->taglist))
        {
                struct AmigaIconNode *node;
                for (node = (struct AmigaIconNode *) AmigaIconList.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct AmigaIconNode *) ((struct MinNode *) node)->mln_Succ)
                {
                        if (strcmp(name,node->name) == 0)
                                return(SC_NewObjectA(node->class,node->classname,NULL));
                }
        }
        else
        {
                if (inst->entry)
                        return(SC_NewObjectA(inst->entry->class,inst->entry->classname,NULL));
        }
        return(NULL);
}
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG AmigaIcon_Get(struct SC_Class *cl, Object *obj, struct opGet *msg, struct AmigaIconInst *inst)
{
        if (msg->opg_AttrID == SCCA_Icon_Table)
        {
                Object *table;

                if (table = SC_NewObject(NULL,SCC_TABLE_NAME,TAG_DONE))
                {
                        SC_DoMethod(table,SCCM_Table_Add,SCCA_Icon_Name,"Name",SCCV_Table_Type_String,0);
                        *(msg->opg_Storage) = (LONG) table;
                        return(TRUE);
                }
        }

        if (msg->opg_AttrID == SCCA_Icon_IsList)
        {
                *(msg->opg_Storage) = TRUE;
                return(TRUE);
        }

        if (msg->opg_AttrID == SCCA_Icon_Name)
        {
                if (inst->entry)
                        *(msg->opg_Storage) = (ULONG) inst->entry->name;
                else
                        *(msg->opg_Storage) = (ULONG) "Amiga";
                return(TRUE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData AmigaIconMethods[] =
{
        { SCCM_AmigaIcon_Add,(ULONG) AmigaIcon_Add, sizeof(struct SCCP_AmigaIcon_Add), 0, NULL },
        { SCCM_AmigaIcon_Remove,(ULONG) AmigaIcon_Remove, sizeof(struct SCCP_AmigaIcon_Remove), 0, NULL },
        { SCCM_Icon_Entry,(ULONG) AmigaIcon_Entry, 0, 0, NULL },
        { SCCM_Icon_GetObject,(ULONG) AmigaIcon_GetObject, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

