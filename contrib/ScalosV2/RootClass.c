// tabsize ts=4

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
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/utility_protos.h>
#include <intuition/classusr.h>
#include <exec/memory.h>

#include "Scalos.h"
#include "RootClass.h"
#include "subroutines.h"
#include "Debug.h"

#include "scalos_protos.h"

static ULONG Root_New(struct SC_Class *cl, Object *obj, struct opSet *msg, struct RootInst *inst)
{
        if (inst = AllocVec(((struct SC_Class *) obj)->InstOffset + ((struct SC_Class *) obj)->InstSize + sizeof(struct RootInst), MEMF_CLEAR | MEMF_ANY))
        {
                inst->oclass = ((struct SC_Class *) obj);
                obj = (Object *) (((struct RootInst *) inst) + 1);
                if (SC_DoMethod(obj, SCCM_Init,msg->ops_AttrList))
                {
                        DEBUG2("MakeObject 0x%08lx - %s\n",myRootInst(obj), SCOCLASS(obj)->ClassName);
                        SCOCLASS(obj)->ObjectCount += 1;
                        return((ULONG) obj);
                }
                SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
                FreeVec(inst);
        }
        return(NULL);
}

static ULONG Root_Init(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        inst = myRootInst(obj);

        NewList((struct List *) &inst->objectlist);
        NewList((struct List *) &inst->notilist);
        InitSemaphore(&inst->objectlistsem);
        InitSemaphore(&inst->notilistsem);
        return(TRUE);
}

static ULONG Root_Dispose(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        DEBUG2("DisposeObject 0x%08lx - %s\n",myRootInst(obj), SCOCLASS(obj)->ClassName);
        SC_DoMethod(obj, SCCM_Exit);
        SCOCLASS(obj)->ObjectCount -= 1;
        FreeVec(myRootInst(obj));
        return(0);
}

static ULONG Root_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        struct MinNode *node;

        inst = myRootInst(obj);
        ObtainSemaphore(&inst->objectlistsem);
        while (!(IsListEmpty((struct List *) &inst->objectlist)))
        {
                node = inst->objectlist.mlh_Head;
                Remove((struct Node *) node);
                SC_DoMethod(SCBASEOBJECT(node), OM_DISPOSE);
        }
        ReleaseSemaphore(&inst->objectlistsem);
        ObtainSemaphore(&inst->notilistsem);
        FreeAllNodes(&inst->notilist);
        ReleaseSemaphore(&inst->notilistsem);
        return(0);
}

static ULONG Root_AddMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct RootInst *inst)
{
        inst = myRootInst(obj);

        ObtainSemaphore(&inst->objectlistsem);
        AddTail((struct List *) &inst->objectlist, (struct Node *) &_SCOBJECT(msg->opam_Object)->sco_Node);
        ReleaseSemaphore(&inst->objectlistsem);
        return(0);
}

static ULONG Root_RemMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct RootInst *inst)
{
        inst = myRootInst(obj);

        ObtainSemaphore(&inst->objectlistsem);
        Remove((struct Node *) &_SCOBJECT(obj)->sco_Node);
        ReleaseSemaphore(&inst->objectlistsem);
        return(0);
}

static ULONG Root_Notify(struct SC_Class *cl, Object *obj, struct SCCP_Notify *msg, struct RootInst *inst)
{
        struct NotifyNode *buffer;
        ULONG cpsize = (msg->numargs)*sizeof(ULONG) + sizeof(struct NotifyNode) - 3*sizeof(ULONG); // complete parameters length

        inst = myRootInst(obj);
        ObtainSemaphore(&inst->notilistsem);
        if (buffer = (struct NotifyNode *) AllocNode(&inst->notilist,cpsize))
                CopyMem(&msg->TriggerAttr, &buffer->TriggerAttr, cpsize - sizeof(struct MinNode));
        ReleaseSemaphore(&inst->notilistsem);
        return TRUE;
}

