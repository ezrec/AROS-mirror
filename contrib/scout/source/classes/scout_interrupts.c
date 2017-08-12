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

struct InterruptsWinData {
    TEXT iwd_Title[WINDOW_TITLE_LENGTH];
    APTR iwd_InterruptTree;
    APTR iwd_InterruptText;
    APTR iwd_InterruptCount;
    APTR iwd_RemoveButton;
    APTR iwd_MoreButton;
};

struct InterruptsCallbackUserData {
    APTR ud_Tree;
    ULONG ud_Count;
    STRPTR ud_FindMe;
    struct Interrupt *ud_FoundThis;
};

STATIC CONST_STRPTR intTypeShort[] = {
    "1 / Serial Out",
    "1 / Disk Block",
    "1 / SoftInt",
    "2 / INT2/CIAA",
    "3 / Copper",
    "3 / Vertical Blank",
    "3 / Blitter",
    "4 / Audio 0",
    "4 / Audio 1",
    "4 / Audio 2",
    "4 / Audio 3",
    "5 / Serial In",
    "5 / Disk Sync",
    "6 / INT6/CIAB",
    "6 / INTEN",
    "7 / NMI"
};

HOOKPROTONHNO(inttree_confunc, IPTR, struct MUIP_NListtree_ConstructMessage *msg)
{
    return AllocListEntry(msg->MemPool, msg->UserData, sizeof(struct InterruptEntry));
}
MakeStaticHook(inttree_conhook, inttree_confunc);

HOOKPROTONHNO(inttree_desfunc, void, struct MUIP_NListtree_DestructMessage *msg)
{
    FreeListEntry(msg->MemPool, &msg->UserData);
}
MakeStaticHook(inttree_deshook, inttree_desfunc);

