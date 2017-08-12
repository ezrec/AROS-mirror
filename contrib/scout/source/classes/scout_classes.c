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

struct ClassesWinData {
    TEXT cwd_Title[WINDOW_TITLE_LENGTH];
    APTR cwd_ClassTree;
    APTR cwd_ClassText;
    APTR cwd_ClassCount;
    APTR cwd_RemoveButton;
    APTR cwd_MoreButton;
};

struct ClassesCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
};

HOOKPROTONHNO(classtree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct ClassEntry));
}
MakeStaticHook(classtree_conhook, classtree_confunc);

HOOKPROTONHNO(classtree_desfunc, void, struct MUIP_NListtree_DestructMessage *msg)
{
    FreeListEntry(msg->MemPool, &msg->UserData);
}
MakeStaticHook(classtree_deshook, classtree_desfunc);

HOOKPROTONHNO(classtree_dspfunc, void, struct MUIP_NListtree_DisplayMessage *msg)
{
    if (msg->TreeNode != NULL) {
        struct ClassEntry *ce = msg->TreeNode->tn_User;

        msg->Array[0] = ce->ce_Address;
        msg->Array[1] = ce->ce_ClassName;
        msg->Array[2] = ce->ce_ObjectCount;
        msg->Array[3] = ce->ce_SubClassCount;
        msg->Array[4] = ce->ce_Dispatcher;
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtClassID;
        msg->Array[2] = txtClassObjects;
        msg->Array[3] = txtClassSubclasses;
        msg->Array[4] = txtClassDispatcher;
        msg->Preparse[0] = (STRPTR)MUIX_B;
        msg->Preparse[1] = (STRPTR)MUIX_B;
        msg->Preparse[2] = (STRPTR)MUIX_B;
        msg->Preparse[3] = (STRPTR)MUIX_B;
        msg->Preparse[4] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(classtree_dsphook, classtree_dspfunc);

HOOKPROTONHNO(classtree_cmpfunc, LONG, struct MUIP_NListtree_CompareMessage *msg)
{
    struct ClassEntry *ce1, *ce2;
    LONG cmp;

    ce1 = (struct ClassEntry *)msg->TreeNode1->tn_User;
    ce2 = (struct ClassEntry *)msg->TreeNode2->tn_User;

    cmp = stricmp(ce1->ce_SuperClassName, ce2->ce_SuperClassName);
    if (cmp == 0) cmp = stricmp(ce1->ce_ClassName, ce2->ce_ClassName);

    return cmp;
}
MakeStaticHook(classtree_cmphook, classtree_cmpfunc);

HOOKPROTONHNO(classtree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg )
{
    struct ClassEntry *ce;

    ce = (struct ClassEntry *)msg->UserData;

    if (ce) {
        return stricmp(msg->User, ce->ce_Address);
    } else {
        return ~0;
    }
}
MakeStaticHook(classtree_findhook, classtree_findfunc);

STATIC void ReceiveList( void (* callback)( struct ClassEntry *ce, void *userData ),
                         void *userData )
{
    struct ClassEntry *ce;

    if ((ce = tbAllocVecPooled(globalPool, sizeof(struct ClassEntry))) != NULL) {
        if (SendDaemon("GetClassList")) {
            while (ReceiveDecodedEntry(&ce, sizeof(struct ClassEntry))) {
                callback(ce, userData);
            }
        }

        tbFreeVecPooled(globalPool, ce);
    }
}

STATIC void IterateList( void (* callback)( struct ClassEntry *ce, void *userData ),
                         void *userData )
{
    struct IClass *myclass;

    if ((myclass = MakeClass("« Scout Dummy Class »", "rootclass", NULL, 0, 0)) != NULL) {
        struct List *clist;
        struct IClass *cl;
        struct MinList tmplist;
        struct ClassEntry *ce;

        NewList((struct List *)&tmplist);

        AddClass(myclass);

        Forbid();

        clist = FindListOfNode((struct Node *)&myclass->cl_Dispatcher.h_MinNode);
        ITERATE_LIST(clist, struct IClass *, cl) {
            if (cl != myclass) {
                if ((ce = AllocVec(sizeof(struct ClassEntry), MEMF_CLEAR)) != NULL) {
                    struct Hook *disp;
                    APTR dispentry;

                    disp = &cl->cl_Dispatcher;

                    ce->ce_Addr = cl;
                    _snprintf(ce->ce_Address, sizeof(ce->ce_Address), ADDRESS_FORMAT, cl);
                    if (cl->cl_Super) {
                        stccpy(ce->ce_SuperClassName, (TEXT *)nonetest((STRPTR)cl->cl_Super->cl_ID), sizeof(ce->ce_SuperClassName));
                    } else {
                        stccpy(ce->ce_SuperClassName, (TEXT *)nonetest(NULL), sizeof(ce->ce_SuperClassName));
                    }
                    _snprintf(ce->ce_SuperClassAddress, sizeof(ce->ce_SuperClassAddress), ADDRESS_FORMAT, cl->cl_Super);

                    dispentry = (disp->h_SubEntry) ? (APTR)disp->h_SubEntry : (APTR)disp->h_Entry;
                    if (points2ram(dispentry)) {
                        _snprintf(ce->ce_Dispatcher, sizeof(ce->ce_Dispatcher), MUIX_PH ADDRESS_FORMAT MUIX_PT, dispentry);
                    } else {
                        _snprintf(ce->ce_Dispatcher, sizeof(ce->ce_Dispatcher), ADDRESS_FORMAT, dispentry);
                    }

                    stccpy(ce->ce_ClassName, (TEXT *)nonetest((STRPTR)cl->cl_ID), sizeof(ce->ce_ClassName));
                    _snprintf(ce->ce_ObjectCount, sizeof(ce->ce_ObjectCount), "%6lD", cl->cl_ObjectCount);
                    _snprintf(ce->ce_SubClassCount, sizeof(ce->ce_SubClassCount), "%6lD", cl->cl_SubclassCount);

                    healstring(ce->ce_ClassName);
                    healstring(ce->ce_SuperClassName);

#ifdef __MORPHOS__
		    AddTail((struct List *)&tmplist, (struct Node *)ce);
#else
                    // build the list in reverse order, because we need rootclass to be the very first element afterwards
                    AddHead((struct List *)&tmplist, (struct Node *)ce);
#endif
                }
            }
        }

        Permit();

        ITERATE_LIST(&tmplist, struct ClassEntry *, ce) {
            callback(ce, userData);
        }
        FreeLinkedList((struct List *)&tmplist);

        FreeClass(myclass);
    }
}

STATIC void UpdateCallback( struct ClassEntry *ce,
                            UNUSED void *userData )
{
    struct ClassesCallbackUserData *ud = (struct ClassesCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;
    ULONG flags;
    SIPTR subclasses;

    IsDec(ce->ce_SubClassCount, &subclasses);

    flags = 0;
    if (subclasses > 0) flags = TNF_LIST | TNF_OPEN;

    parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, ce->ce_SuperClassAddress, MUIV_NListtree_FindUserData_Flag_StartNode);
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, ce->ce_ClassName, ce, parent, MUIV_NListtree_Insert_PrevNode_Sorted, flags);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ClassEntry *ce,
                           void *userData )
{
    if (strncmp(ce->ce_Dispatcher, MUIX_PH, strlen(MUIX_PH)) == 0) {
        PrintFOneLine((BPTR)userData, txtClassesPrintLine, ce->ce_Address, ce->ce_ObjectCount, ce->ce_SubClassCount, ce->ce_SuperClassName, ce->ce_Dispatcher + 2, ce->ce_ClassName);
    } else {
        PrintFOneLine((BPTR)userData, txtClassesPrintLine, ce->ce_Address, ce->ce_ObjectCount, ce->ce_SubClassCount, ce->ce_SuperClassName, ce->ce_Dispatcher, ce->ce_ClassName);
    }
}

STATIC void SendCallback( struct ClassEntry *ce,
                          UNUSED void *userData )
{
    SendEncodedEntry(ce, sizeof(struct ClassEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR classtree, classtext, classcount, updateButton, printButton, removeButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "BoopsiClasses",
        MUIA_Window_ID, MakeID('C','L','A','S'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&classtree, "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR", &classtree_conhook, &classtree_deshook, &classtree_dsphook, &classtree_cmphook, &classtree_findhook, 1, FALSE),

            Child, (IPTR)MyBelowListview(&classtext, &classcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(removeButton = MakeButton(txtRemove)),
                Child, (IPTR)(moreButton   = MakeButton(txtMore)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ClassesWinData *cwd = INST_DATA(cl, obj);
        APTR parent;

        cwd->cwd_ClassTree = classtree;
        cwd->cwd_ClassText = classtext;
        cwd->cwd_ClassCount = classcount;
        cwd->cwd_RemoveButton = removeButton;
        cwd->cwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtClassesTitle, cwd->cwd_Title, sizeof(cwd->cwd_Title)));
        set(obj, MUIA_Window_DefaultObject, classtree);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, cwd->cwd_RemoveButton,
                                                          cwd->cwd_MoreButton,
                                                          NULL);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest,   TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(classtree,    MUIM_Notify, MUIA_NListtree_Active,      MUIV_EveryTime, obj,                     1, MUIM_ClassesWin_ListChange);
        DoMethod(classtree,    MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime, obj,                     1, MUIM_ClassesWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_ClassesWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_ClassesWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_ClassesWin_Remove);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_ClassesWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ClassesWinData *cwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cwd->cwd_ClassTree, MUIM_NListtree_Clear, NULL, 0);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ClassesWinData *cwd = INST_DATA(cl, obj);
    struct ClassesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(cwd->cwd_ClassTree, MUIA_NListtree_Quiet, TRUE);
    DoMethod(cwd->cwd_ClassTree, MUIM_NListtree_Clear, NULL, 0);

    ud.ud_Tree = cwd->cwd_ClassTree;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(cwd->cwd_ClassCount, ud.ud_Count);
    MySetContents(cwd->cwd_ClassText, "");

    set(cwd->cwd_ClassTree, MUIA_NListtree_Quiet, FALSE);
    set(cwd->cwd_ClassTree, MUIA_NListtree_Active, MUIV_NListtree_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, cwd->cwd_RemoveButton,
                                                      cwd->cwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintClass(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ClassesWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
        struct ClassEntry *ce = (struct ClassEntry *)tn->tn_User;

        if (MyRequest(msgYesNo, msgWantToRemoveClass, ce->ce_ClassName)) {
            if (MyDoCommand("RemoveClass %s", ce->ce_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_ClassesWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct ClassesWinData *cwd = INST_DATA(cl, obj);
        struct MUI_NListtree_TreeNode *tn;

        if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
            struct ClassEntry *ce = (struct ClassEntry *)tn->tn_User;
            APTR detailWin;

            if ((detailWin = (Object *)(ClassesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End)) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_ClassesDetailWin_Class, ce);
                set(detailWin, MUIA_Window_Open, TRUE);
                REISSUE_RETURNIDS;
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct ClassesWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
        struct ClassEntry *ce = (struct ClassEntry *)tn->tn_User;

        MySetContents(cwd->cwd_ClassText, "%s \"%s\"", ce->ce_Address, ce->ce_ClassName);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, cwd->cwd_RemoveButton,
                                                           (clientstate) ? NULL : cwd->cwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(ClassesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                      return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                  return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ClassesWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ClassesWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ClassesWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_ClassesWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_ClassesWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintClass( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtClassesPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendClassList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeClassesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ClassesWinData), ENTRY(ClassesWinDispatcher));
}