static void Root_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct RootInst *inst)
{
        struct  TagItem *tmptaglist = msg->ops_AttrList;
        struct  TagItem **taglist = &tmptaglist;
        struct  TagItem *tag;
        struct  NotifyNode *node;
        ULONG   tmpvalue;
        ULONG   tmparray[10];
        ULONG   *srcarray;
        ULONG   *destarray;
        int     i;

        inst = myRootInst(obj);
        ObtainSemaphoreShared(&inst->notilistsem);
        while(tag = NextTagItem(taglist)) // search in taglist
        {
                // look for a attribute that we should trigger
                for (node = (struct NotifyNode *) inst->notilist.mlh_Head; node->node.mln_Succ; node = (struct NotifyNode *) node->node.mln_Succ)
                {
                        if (tag->ti_Tag == node->TriggerAttr) // if found
                        {
                                if ( !(SC_GetAttr(tag->ti_Tag,obj,&tmpvalue)) || (tag->ti_Data != tmpvalue) ) // if current attribute from object doesn`t exist or the attribute has been changed

                                        /* execute only if attr-value has changed */
                                        if ((node->TriggerVal == SCCV_EveryTime) || (tag->ti_Data == node->TriggerVal)) // SCCV_EveryTime: notify everytime the values changes
                                        {
                                                if (node->arg_method == SCCM_Set)
                                                {
                                                        if (node->arg_value == SCCV_TriggerValue)
                                                                SC_SetAttrs(node->DestObject, node->arg_attr, tag->ti_Data, TAG_DONE);
                                                        else
                                                                SC_SetAttrs(node->DestObject, node->arg_attr, node->arg_value, TAG_DONE); /* AROS AMIGAOS fix */
                                                }
                                                else
                                                {
                                                        srcarray = &node->arg_method;
                                                        if (node->numargs > 10)
                                                                destarray = AllocMem(node->numargs * sizeof(ULONG), MEMF_PUBLIC);
                                                        else
                                                                destarray = (ULONG *) &tmparray;
                                                        for (i = 0; i < node->numargs; i++)
                                                        {
                                                                if (srcarray[i] == SCCV_TriggerValue)
                                                                        destarray[i] = tag->ti_Data;
                                                                else
                                                                        destarray[i] = srcarray[i];
                                                        }
                                                        SC_DoMethodA(node->DestObject, (Msg) destarray);
                                                        if (node->numargs > 10)
                                                                FreeMem(destarray, node->numargs * sizeof(ULONG));
                                                }
                                        }
                        }
                }
        }
        ReleaseSemaphore(&inst->notilistsem);
}

static struct MinList *Root_LockObjectList(struct SC_Class *cl, Object *obj, struct SCCP_LockObjectList *msg, struct RootInst *inst)
{
        inst = myRootInst(obj);
        switch (msg->locktype)
        {
                case SCCV_LockExclusive :
                        ObtainSemaphore(&inst->objectlistsem);
                        return(&inst->objectlist);

                case SCCV_LockShared :
                        ObtainSemaphoreShared(&inst->objectlistsem);
                        return(&inst->objectlist);

                case SCCV_LockAttempt :
                        if (AttemptSemaphore(&inst->objectlistsem))
                                return(&inst->objectlist);
                        else
                                return(NULL);

                case SCCV_LockAttemptShared :
                        if (AttemptSemaphoreShared(&inst->objectlistsem))
                                return(&inst->objectlist);
                        else
                                return(NULL);
        }
}

static void Root_UnlockObjectList(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        inst = myRootInst(obj);
        ReleaseSemaphore(&inst->objectlistsem);
}

/****** Root.scalos/SCCM_Clone **********************************************
*
*  NAME
*   SCCM_Clone
*
*  SYNOPSIS
*   Object *SC_DoMethod(obj,SCCM_Clone);
*
*  FUNCTION
*   This method will make a copy of the object. The root class will only
*   generate a new instance and the subclasses will copy all datas from the
*   original to the cloned object.
*   The list of subobjects and notifies will not be copied.
*
*  INPUTS
*
*  RESULT
*   the cloned object
*
*  NOTES
*   If you implement this method in your class, don't copy pointers!
*   Allocated data have to be newly allocated to generate full working
*   object.
*
*  SEE ALSO
*
*****************************************************************************
*/
static Object *Root_Clone(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        Object *newobj;
        struct RootInst *newinst;
        inst = myRootInst(obj);

        if (newinst = AllocVec(inst->oclass->InstOffset + inst->oclass->InstSize + sizeof(struct RootInst), MEMF_CLEAR | MEMF_ANY))
        {
                newinst->oclass = inst->oclass;
                newobj = (Object *) (newinst + 1);
                NewList((struct List *) &newinst->objectlist);
                NewList((struct List *) &newinst->notilist);
                InitSemaphore(&newinst->objectlistsem);
                InitSemaphore(&newinst->notilistsem);
                return(newobj);
        }
        return(NULL);
}

static ULONG Root_Default(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
        return(0);
}

struct SC_MethodData RootMethods[] = 
{
        { OM_NEW, (ULONG) Root_New, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
        { OM_DISPOSE, (ULONG) Root_Dispose, sizeof(ULONG), 0, NULL },
        { OM_SET, (ULONG) Root_Set, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
        { OM_GET, NULL, sizeof(struct opGet), SCMDF_FULLFLUSH, NULL },
        { OM_ADDMEMBER, (ULONG) Root_AddMember, sizeof(struct opMember), 0, NULL },
        { OM_REMMEMBER, (ULONG) Root_RemMember, sizeof(struct opMember), 0, NULL },
        { SCCM_Init, (ULONG) Root_Init, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
        { SCCM_Exit, (ULONG) Root_Exit, sizeof(ULONG), 0, NULL },
        { SCCM_Notify, (ULONG) Root_Notify, sizeof(struct SCCP_Notify) + sizeof(ULONG)*32, 0, NULL },   // maximal 32 args with notifies
        { SCCM_LockObjectList, (ULONG) Root_LockObjectList, sizeof(struct SCCP_LockObjectList), 0, NULL },
        { SCCM_UnlockObjectList, (ULONG) Root_UnlockObjectList, sizeof(ULONG), 0, NULL },
        { SCCM_Clone, (ULONG) Root_Clone, sizeof(ULONG), SCMDF_FULLFLUSH, NULL },
        { SCMETHOD_DONE, (ULONG) Root_Default, 0, 0, NULL }
};