HOOKPROTONHNO(inttree_dspfunc, void, struct MUIP_NListtree_DisplayMessage *msg)
{
    STATIC CONST_STRPTR empty = "\0";

    if (msg->TreeNode != NULL) {
        struct InterruptEntry *ie = msg->TreeNode->tn_User;

        if (stricmp(ie->ie_Type, "HANDLER") == 0) {
            msg->Array[0] = ie->ie_Address;
            msg->Array[1] = ie->ie_Number;
            msg->Array[2] = ie->ie_Name;
            msg->Array[3] = ie->ie_Kind;
            msg->Array[4] = ie->ie_Pri;
            msg->Array[5] = ie->ie_Data;
            msg->Array[6] = ie->ie_Code;
        } else if (stricmp(ie->ie_Type, "SERVER_LIST") == 0) {
            msg->Array[0] = ie->ie_Address;
            msg->Array[1] = ie->ie_Number;
            msg->Array[2] = (STRPTR)empty;
            msg->Array[3] = ie->ie_Kind;
            msg->Array[4] = ie->ie_Pri;
            msg->Array[5] = ie->ie_Data;
            msg->Array[6] = ie->ie_Code;
        } else if (stricmp(ie->ie_Type, "SERVER_NODE") == 0) {
            msg->Array[0] = ie->ie_Address;
            msg->Array[1] = ie->ie_Number;
            msg->Array[2] = ie->ie_Name;
            msg->Array[3] = (STRPTR)empty;
            msg->Array[4] = ie->ie_Pri;
            msg->Array[5] = ie->ie_Data;
            msg->Array[6] = ie->ie_Code;
        }
    } else {
        msg->Array[0] = txtAddress;
        msg->Array[1] = txtInterruptIPL;
        msg->Array[2] = txtNodeName;
        msg->Array[3] = txtInterruptType;
        msg->Array[4] = txtNodePri;
        msg->Array[5] = txtInterruptData;
        msg->Array[6] = txtInterruptCode;
        msg->Preparse[0] = (STRPTR)MUIX_B;
        msg->Preparse[1] = (STRPTR)MUIX_B;
        msg->Preparse[2] = (STRPTR)MUIX_B;
        msg->Preparse[3] = (STRPTR)MUIX_B;
        msg->Preparse[4] = (STRPTR)MUIX_B;
        msg->Preparse[5] = (STRPTR)MUIX_B;
        msg->Preparse[6] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(inttree_dsphook, inttree_dspfunc);

HOOKPROTONHNO(inttree_findfunc, LONG, struct MUIP_NListtree_FindUserDataMessage *msg)
{
    struct InterruptEntry *ie;

    ie = (struct InterruptEntry *)msg->UserData;

    if (ie) {
        return stricmp(msg->User, ie->ie_RealNumber);
    } else {
        return ~0;
    }
}
MakeStaticHook(inttree_findhook, inttree_findfunc);

STATIC void ReceiveList( void (* callback)( struct InterruptEntry *ie, void *userData ),
                         void *userData )
{
    struct InterruptEntry *ie;

    if ((ie = tbAllocVecPooled(globalPool, sizeof(struct InterruptEntry))) != NULL) {
        if (SendDaemon("GetIntList")) {
            while (ReceiveDecodedEntry(ie, sizeof(struct InterruptEntry))) {
                callback(ie, userData);
            }
        }

        tbFreeVecPooled(globalPool, ie);
    }
}

STATIC void IterateList( void (* callback)( struct InterruptEntry *ie, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct InterruptEntry *ie;
    ULONG vec;

    NewList((struct List *)&tmplist);

    Forbid();

    for (vec = 0; vec <= 15; vec++) {
        struct IntVector *intvec;

        if ((intvec = (struct IntVector *)&SysBase->IntVects[vec]) != NULL) {
            if ((ie = AllocVec(sizeof(struct InterruptEntry), MEMF_CLEAR)) != NULL) {
                if (intvec->iv_Node == NULL && intvec->iv_Data != NULL && intvec->iv_Data != (APTR)-1) {
                    struct Interrupt *irq;
                    ULONG servercnt;

                    irq = (struct Interrupt *)intvec->iv_Node;
                    ie->ie_Addr = irq;
                    _snprintf(ie->ie_Address, sizeof(ie->ie_Address), ADDRESS_FORMAT, intvec);
                    ie->ie_Name[0] = 0x00;
                    if (points2ram((APTR)intvec->iv_Code)) {
                        _snprintf(ie->ie_Code, sizeof(ie->ie_Code), MUIX_PH ADDRESS_FORMAT MUIX_PT, intvec->iv_Code);
                    } else {
                        _snprintf(ie->ie_Code, sizeof(ie->ie_Code), ADDRESS_FORMAT, intvec->iv_Code);
                    }
                    _snprintf(ie->ie_Data, sizeof(ie->ie_Data), ADDRESS_FORMAT, intvec->iv_Data);
                    _snprintf(ie->ie_Pri, sizeof(ie->ie_Pri), "%4ld", (intvec->iv_Node != NULL) ? intvec->iv_Node->ln_Pri : 0);
                    stccpy(ie->ie_Number, intTypeShort[vec], sizeof(ie->ie_Number));
                    stccpy(ie->ie_RealNumber, intTypeShort[vec], sizeof(ie->ie_RealNumber));
                    stccpy(ie->ie_Kind, txtInterruptServer, sizeof(ie->ie_Kind));
                    stccpy(ie->ie_Type, "SERVER_LIST", sizeof(ie->ie_Type));
                    ie->ie_IntNumber = vec;

                    AddTail((struct List *)&tmplist, (struct Node *)ie);

                    servercnt = 0;
                    ITERATE_LIST(intvec->iv_Data, struct Interrupt *, irq) {
                        if ((ie = AllocVec(sizeof(struct InterruptEntry), MEMF_CLEAR)) != NULL) {
                            servercnt++;

                            ie->ie_Addr = irq;
                            if (points2ram((APTR)irq->is_Code)) {
                                _snprintf(ie->ie_Code, sizeof(ie->ie_Code), MUIX_PH ADDRESS_FORMAT MUIX_PT, irq->is_Code);
                            } else {
                                _snprintf(ie->ie_Code, sizeof(ie->ie_Code), ADDRESS_FORMAT, irq->is_Code);
                            }
                            _snprintf(ie->ie_Address, sizeof(ie->ie_Address), ADDRESS_FORMAT, irq);
                            stccpy(ie->ie_Name, nonetest(irq->is_Node.ln_Name), sizeof(ie->ie_Name));
                            _snprintf(ie->ie_Pri, sizeof(ie->ie_Pri), "%4ld", irq->is_Node.ln_Pri);
                            _snprintf(ie->ie_Data, sizeof(ie->ie_Data), ADDRESS_FORMAT, irq->is_Data);
                            _snprintf(ie->ie_Number, sizeof(ie->ie_Number), "%ld", servercnt);
                            stccpy(ie->ie_RealNumber, intTypeShort[vec], sizeof(ie->ie_RealNumber));
                            stccpy(ie->ie_Kind, txtInterruptServer, sizeof(ie->ie_Kind));
                            stccpy(ie->ie_Type, "SERVER_NODE", sizeof(ie->ie_Type));
                            ie->ie_IntNumber = vec;

                            AddTail((struct List *)&tmplist, (struct Node *)ie);
                        }
                    }
                } else {
                    struct Interrupt *irq;

                    irq = (struct Interrupt *)intvec->iv_Node;

                    if (intvec->iv_Node != NULL && intvec->iv_Code != NULL) {
                        ie->ie_Addr = irq;
                    } else {
                        ie->ie_Addr = NULL;
                    }

                    if (points2ram((APTR)intvec->iv_Code)) {
                        _snprintf(ie->ie_Code, sizeof(ie->ie_Code), MUIX_PH ADDRESS_FORMAT MUIX_PT, intvec->iv_Code);
                    } else {
                        _snprintf(ie->ie_Code, sizeof(ie->ie_Code), ADDRESS_FORMAT, intvec->iv_Code);
                    }
                    _snprintf(ie->ie_Address, sizeof(ie->ie_Address), ADDRESS_FORMAT, intvec);
                    if (irq) {
                        stccpy(ie->ie_Name, nonetest(irq->is_Node.ln_Name), sizeof(ie->ie_Name));
                    } else {
                        stccpy(ie->ie_Name, nonetest(NULL), sizeof(ie->ie_Name));
                    }
                    _snprintf(ie->ie_Pri, sizeof(ie->ie_Pri), "%4ld", (intvec->iv_Node != NULL) ? intvec->iv_Node->ln_Pri : 0);
                    _snprintf(ie->ie_Data, sizeof(ie->ie_Data), ADDRESS_FORMAT, intvec->iv_Data);
                    stccpy(ie->ie_Number, intTypeShort[vec], sizeof(ie->ie_Number));
                    stccpy(ie->ie_RealNumber, intTypeShort[vec], sizeof(ie->ie_RealNumber));
                    if (intvec->iv_Data == NULL && intvec->iv_Code == NULL) {
                        stccpy(ie->ie_Kind, txtInterruptUnused, sizeof(ie->ie_Kind));
                    } else if (intvec->iv_Data == (APTR)-1 && intvec->iv_Code == (APTR)-1) {
                        stccpy(ie->ie_Kind, txtInterruptFree, sizeof(ie->ie_Kind));
                    } else {
                        stccpy(ie->ie_Kind, txtInterruptHandler, sizeof(ie->ie_Kind));
                    }
                    stccpy(ie->ie_Type, "HANDLER", sizeof(ie->ie_Type));
                    ie->ie_IntNumber = vec;

                    AddTail((struct List *)&tmplist, (struct Node *)ie);
                }
            }
        }
    }

    Permit();

    ITERATE_LIST(&tmplist, struct InterruptEntry *, ie) {
        callback(ie, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct InterruptEntry *ie,
                            void *userData )
{
    struct InterruptsCallbackUserData *ud = (struct InterruptsCallbackUserData *)userData;
    struct MUI_NListtree_TreeNode *parent;
    ULONG flags;
    BOOL search_parent;

    flags = 0;
    search_parent = FALSE;
    if (stricmp(ie->ie_Type, "HANDLER") == 0) {
        ud->ud_Count++;
    } else if (stricmp(ie->ie_Type, "SERVER_LIST") == 0) {
        flags = TNF_LIST | TNF_OPEN;
        // don't count up, we only count real IRQ entries
    } else if (stricmp(ie->ie_Type, "SERVER_NODE") == 0) {
        ud->ud_Count++;
        search_parent = TRUE;
    }

    if (search_parent) {
        parent = (struct MUI_NListtree_TreeNode *)DoMethod(ud->ud_Tree, MUIM_NListtree_FindUserData, MUIV_NListtree_FindUserData_ListNode_Root, ie->ie_RealNumber, MUIV_NListtree_FindUserData_Flag_StartNode);
    } else {
        parent = MUIV_NListtree_Insert_ListNode_Root;
    }
    DoMethod(ud->ud_Tree, MUIM_NListtree_Insert, ie->ie_Name, ie, parent, MUIV_NListtree_Insert_PrevNode_Tail, flags);
}

STATIC void PrintCallback( struct InterruptEntry *ie,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtInterruptsPrintLine, ie->ie_Address, ie->ie_Pri, ie->ie_Data, StripMUIFormatting(ie->ie_Code), ie->ie_RealNumber, ie->ie_Kind, ie->ie_Name);
}

STATIC void SendCallback( struct InterruptEntry *ie,
                          UNUSED void *userData )
{
    SendEncodedEntry(ie, sizeof(struct InterruptEntry));
}

STATIC void FindCallback( struct InterruptEntry *ie,
                          void *userData )
{
    struct InterruptsCallbackUserData *ud = (struct InterruptsCallbackUserData *)userData;

    if (stricmp(ud->ud_FindMe, ie->ie_Name) == 0) ud->ud_FoundThis = ie->ie_Addr;
}

STATIC void RemoveCallback( struct InterruptEntry *ie,
                            void *userData )
{
    struct InterruptsCallbackUserData *ud = (struct InterruptsCallbackUserData *)userData;

    if (stricmp(ie->ie_Type, "SERVER_NODE") == 0 && stricmp(ud->ud_FindMe, ie->ie_Name) == 0) {
        RemIntServer(ie->ie_IntNumber, ie->ie_Addr);
        ud->ud_FoundThis = ie->ie_Addr;
    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR inttree, inttext, intcount, updateButton, printButton, removeButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Interrupts",
        MUIA_Window_ID, MakeID('I','N','T','E'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListtreeObject(&inttree, "BAR,BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR", &inttree_conhook, &inttree_deshook, &inttree_dsphook, NULL, &inttree_findhook, 1, FALSE),
            Child, (IPTR)MyBelowListview(&inttext, &intcount),

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
        struct InterruptsWinData *iwd = INST_DATA(cl, obj);
        APTR parent;

        iwd->iwd_InterruptTree = inttree;
        iwd->iwd_InterruptText = inttext;
        iwd->iwd_InterruptCount = intcount;
        iwd->iwd_RemoveButton = removeButton;
        iwd->iwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtInterruptsTitle, iwd->iwd_Title, sizeof(iwd->iwd_Title)));
        set(obj, MUIA_Window_DefaultObject, inttree);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, iwd->iwd_RemoveButton,
                                                          iwd->iwd_MoreButton,
                                                          NULL);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest,   TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(inttree,      MUIM_Notify, MUIA_NListtree_Active,      MUIV_EveryTime, obj,                     1, MUIM_InterruptsWin_ListChange);
        DoMethod(inttree,      MUIM_Notify, MUIA_NListtree_DoubleClick, MUIV_EveryTime, obj,                     1, MUIM_InterruptsWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_InterruptsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_InterruptsWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_InterruptsWin_Remove);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     1, MUIM_InterruptsWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,               FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct InterruptsWinData *iwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(iwd->iwd_InterruptTree, MUIM_NListtree_Clear, NULL, 0);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct InterruptsWinData *iwd = INST_DATA(cl, obj);
    struct InterruptsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(iwd->iwd_InterruptTree, MUIA_NListtree_Quiet, TRUE);
    DoMethod(iwd->iwd_InterruptTree, MUIM_NListtree_Clear, NULL, 0);

    ud.ud_Tree = iwd->iwd_InterruptTree;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(iwd->iwd_InterruptCount, ud.ud_Count);
    MySetContents(iwd->iwd_InterruptText, "");

    set(iwd->iwd_InterruptTree, MUIA_NListtree_Quiet, FALSE);
    set(iwd->iwd_InterruptTree, MUIA_NListtree_Active, MUIV_NListtree_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, iwd->iwd_RemoveButton,
                                                      iwd->iwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintInterrupts(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct InterruptsWinData *iwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(iwd->iwd_InterruptTree)) != NULL) {
        struct InterruptEntry *ie = (struct InterruptEntry *)tn->tn_User;

        if (stricmp(ie->ie_Type, "HANDLER") == 0) {
            MyRequest(msgErrorContinue, msgCantRemoveInterruptHandler);
        } else {
            if (MyRequest(msgYesNo, msgWantToRemoveInterrupt, ie->ie_Name)) {
                if (MyDoCommand("RemoveInterrupt \"%s\"", ie->ie_Name) == RETURN_OK) {
                    DoMethod(obj, MUIM_InterruptsWin_Update);
                }
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
        struct InterruptsWinData *iwd = INST_DATA(cl, obj);
        struct MUI_NListtree_TreeNode *tn;

        if ((tn = GetActiveTreeNode(iwd->iwd_InterruptTree)) != NULL) {
            struct InterruptEntry *ie = (struct InterruptEntry *)tn->tn_User;

            if ((stricmp(ie->ie_Type, "HANDLER") == 0 && ie->ie_Addr != NULL) || stricmp(ie->ie_Type, "SERVER_NODE") == 0) {
                APTR detailWin;

                if  ((detailWin = (Object *)InterruptsDetailWindowObject,
                        MUIA_Window_ParentWindow, (IPTR)obj,
                    End) != NULL) {
                    COLLECT_RETURNIDS;
                    set(detailWin, MUIA_InterruptsDetailWin_Interrupt, ie);
                    set(detailWin, MUIA_Window_Open, TRUE);
                    REISSUE_RETURNIDS;
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
    struct InterruptsWinData *iwd = INST_DATA(cl, obj);
    struct MUI_NListtree_TreeNode *tn;

    if ((tn = GetActiveTreeNode(iwd->iwd_InterruptTree)) != NULL) {
        struct InterruptEntry *ie = (struct InterruptEntry *)tn->tn_User;

        if (stricmp(ie->ie_Type, "HANDLER") == 0 && ie->ie_Addr != NULL) {
            MySetContents(iwd->iwd_InterruptText, "%s \"%s\"", ie->ie_Address, ie->ie_Name);
            set(iwd->iwd_RemoveButton, MUIA_Disabled, TRUE);
            if (!clientstate) set(iwd->iwd_MoreButton, MUIA_Disabled, FALSE);
        } else if (stricmp(ie->ie_Type, "SERVER_LIST") == 0) {
            MySetContents(iwd->iwd_InterruptText, "");
            set(iwd->iwd_RemoveButton, MUIA_Disabled, TRUE);
            if (!clientstate) set(iwd->iwd_MoreButton, MUIA_Disabled, TRUE);
        } else if (stricmp(ie->ie_Type, "SERVER_NODE") == 0) {
            MySetContents(iwd->iwd_InterruptText, "%s \"%s\"", ie->ie_Address, ie->ie_Name);
            set(iwd->iwd_RemoveButton, MUIA_Disabled, FALSE);
            if (!clientstate) set(iwd->iwd_MoreButton, MUIA_Disabled, FALSE);
        } else {
            MySetContents(iwd->iwd_InterruptText, "");
            DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, iwd->iwd_RemoveButton,
                                                              iwd->iwd_MoreButton,
                                                              NULL);
        }
    }

    return 0;
}

DISPATCHER(InterruptsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                         return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                     return (mDispose(cl, obj, (APTR)msg));
        case MUIM_InterruptsWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_InterruptsWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_InterruptsWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_InterruptsWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_InterruptsWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintInterrupts( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtInterruptsPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendIntList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

struct Interrupt *FindInterrupt( STRPTR name )
{
    struct InterruptsCallbackUserData ud;

    ud.ud_FindMe = name;
    ud.ud_FoundThis = NULL;

    IterateList(FindCallback, &ud);

    return ud.ud_FoundThis;
}

struct Interrupt *RemoveInterrupt( STRPTR name )
{
    struct InterruptsCallbackUserData ud;

    ud.ud_FindMe = name;
    ud.ud_FoundThis = NULL;

    IterateList(RemoveCallback, &ud);

    return ud.ud_FoundThis;
}

APTR MakeInterruptsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct InterruptsWinData), ENTRY(InterruptsWinDispatcher));
}

