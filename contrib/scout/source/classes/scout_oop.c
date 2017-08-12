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
 * @author Pavel Fedin <pavel.fedin@mail.ru>
 */


#ifdef __AROS__

#include <oop/oop.h>
#include <proto/oop.h>

#endif

#include "system_headers.h"

/* This class operates on AROS-specific data.
   On non-AROS systems oop.library and its classes do not exist,
   however it's possible to retrieve the data via network from the
   remote AROS machine */

#ifdef __AROS__

struct Library *OOPBase;
OOP_AttrBase MetaAttrBase;

#endif

struct OOPWinData {
    TEXT cwd_Title[WINDOW_TITLE_LENGTH];
    APTR cwd_ClassTree;
    APTR cwd_ClassText;
    APTR cwd_ClassCount;
    APTR cwd_RemoveButton;
    APTR cwd_MoreButton;
};

struct OOPCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
};

HOOKPROTONHNO(classtree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct OOPClassEntry));
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
        struct OOPClassEntry *ce = msg->TreeNode->tn_User;

        msg->Array[0] = ce->ce_Address;
        msg->Array[1] = ce->ce_ClassName;
        msg->Array[2] = ce->ce_Dispatcher;
	msg->Array[3] = ce->ce_CoerceDispatcher;
	msg->Array[4] = ce->ce_SuperDispatcher;
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtNodeName;
        msg->Array[2] = txtOOPDoMethod;
	msg->Array[3] = txtOOPCoerceMethod;
	msg->Array[4] = txtOOPDoSuperMethod;
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
    struct OOPClassEntry *ce1, *ce2;
    LONG cmp;

    ce1 = (struct OOPClassEntry *)msg->TreeNode1->tn_User;
    ce2 = (struct OOPClassEntry *)msg->TreeNode2->tn_User;

    cmp = stricmp(ce1->ce_SuperClassName, ce2->ce_SuperClassName);
    if (cmp == 0) cmp = stricmp(ce1->ce_ClassName, ce2->ce_ClassName);

    return cmp;
}
MakeStaticHook(classtree_cmphook, classtree_cmpfunc);

HOOKPROTONHNO(classtree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg )
{
    struct OOPClassEntry *ce;

    ce = (struct OOPClassEntry *)msg->UserData;

    if (ce) {
        return stricmp(msg->User, ce->ce_Address);
    } else {
        return ~0;
    }
}
MakeStaticHook(classtree_findhook, classtree_findfunc);


STATIC void ReceiveList( void (* callback)( struct OOPClassEntry *ce, void *userData ),
                         void *userData )
{
    struct OOPClassEntry *ce;

    if ((ce = tbAllocVecPooled(globalPool, sizeof(struct OOPClassEntry))) != NULL) {
        if (SendDaemon("GetOOPClassList")) {
            while (ReceiveDecodedEntry(&ce, sizeof(struct OOPClassEntry))) {
                callback(ce, userData);
            }
        }

        tbFreeVecPooled(globalPool, ce);
    }
}

STATIC void FormatAddress(STRPTR buf, APTR dispentry)
{
    if (points2ram(dispentry)) {
        _snprintf(buf, ADDRESS_LENGTH, MUIX_PH ADDRESS_FORMAT MUIX_PT, dispentry);
    } else {
        _snprintf(buf, ADDRESS_LENGTH, ADDRESS_FORMAT, dispentry);
    }
}

STATIC void IterateList( void (* callback)( struct OOPClassEntry *ce, void *userData ),
                         void *userData )
{

#ifdef __AROS__

    OOP_Class *myclass;

    struct OOP_InterfaceDescr my_if = {NULL, NULL, 0};
    struct TagItem mytags[] = {
	{aMeta_SuperID       , (IPTR)CLID_Root},
	{aMeta_InterfaceDescr, (IPTR)&my_if   },
	{aMeta_InstSize      , 0              },
	{TAG_DONE            , 0              }
    };

    if ((myclass = OOP_NewObject(NULL, CLID_SIMeta, mytags)) != NULL) {
        struct List *clist;
        OOP_Class *cl;
        struct MinList tmplist;
        struct OOPClassEntry *ce;

        NewList((struct List *)&tmplist);

        OOP_AddClass(myclass);

        Forbid();

        clist = FindListOfNode(&myclass->ClassNode);
        ITERATE_LIST(clist, OOP_Class *, cl) {
            if (cl != myclass) {
                if ((ce = AllocVec(sizeof(struct OOPClassEntry), MEMF_CLEAR)) != NULL) {
                    ce->ce_Addr = cl;
                    _snprintf(ce->ce_Address, sizeof(ce->ce_Address), ADDRESS_FORMAT, cl);
                    if (cl->superclass) {
                        stccpy(ce->ce_SuperClassName, (TEXT *)nonetest((STRPTR)cl->superclass->ClassNode.ln_Name), sizeof(ce->ce_SuperClassName));
                    } else {
                        stccpy(ce->ce_SuperClassName, (TEXT *)nonetest(NULL), sizeof(ce->ce_SuperClassName));
                    }
                    _snprintf(ce->ce_SuperClassAddress, sizeof(ce->ce_SuperClassAddress), ADDRESS_FORMAT, cl->superclass);

		    FormatAddress(ce->ce_Dispatcher, cl->cl_DoMethod);
		    FormatAddress(ce->ce_CoerceDispatcher, cl->cl_CoerceMethod);
		    FormatAddress(ce->ce_SuperDispatcher, cl->cl_DoSuperMethod);

                    stccpy(ce->ce_ClassName, (TEXT *)nonetest(cl->ClassNode.ln_Name), sizeof(ce->ce_ClassName));

                    healstring(ce->ce_ClassName);

                    AddTail((struct List *)&tmplist, (struct Node *)ce);
                }
            }
        }

        Permit();

        ITERATE_LIST(&tmplist, struct OOPClassEntry *, ce) {
            callback(ce, userData);
        }
        FreeLinkedList((struct List *)&tmplist);

	OOP_RemoveClass(myclass);
        OOP_DisposeObject((OOP_Object *)myclass);
    }

#endif

}

