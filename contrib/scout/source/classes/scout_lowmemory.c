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

struct LowMemoryWinData {
    TEXT lmwd_Title[WINDOW_TITLE_LENGTH];
    APTR lmwd_LowMemoryList;
    APTR lmwd_LowMemoryText;
    APTR lmwd_LowMemoryCount;
    APTR lmwd_CauseButton;
    APTR lmwd_RemoveButton;
    APTR lmwd_PriorityButton;
    APTR lmwd_MoreButton;
};

struct LowMemoryCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(lowmemlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct LowMemoryEntry));
}
MakeStaticHook(lowmemlist_con2hook, lowmemlist_con2func);

HOOKPROTONHNO(lowmemlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(lowmemlist_des2hook, lowmemlist_des2func);

HOOKPROTONHNO(lowmemlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct LowMemoryEntry *lme = (struct LowMemoryEntry *)msg->entry;

    if (lme) {
        msg->strings[0] = lme->lme_Address;
        msg->strings[1] = lme->lme_Name;
        msg->strings[2] = lme->lme_Type;
        msg->strings[3] = lme->lme_Pri;
        msg->strings[4] = lme->lme_Data;
        msg->strings[5] = lme->lme_Code;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtInterruptData;
        msg->strings[5] = txtInterruptCode;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(lowmemlist_dsp2hook, lowmemlist_dsp2func);

STATIC LONG lowmemlist_cmp2colfunc( struct LowMemoryEntry *lme1,
                                    struct LowMemoryEntry *lme2,
                                    ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(lme1->lme_Address, lme2->lme_Address);
        case 1: return stricmp(lme1->lme_Name, lme2->lme_Name);
        case 2: return stricmp(lme1->lme_Type, lme2->lme_Type);
        case 3: return PriorityCompare(lme1->lme_Pri, lme2->lme_Pri);
        case 4: return HexCompare(lme1->lme_Data, lme2->lme_Data);
        case 5: return HexCompare(lme1->lme_Code, lme2->lme_Code);
    }
}

HOOKPROTONHNO(lowmemlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct LowMemoryEntry *lme1, *lme2;
    ULONG col1, col2;

    lme1 = (struct LowMemoryEntry *)msg->entry1;
    lme2 = (struct LowMemoryEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = lowmemlist_cmp2colfunc(lme2, lme1, col1);
    } else {
        cmp = lowmemlist_cmp2colfunc(lme1, lme2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = lowmemlist_cmp2colfunc(lme2, lme1, col2);
    } else {
        cmp = lowmemlist_cmp2colfunc(lme1, lme2, col2);
    }

    return cmp;
}
MakeStaticHook(lowmemlist_cmp2hook, lowmemlist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct LowMemoryEntry *lme, void *userData ),
                         void *userData )
{
    struct LowMemoryEntry *lme;

    if ((lme = tbAllocVecPooled(globalPool, sizeof(struct LowMemoryEntry))) != NULL) {
        if (SendDaemon("GetLowMemList")) {
            while (ReceiveDecodedEntry(lme, sizeof(struct LowMemoryEntry))) {
                callback(lme, userData);
            }
        }

        tbFreeVecPooled(globalPool, lme);
    }
}

STATIC void IterateList( void (* callback)( struct LowMemoryEntry *lme, void *userData ),
                         void *userData )
{
    if (CheckLibVersion((struct Library *)SysBase, 39, 0) == CLV_NEWER_OR_SAME) {
        struct MinList tmplist;
        struct LowMemoryEntry *lme;
        struct Interrupt *irq;

        NewList((struct List *)&tmplist);

        Forbid();

        ITERATE_LIST(&SysBase->ex_MemHandlers, struct Interrupt *, irq) {
            if ((lme = AllocVec(sizeof(struct LowMemoryEntry), MEMF_CLEAR)) != NULL) {
                lme->lme_Addr = irq;

                _snprintf(lme->lme_Address, sizeof(lme->lme_Address), ADDRESS_FORMAT, irq);
                stccpy(lme->lme_Name, nonetest(irq->is_Node.ln_Name), sizeof(lme->lme_Name));
                stccpy(lme->lme_Type, GetNodeType(irq->is_Node.ln_Type), sizeof(lme->lme_Type));
                _snprintf(lme->lme_Pri, sizeof(lme->lme_Pri), "%4ld", irq->is_Node.ln_Pri);
                _snprintf(lme->lme_Data, sizeof(lme->lme_Data), ADDRESS_FORMAT, irq->is_Data);
                if (points2ram((APTR)irq->is_Code)) {
                   _snprintf(lme->lme_Code, sizeof(lme->lme_Code), MUIX_PH ADDRESS_FORMAT, irq->is_Code);
                } else {
                   _snprintf(lme->lme_Code, sizeof(lme->lme_Code), ADDRESS_FORMAT, irq->is_Code);
                }

                AddTail((struct List *)&tmplist, (struct Node *)lme);
            }
        }

        Permit();

        ITERATE_LIST(&tmplist, struct LowMemoryEntry *, lme) {
            callback(lme, userData);
        }
        FreeLinkedList((struct List *)&tmplist);
    }
}

STATIC void UpdateCallback( struct LowMemoryEntry *lme,
                            void *userData )
{
    struct LowMemoryCallbackUserData *ud = (struct LowMemoryCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, lme);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct LowMemoryEntry *lme,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtLowMemoryPrintLine, lme->lme_Address, lme->lme_Type, lme->lme_Pri, lme->lme_Data, StripMUIFormatting(lme->lme_Code), lme->lme_Name);
}

STATIC void SendCallback( struct LowMemoryEntry *lme,
                          UNUSED void *userData )
{
    SendEncodedEntry(lme, sizeof(struct LowMemoryEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR lowmemlist, lowmemtext, lowmemcount, updateButton, printButton, causeButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "LowMemory",
        MUIA_Window_ID, MakeID('L','O','W','M'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&lowmemlist, MakeID('L','M','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR", &lowmemlist_con2hook, &lowmemlist_des2hook, &lowmemlist_dsp2hook, &lowmemlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&lowmemtext, &lowmemcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(causeButton    = MakeButton(txtCause)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
        APTR parent;

        lmwd->lmwd_LowMemoryList = lowmemlist;
        lmwd->lmwd_LowMemoryText = lowmemtext;
        lmwd->lmwd_LowMemoryCount = lowmemcount;
        lmwd->lmwd_CauseButton = causeButton;
        lmwd->lmwd_RemoveButton = removeButton;
        lmwd->lmwd_PriorityButton = priorityButton;
        lmwd->lmwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtLowMemoryTitle, lmwd->lmwd_Title, sizeof(lmwd->lmwd_Title)));
        set(obj, MUIA_Window_DefaultObject, lowmemlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, lmwd->lmwd_CauseButton,
                                                          lmwd->lmwd_RemoveButton,
                                                          lmwd->lmwd_PriorityButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(lowmemlist,     MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_LowMemoryWin_ListChange);
        DoMethod(lowmemlist,     MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_LowMemoryWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_Print);
        DoMethod(causeButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_Cause);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_Remove);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_Priority);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LowMemoryWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(lmwd->lmwd_LowMemoryList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
    struct LowMemoryCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(lmwd->lmwd_LowMemoryList, MUIA_NList_Quiet, TRUE);
    DoMethod(lmwd->lmwd_LowMemoryList, MUIM_NList_Clear);

    ud.ud_List = lmwd->lmwd_LowMemoryList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(lmwd->lmwd_LowMemoryCount, ud.ud_Count);
    MySetContents(lmwd->lmwd_LowMemoryText, "");

    set(lmwd->lmwd_LowMemoryList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, lmwd->lmwd_CauseButton,
                                                      lmwd->lmwd_RemoveButton,
                                                      lmwd->lmwd_PriorityButton,
                                                      lmwd->lmwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintLowMemory(NULL);

    return 0;
}

STATIC IPTR mCause( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
    struct LowMemoryEntry *lme;

    if ((lme = (struct LowMemoryEntry *)GetActiveEntry(lmwd->lmwd_LowMemoryList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToCauseLowMemory, lme->lme_Name)) {
            MyDoCommand("CauseLowMemory %s", lme->lme_Address);
        }
    }

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
    struct LowMemoryEntry *lme;

    if ((lme = (struct LowMemoryEntry *)GetActiveEntry(lmwd->lmwd_LowMemoryList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveLowMemory, lme->lme_Name)) {
            if (MyDoCommand("RemoveLowMemory %s", lme->lme_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_LowMemoryWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
    struct LowMemoryEntry *lme;

    if ((lme = (struct LowMemoryEntry *)GetActiveEntry(lmwd->lmwd_LowMemoryList)) != NULL) {
        LONG pri;

        pri = atol(lme->lme_Pri);
        if (GetPriority(lme->lme_Name, &pri)) {
            if (MyDoCommand("SetPriority LOWMEMORY \"%s\" %ld", lme->lme_Name, pri) == RETURN_OK) {
                _snprintf(lme->lme_Pri, sizeof(lme->lme_Pri), "%4ld", pri);
                RedrawActiveEntry(lmwd->lmwd_LowMemoryList);
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
        struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
        struct LowMemoryEntry *lme;

        if ((lme = (struct LowMemoryEntry *)GetActiveEntry(lmwd->lmwd_LowMemoryList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)LowMemoryDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                SetAttrs(detailWin, MUIA_LowMemoryDetailWin_LowMemoryHandler, lme,
                                    MUIA_Window_Open, TRUE,
                                    TAG_DONE);
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
    struct LowMemoryWinData *lmwd = INST_DATA(cl, obj);
    struct LowMemoryEntry *lme;

    if ((lme = (struct LowMemoryEntry *)GetActiveEntry(lmwd->lmwd_LowMemoryList)) != NULL) {
        MySetContents(lmwd->lmwd_LowMemoryText, "%s \"%s\"", lme->lme_Address, lme->lme_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, lmwd->lmwd_CauseButton,
                                                           lmwd->lmwd_RemoveButton,
                                                           lmwd->lmwd_PriorityButton,
                                                           (clientstate) ? NULL : lmwd->lmwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(LowMemoryWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                        return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                    return (mDispose(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_Cause:       return (mCause(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_Priority:    return (mPriority(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_LowMemoryWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintLowMemory( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtLowMemoryPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendLowMemory( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeLowMemoryWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct LowMemoryWinData), ENTRY(LowMemoryWinDispatcher));
}

