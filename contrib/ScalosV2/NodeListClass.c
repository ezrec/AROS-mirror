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
#include <proto/exec.h>
#include <string.h>
#include <exec/memory.h>

#include "Scalos.h"
#include "NodeListClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** NodeList.scalos/--background ****************************************
*
*   The node list class was made to build an standard interface of lists and
*   to make the work with list easier.
*
*****************************************************************************
*/
// /

/****** NodeList.scalos/SCCA_NodeList_StringList ****************************
*
*  NAME
*   SCCA_NodeList_StringList -- (V40) I.. (BOOL)
*
*  FUNCTION
*   To generate a list of strings it's enough to set this attribute to TRUE.
*   All elements are now strings. They will be copied automatically.
*
*****************************************************************************
*/
// /
/****** NodeList.scalos/SCCA_NodeList_Argument ******************************
*
*  NAME
*   SCCA_NodeList_Argument -- (V40) ..G (ULONG) (char *)
*
*  FUNCTION
*   This attribute is used to get the argument from the current selected
*   entry. If the List is a string list a pointer to the string will be
*   return.
*
*****************************************************************************
*/
// /

/** Init a NodeList
*/
static ULONG NodeList_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct NodeListInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                NewList((struct List *) &inst->list);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Exit a NodeList
*/
static ULONG NodeList_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct NodeListInst *inst)
{
        FreeAllNodes(&inst->list);
        return(SC_DoSuperMethodA(cl,obj, (Msg) msg));
}
// /

/****** NodeList.scalos/SCCM_NodeList_Insert ********************************
*
*  NAME
*   SCCM_NodeList_Insert
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_NodeList_Insert, ULONG arg, ULONG pos);
*
*  FUNCTION
*   This method is used to insert new items to the list. The new entry will
*   inserted relative to the current selected entry, according to the pos
*   argument.
*
*  INPUTS
*   arg - the argument for this entry, if this is a string list the arg is
*         from type (char *)
*   pos - available values are:
*           SCCV_NodeList_Insert_Before - insert the new entry before the
*                                         current entry or as first entry
*                                         if none is selected
*           SCCV_NodeList_Insert_After - insert the new entry after the
*                                        current entry or as last entry if
*                                        none is selected
*
*  RESULT
*   TRUE if adding was successfully, else FALSE.
*
*  NOTES
*
*  SEE ALSO
*   SCCM_NodeList_Remove
*
*****************************************************************************
*/
static ULONG NodeList_Insert(struct SC_Class *cl, Object *obj, struct SCCP_NodeList_Insert *msg, struct NodeListInst *inst)
{
        struct NodeListNode *node;

        if (inst->stringlist)
        {
                if ((node = (struct NodeListNode *) AllocVec(sizeof(struct MinNode) + strlen((char *) msg->arg) + 1,MEMF_ANY)))
                {
                        strcpy((char *) &node->arg, (char *) msg->arg);
                }
                else
                        return(FALSE);
        }
        else
        {
                if ((node = (struct NodeListNode *) AllocVec(sizeof(struct NodeListNode),MEMF_ANY)))
                {
                        node->arg = msg->arg;
                }
                else
                        return(FALSE);
        }


        if (msg->pos == SCCV_NodeList_Insert_Before)
        {
                if (inst->curnode && (inst->curnode->node.mln_Pred->mln_Pred != NULL))
                {
                        Insert((struct List *) &inst->list, (struct Node *) node,(struct Node *) inst->curnode->node.mln_Pred);
                        return(TRUE);
                }
                else
                {
                        AddHead((struct List *) &inst->list, (struct Node *) node);
                        return(TRUE);
                }
        }

        if (msg->pos == SCCV_NodeList_Insert_After)
        {
                if (inst->curnode)
                {
                        Insert((struct List *) &inst->list, (struct Node *) node, (struct Node *) inst->curnode);
                        return(TRUE);
                }
                else
                {
                        AddTail((struct List *) &inst->list, (struct Node *) node);
                        return(TRUE);
                }
        }

        return(FALSE);
}
// /