STATIC void UpdateCallback( struct OOPClassEntry *ce,
                            UNUSED void *userData )
{
    struct OOPCallbackUserData *ud = (struct OOPCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;

    parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, ce->ce_SuperClassAddress, MUIV_NListtree_FindUserData_Flag_StartNode);
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, ce->ce_ClassName, ce, parent, MUIV_NListtree_Insert_PrevNode_Sorted, TNF_LIST | TNF_OPEN);
}

INLINE STRPTR SkipFormat(STRPTR str)
{
    if (strncmp(str, MUIX_PH, strlen(MUIX_PH)))
        return str;
    else
        return str + 2;
}

STATIC void PrintCallback( struct OOPClassEntry *ce,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtOOPPrintLine, ce->ce_Address, ce->ce_SuperClassName,
		  SkipFormat(ce->ce_Dispatcher), SkipFormat(ce->ce_CoerceDispatcher), SkipFormat(ce->ce_SuperDispatcher), ce->ce_ClassName);
}

STATIC void SendCallback( struct OOPClassEntry *ce,
                          UNUSED void *userData )
{
    SendEncodedEntry(ce, sizeof(struct OOPClassEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR classtree, classtext, classcount, updateButton, printButton, removeButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "OOPClass",
        MUIA_Window_ID, MakeID('O','O','P',' '),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&classtree, "BAR,BAR,BAR,BAR,BAR", &classtree_conhook, &classtree_deshook, &classtree_dsphook, &classtree_cmphook, &classtree_findhook, 1, TRUE),

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
        struct OOPWinData *cwd = INST_DATA(cl, obj);
        APTR parent;

        cwd->cwd_ClassTree = classtree;
        cwd->cwd_ClassText = classtext;
        cwd->cwd_ClassCount = classcount;
        cwd->cwd_RemoveButton = removeButton;
        cwd->cwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtOOPTitle, cwd->cwd_Title, sizeof(cwd->cwd_Title)));
        set(obj, MUIA_Window_DefaultObject, classtree);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, cwd->cwd_RemoveButton,
                                                          cwd->cwd_MoreButton,
                                                          NULL);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest,   TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(classtree,    MUIM_Notify, MUIA_NList_Active,          MUIV_EveryTime, obj,                     1, MUIM_OOPWin_ListChange);
        DoMethod(classtree,    MUIM_Notify, MUIA_NList_DoubleClick,     MUIV_EveryTime, obj,                     1, MUIM_OOPWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_OOPWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_OOPWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_OOPWin_Remove);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_OOPWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct OOPWinData *cwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cwd->cwd_ClassTree, MUIM_NListtree_Clear, NULL, 0);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct OOPWinData *cwd = INST_DATA(cl, obj);
    struct OOPCallbackUserData ud;

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
    PrintOOPClass(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct OOPWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
        struct OOPClassEntry *ce = (struct OOPClassEntry *)tn->tn_User;

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
        struct OOPWinData *cwd = INST_DATA(cl, obj);
        struct MUI_NListtree_TreeNode *tn;

        if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
            struct ClassEntry *ce = (struct ClassEntry *)tn->tn_User;
            APTR detailWin;

            if ((detailWin = (Object *)(OOPDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End)) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_OOPDetailWin_Class, ce);
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
    struct OOPWinData *cwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(cwd->cwd_ClassTree)) != NULL) {
        struct OOPClassEntry *ce = (struct OOPClassEntry *)tn->tn_User;

        MySetContents(cwd->cwd_ClassText, "%s \"%s\"", ce->ce_Address, ce->ce_ClassName);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, cwd->cwd_RemoveButton,
                                                           (clientstate) ? NULL : cwd->cwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(OOPWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                  return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:              return (mDispose(cl, obj, (APTR)msg));
        case MUIM_OOPWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_OOPWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_OOPWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_OOPWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_OOPWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintOOPClass( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtOOPPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendOOPClassList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeOOPWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct OOPWinData), ENTRY(OOPWinDispatcher));
}

#ifdef __AROS__

BOOL InitOOP(void)
{
    OOPBase = OpenLibrary("oop.library", 0);
    if (!OOPBase)
        return FALSE;

    MetaAttrBase = OOP_ObtainAttrBase(IID_Meta);

    if (MetaAttrBase)
        return TRUE;

    CleanupOOP();
    return FALSE;
}

void CleanupOOP(void)
{
    if (MetaAttrBase)
        OOP_ReleaseAttrBase(IID_Meta);

    if (OOPBase) {
	CloseLibrary(OOPBase);
	OOPBase = NULL;
    }
}

#else

BOOL InitOOP(void)
{
    return TRUE;
}

void CleanupOOP(void)
{
}

#endif
