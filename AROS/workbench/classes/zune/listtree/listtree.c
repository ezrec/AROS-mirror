/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <clib/alib_protos.h>

#include "Listtree_mcc.h"
#include "listtree_private.h"

#include <aros/debug.h>

/* TEMP CODE */
ULONG Listtree_Active_HookFunc(struct Hook * h, APTR obj, void **msg)
{
    APTR active = NULL;

    active = (APTR)DoMethod(obj, MUIM_Listtree_GetEntry, NULL, MUIV_Listtree_GetEntry_Position_Active, 0);
    set(obj, MUIA_Listtree_Active, active);

    return 0;
}

static struct Hook _Listtree_Active_Hook;
/* TEMP CODE */

/*** Methods ****************************************************************/
Object *Listtree__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct Listtree_DATA *data = NULL;
    struct TagItem *tag;
    const struct TagItem *tags;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj) return FALSE;

    data = INST_DATA(cl, obj);

    NewList((struct List*)&data->nodes);
    data->pool = CreatePool(MEMF_ANY | MEMF_CLEAR, 16 * 1024, 8 * 1024);

    /* parse initial taglist */
    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
        switch (tag->ti_Tag)
        {
        case(MUIA_Listtree_ConstructHook):
            data->constrhook = (struct Hook *)tag->ti_Data;
            break;
        case(MUIA_Listtree_DestructHook):
            data->destrhook = (struct Hook *)tag->ti_Data;
            break;
        }
    }

    /* TEMP CODE */
    _Listtree_Active_Hook.h_Entry = HookEntry;
    _Listtree_Active_Hook.h_SubEntry = (HOOKFUNC)Listtree_Active_HookFunc;
    _Listtree_Active_Hook.h_Data = NULL;

    DoMethod(obj, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, obj, 2, MUIM_CallHook, (IPTR)&_Listtree_Active_Hook);

    /* TEMP CODE */

    return obj;
}

IPTR Listtree__MUIM_Listtree_Insert(struct IClass *cl, Object *obj, struct MUIP_Listtree_Insert *msg)
{
    struct Listtree_DATA *data = INST_DATA(cl, obj);
    struct MUIS_Listtree_TreeNode * _return = AllocPooled(data->pool, sizeof(struct MUIS_Listtree_TreeNode));

    if (_return == NULL)
        return (IPTR)NULL;

    _return->tn_Flags = (UWORD)msg->Flags;
    if (msg->Name)
    {
        LONG len = strlen(msg->Name) + 1;
        _return->tn_Name = AllocPooled(data->pool, len);
        CopyMem(msg->Name, _return->tn_Name, len);
    }

    if (data->constrhook)
        _return->tn_User = (APTR)CallHookPkt(data->constrhook, data->pool, msg->User);
    else
        _return->tn_User = msg->User;

    AddTail((struct List *)&data->nodes, (struct Node *)_return);

    DoMethod(obj, MUIM_List_InsertSingle, _return->tn_Name, MUIV_List_Insert_Bottom);

    return (IPTR)_return;
}

IPTR Listtree__MUIM_Listtree_GetEntry(struct IClass *cl, Object *obj, struct MUIP_Listtree_GetEntry *msg)
{
    struct Listtree_DATA *data = INST_DATA(cl, obj);
    struct Node * node = NULL;
    ULONG counter = 0;

    if (msg->Position == MUIV_Listtree_GetEntry_Position_Active)
    {
        IPTR active = 0;

        get(obj, MUIA_List_Active, &active);

        ForeachNode(&data->nodes, node)
        {
            if (counter == active)
                return (IPTR)node;
            counter++;
        }
    }

    if ((msg->Node == MUIV_Listtree_GetEntry_ListNode_Root) && (msg->Flags & MUIV_Listtree_GetEntry_Flags_SameLevel))
    {
        ForeachNode(&data->nodes, node)
        {
            if (counter == msg->Position)
                return (IPTR)node;
            counter++;
        }
    }

    /* This probably has different "stop" condition than the one with SameLevel flag if in real tree. Since
     * this is a list, the code is the same
     */
    if ((msg->Node == MUIV_Listtree_GetEntry_ListNode_Root) && (msg->Flags == 0))
    {
        ForeachNode(&data->nodes, node)
        {
            if (counter == msg->Position)
                return (IPTR)node;
            counter++;
        }
    }

    return (IPTR)NULL;
}

IPTR Listtree__MUIM_Listtree_GetNr(struct IClass *cl, Object *obj, struct MUIP_Listtree_GetNr *msg)
{
    struct Listtree_DATA *data = INST_DATA(cl, obj);
    struct Node * node = NULL;
    ULONG counter = 0;

    ForeachNode(&data->nodes, node)
    {
        if (msg->TreeNode == node)
            return counter;
        counter++;
    }

    return (IPTR)0;
}

IPTR Listtree__MUIM_Listtree_Remove(struct IClass *cl, Object *obj, struct MUIP_Listtree_Remove *msg)
{
    struct Listtree_DATA *data = INST_DATA(cl, obj);
    ULONG counter = 0;
    struct Node *todelete = NULL;

    if (msg->TreeNode == (APTR)MUIV_Listtree_Remove_TreeNode_Active)
    {
        IPTR active = 0;
        struct Node *node;

        get(obj, MUIA_List_Active, &active);

        ForeachNode(&data->nodes, node)
        {
            if (counter == active)
            {
                todelete = node;
                break;
            }
            counter++;
        }

        if (todelete)
        {
            Remove(todelete);
            DoMethod(obj, MUIM_List_Remove, MUIV_List_Remove_Active);
        }
    }

    if (todelete)
    {
        if (data->destrhook)
            CallHookPkt(data->destrhook, data->pool, ((struct MUIS_Listtree_TreeNode *)todelete)->tn_User);
        FreePooled(data->pool, todelete, sizeof(struct MUIS_Listtree_TreeNode));
    }

    return (IPTR)FALSE;
}
