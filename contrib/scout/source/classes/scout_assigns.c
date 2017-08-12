/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

struct AssignsWinData {
    TEXT awd_Title[WINDOW_TITLE_LENGTH];
    APTR awd_AssignTree;
    APTR awd_AssignText;
    APTR awd_AssignCount;
    APTR awd_RemoveButton;
};

struct AssignsCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
};

HOOKPROTONHNO(asstree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct AssignEntry));
}
MakeStaticHook(asstree_conhook, asstree_confunc);

HOOKPROTONHNO(asstree_desfunc, void, struct MUIP_NListtree_DestructMessage *msg)
{
    FreeListEntry(msg->MemPool, &msg->UserData);
}
MakeStaticHook(asstree_deshook, asstree_desfunc);

HOOKPROTONHNO(asstree_dspfunc, void, struct MUIP_NListtree_DisplayMessage *msg)
{
    STATIC CONST_STRPTR empty = "\0";

    if (msg->TreeNode != NULL) {
        struct AssignEntry *ae = msg->TreeNode->tn_User;

        if (stricmp(ae->ae_Type, "ADD_NODE") == 0) {
            msg->Array[0] = ae->ae_Address;
            msg->Array[1] = (STRPTR)empty;
            msg->Array[2] = ae->ae_Path;
        } else {
            msg->Array[0] = ae->ae_Address;
            msg->Array[1] = ae->ae_Name;
            msg->Array[2] = ae->ae_Path;
        }
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtName;
        msg->Array[2] = txtAssignsPath;
        msg->Preparse[0] = (STRPTR)MUIX_B;
        msg->Preparse[1] = (STRPTR)MUIX_B;
        msg->Preparse[2] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(asstree_dsphook, asstree_dspfunc);

STATIC LONG asslist_cmpfunc( const struct Node *n1,
                             const struct Node *n2 )
{
    LONG cmp;
    struct AssignEntry *ae1, *ae2;

    ae1 = (struct AssignEntry *)n1;
    ae2 = (struct AssignEntry *)n2;

    cmp = stricmp(ae1->ae_Name, ae2->ae_Name);
    if (cmp == 0) cmp = stricmp(ae1->ae_RealPath, ae2->ae_RealPath);

    return cmp;
}

HOOKPROTONHNO(asstree_cmpfunc, LONG, struct MUIP_NListtree_CompareMessage *msg)
{
    return asslist_cmpfunc((const struct Node *)msg->TreeNode1->tn_User, (const struct Node *)msg->TreeNode2->tn_User);
}
MakeStaticHook(asstree_cmphook, asstree_cmpfunc);

HOOKPROTONHNO(asstree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg)
{
    struct AssignEntry *ae;

    ae = (struct AssignEntry *)msg->UserData;

    if (ae) {
        return stricmp(msg->User, ae->ae_Name);
    } else {
        return ~0;
    }
}
MakeStaticHook(asstree_findhook, asstree_findfunc);

STATIC void ReceiveList( void (* callback)( struct AssignEntry *ae, void *userData ),
                         void *userData )
{
    struct AssignEntry *ae;

    if ((ae = tbAllocVecPooled(globalPool, sizeof(struct AssignEntry))) != NULL) {
        if (SendDaemon("GetAssList")) {
            while (ReceiveDecodedEntry(ae, sizeof(struct AssignEntry))) {
                callback(ae, userData);
            }
        }

        tbFreeVecPooled(globalPool, ae);
    }
}

STATIC void IterateList( void (* callback)( struct AssignEntry *ae, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct AssignEntry *ae;
    struct FileInfoBlock *fib;

    NewList((struct List *)&tmplist);

    if ((fib = AllocDosObject(DOS_FIB, TAG_DONE)) != NULL) {
        STRPTR tmp;

        if ((tmp = tbAllocVecPooled(globalPool, PATH_LENGTH)) != NULL) {
            struct DosList *dol;

            dol = LockDosList(LDF_READ | LDF_ASSIGNS);

            while ((dol = NextDosEntry(dol, LDF_ASSIGNS)) != NULL) {
                if ((ae = AllocVec(sizeof(struct AssignEntry), MEMF_CLEAR)) != NULL) {
                    struct AssignList *list;

                    ae->ae_Addr = BADDR(dol->dol_Lock);
                    _snprintf(ae->ae_Address, sizeof(ae->ae_Address), ADDRESS_FORMAT, BADDR(dol->dol_Lock));

                    ae->ae_Addr = dol;
                    _snprintf(ae->ae_Address, sizeof(ae->ae_Address), ADDRESS_FORMAT, dol);
                    _snprintf(ae->ae_Name, sizeof(ae->ae_Name), "%b", dol->dol_Name);

                    if (dol->dol_Lock != (BPTR)NULL && Examine(dol->dol_Lock, fib)) {
                        NameFromLock(dol->dol_Lock, tmp, PATH_LENGTH);

                        stccpy(ae->ae_Path, tmp, sizeof(ae->ae_Path));
                        stccpy(ae->ae_RealPath, tmp, sizeof(ae->ae_RealPath));
                        stccpy(ae->ae_Type, "NORMAL", sizeof(ae->ae_Type));
                    } else {
                        switch (dol->dol_Type) {
                            case DLT_LATE:
                                _snprintf(ae->ae_Path, sizeof(ae->ae_Path), "<%s>", dol->dol_misc.dol_assign.dol_AssignName);
                                stccpy(ae->ae_RealPath, (TEXT *)dol->dol_misc.dol_assign.dol_AssignName, sizeof(ae->ae_RealPath));
                                stccpy(ae->ae_Type, "LATE", sizeof(ae->ae_Type));
                                break;

                            case DLT_NONBINDING:
                                _snprintf(ae->ae_Path, sizeof(ae->ae_Path), "[%s]", dol->dol_misc.dol_assign.dol_AssignName);
                                stccpy(ae->ae_RealPath, (TEXT *)dol->dol_misc.dol_assign.dol_AssignName, sizeof(ae->ae_RealPath));
                                stccpy(ae->ae_Type, "PATH", sizeof(ae->ae_Type));
                                break;

                            default:
                                ae->ae_Path[0] = 0x00;
                                ae->ae_RealPath[0] = 0x00;
                                stccpy(ae->ae_Type, "UNKNOWN", sizeof(ae->ae_Type));
                                break;
                        }
                    }


                    if ((list = dol->dol_misc.dol_assign.dol_List) != NULL) {
                        stccpy(ae->ae_Type, "ADD_LIST", sizeof(ae->ae_Type));
                        AddTail((struct List *)&tmplist, (struct Node *)ae);

                        do {
                            if ((ae = AllocVec(sizeof(struct AssignEntry), MEMF_CLEAR)) != NULL) {
                                ae->ae_Addr = (APTR)BADDR(list->al_Lock);
                                _snprintf(ae->ae_Address, sizeof(ae->ae_Address), ADDRESS_FORMAT, BADDR(list->al_Lock));
                                _snprintf(ae->ae_Name, sizeof(ae->ae_Name), "%b", dol->dol_Name);

                                if (list->al_Lock != (BPTR)NULL && Examine(list->al_Lock, fib)) {
                                    NameFromLock(list->al_Lock, tmp, PATH_LENGTH);

                                    if (dol->dol_Type == DLT_DIRECTORY) {
                                        stccpy(ae->ae_Path, tmp, sizeof(ae->ae_RealPath));
                                        stccpy(ae->ae_RealPath, tmp, sizeof(ae->ae_RealPath));
                                    }
                                } else {
                                    ae->ae_Path[0] = 0x00;
                                    ae->ae_RealPath[0] = 0x00;
                                }

                                stccpy(ae->ae_Type, "ADD_NODE", sizeof(ae->ae_Type));

                                AddTail((struct List *)&tmplist, (struct Node *)ae);
                            }

                            list = (struct AssignList *)list->al_Next;
                        } while (list);
                    } else {
                        AddTail((struct List *)&tmplist, (struct Node *)ae);
                    }
                }
            }

            UnLockDosList(LDF_READ | LDF_ASSIGNS);

            tbFreeVecPooled(globalPool, tmp);
        }

        FreeDosObject(DOS_FIB, fib);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, asslist_cmpfunc);

    ITERATE_LIST(&tmplist, struct AssignEntry *, ae) {
        callback(ae, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct AssignEntry *ae,
                            void *userData )
{
    struct AssignsCallbackUserData *ud = (struct AssignsCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;
    ULONG flags;

    flags = 0;
    if (stricmp(ae->ae_Type, "ADD_LIST") == 0) flags = TNF_LIST | TNF_OPEN;

    parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, ae->ae_Name, MUIV_NListtree_FindUserData_Flag_StartNode);
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, ae->ae_Name, ae, parent, MUIV_NListtree_Insert_PrevNode_Sorted, flags);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct AssignEntry *ae,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtAssignsPrintLine, ae->ae_Address, ae->ae_Name, ae->ae_Path);
}

STATIC void SendCallback( struct AssignEntry *ae,
                          UNUSED void *userData )
{
    SendEncodedEntry(ae, sizeof(struct AssignEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR asstree, asstext, asscount, updateButton, printButton, removeButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Assigns",
        MUIA_Window_ID, MakeID('A','S','S','I'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&asstree, "BAR,BAR,BAR", &asstree_conhook, &asstree_deshook, &asstree_dsphook, &asstree_cmphook, &asstree_findhook, 2, FALSE),

            Child, (IPTR)MyBelowListview(&asstext, &asscount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(removeButton = MakeButton(txtRemove)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct AssignsWinData *awd = INST_DATA(cl, obj);
        APTR parent;

        awd->awd_AssignTree = asstree;
        awd->awd_AssignText = asstext;
        awd->awd_AssignCount = asscount;
        awd->awd_RemoveButton = removeButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtAssignsTitle, awd->awd_Title, sizeof(awd->awd_Title)));
        set(obj, MUIA_Window_DefaultObject, asstree);
        set(removeButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(asstree,      MUIM_Notify, MUIA_NListtree_Active,    MUIV_EveryTime, obj,                     1, MUIM_AssignsWin_ListChange);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AssignsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AssignsWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_AssignsWin_Remove);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct AssignsWinData *awd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(awd->awd_AssignTree, MUIM_NListtree_Clear, NULL, 0);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct AssignsWinData *awd = INST_DATA(cl, obj);
    struct AssignsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(awd->awd_AssignTree, MUIA_NListtree_Quiet, TRUE);
    DoMethod(awd->awd_AssignTree, MUIM_NListtree_Clear, NULL, 0);

    ud.ud_Tree = awd->awd_AssignTree;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(awd->awd_AssignCount, ud.ud_Count);
    MySetContents(awd->awd_AssignText, "");

    set(awd->awd_AssignTree, MUIA_NListtree_Quiet, FALSE);
    set(awd->awd_RemoveButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintAssigns(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct AssignsWinData *awd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(awd->awd_AssignTree)) != NULL) {
        struct AssignEntry *ae = (struct AssignEntry *)tn->tn_User;

        if (stricmp(ae->ae_Type, "ADD_LIST") == 0 || stricmp(ae->ae_Type, "ADD_NODE") == 0) {
            if (MyRequest(msgYesNo, msgWantToRemoveAssignFromList, ae->ae_Path, ae->ae_Name)) {
                if (MyDoCommand("RemoveAssignList %s " ADDRESS_FORMAT, ae->ae_Name, MKBADDR(ae->ae_Addr)) == RETURN_OK) {
                    DoMethod(obj, MUIM_AssignsWin_Update);
                }
            }
        } else {
            if (MyRequest(msgYesNo, msgWantToRemoveAssign, ae->ae_Name)) {
                if (MyDoCommand("RemoveAssign %s", ae->ae_Name) == RETURN_OK) {
                    DoMethod(obj, MUIM_AssignsWin_Update);
                }
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct AssignsWinData *awd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(awd->awd_AssignTree)) != NULL) {
        struct AssignEntry *ae = (struct AssignEntry *)tn->tn_User;

        MySetContents(awd->awd_AssignText, "%s \"%s\"", ae->ae_Address, ae->ae_Name);
        set(awd->awd_RemoveButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(AssignsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                 return (mDispose(cl, obj, (APTR)msg));
        case MUIM_AssignsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_AssignsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_AssignsWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_AssignsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintAssigns( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtAssignsPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendAssList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeAssignsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct AssignsWinData), ENTRY(AssignsWinDispatcher));
}