/****** NodeList.scalos/SCCM_NodeList_Remove ********************************
*
*  NAME
*   SCCM_NodeList_Remove
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_NodeList_Remove);
*
*  FUNCTION
*   This method remove the current selected entry from the list.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static void NodeList_Rem(struct SC_Class *cl, Object *obj, Msg msg, struct NodeListInst *inst)
{
        struct NodeListNode *node = inst->curnode;

        if (node)
        {
                if (node->node.mln_Succ->mln_Succ == NULL)
                        inst->curnode = NULL;
                else
                        inst->curnode = (struct NodeListNode *) node->node.mln_Succ;
        }

        FreeNode(node);
}
// /

/****** NodeList.scalos/SCCM_NodeList_Entry *********************************
*
*  NAME
*   SCCM_NodeList_Entry
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_NodeList_Entry, ULONG Pos);
*
*  FUNCTION
*   This method selectes a current entry. All attributes are relative to
*   this. Preselected is the first entry.
*
*  INPUTS
*   Pos - one of these values
*           SCCV_NodeList_Entry_First - select first entry
*           SCCV_NodeList_Entry_Last - select last entry
*           SCCV_NodeList_Entry_Next - select next entry
*           SCCV_NodeList_Entry_Previous - select previous entry
*
*  RESULT
*   TRUE if the new entry was successfully selected.
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static ULONG NodeList_Entry(struct SC_Class *cl, Object *obj, struct SCCP_NodeList_Entry *msg, struct NodeListInst *inst)
{
        if (msg->Pos == SCCV_NodeList_Entry_Next)
        {
                if (inst->curnode)
                {
                        inst->curnode = (struct NodeListNode *) inst->curnode->node.mln_Succ;
                        if (inst->curnode->node.mln_Succ != 0)
                                return(TRUE);
                }
        }

        if (msg->Pos == SCCV_NodeList_Entry_Previous)
        {
                if (inst->curnode)
                {
                        inst->curnode = (struct NodeListNode *) inst->curnode->node.mln_Pred;
                        if (inst->curnode->node.mln_Pred != 0)
                                return(TRUE);
                }
        }

        if ((msg->Pos == SCCV_NodeList_Entry_First) && !(IsListEmpty((struct List *) &inst->list)))
        {
                inst->curnode = (struct NodeListNode *) inst->list.mlh_Head;
                return(TRUE);
        }

        if ((msg->Pos == SCCV_NodeList_Entry_Last) && !(IsListEmpty((struct List *) &inst->list)))
        {
                inst->curnode = (struct NodeListNode *) inst->list.mlh_Tail;
                return(TRUE);
        }

        return(FALSE);
};
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG NodeList_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct NodeListInst *inst )
{
        if (inst->curnode)
        {
                if (msg->opg_AttrID == SCCA_NodeList_Argument)
                {
                        if (inst->stringlist)
                        {
                                *(msg->opg_Storage) = (ULONG) &inst->curnode->arg;
                        }
                        else
                        {
                                *(msg->opg_Storage) = inst->curnode->arg;
                        }
                        return(TRUE);
                }
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData NodeListMethods[] =
{
        { SCCM_NodeList_Insert,(ULONG) NodeList_Insert, sizeof(struct SCCP_NodeList_Insert), 0, NULL },
        { SCCM_NodeList_Remove,(ULONG) NodeList_Rem, sizeof(ULONG), 0, NULL },
        { SCCM_NodeList_Entry,(ULONG) NodeList_Entry, sizeof(struct SCCP_NodeList_Entry), 0, NULL },
        { SCCM_Init,(ULONG) NodeList_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) NodeList_Exit, 0, 0, NULL },
        { OM_GET,(ULONG) NodeList_Get, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

