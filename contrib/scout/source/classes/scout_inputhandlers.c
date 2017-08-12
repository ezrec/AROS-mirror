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

struct InputHandlersWinData {
    TEXT ihwd_Title[WINDOW_TITLE_LENGTH];
    APTR ihwd_InputHandlerList;
    APTR ihwd_InputHandlerText;
    APTR ihwd_InputHandlerCount;
    APTR ihwd_RemoveButton;
    APTR ihwd_PriorityButton;
    APTR ihwd_MoreButton;
};

struct InputCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(inputlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct InputHandlerEntry));
}
MakeStaticHook(inputlist_con2hook, inputlist_con2func);

HOOKPROTONHNO(inputlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(inputlist_des2hook, inputlist_des2func);

HOOKPROTONHNO(inputlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct InputHandlerEntry *ihe = (struct InputHandlerEntry *)msg->entry;

    if (ihe) {
        msg->strings[0] = ihe->ihe_Address;
        msg->strings[1] = ihe->ihe_Name;
        msg->strings[2] = ihe->ihe_Type;
        msg->strings[3] = ihe->ihe_Pri;
        msg->strings[4] = ihe->ihe_Data;
        msg->strings[5] = ihe->ihe_Code;
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
MakeStaticHook(inputlist_dsp2hook, inputlist_dsp2func);

STATIC LONG inputlist_cmp2colfunc( struct InputHandlerEntry *ihe1,
                                   struct InputHandlerEntry *ihe2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(ihe1->ihe_Address, ihe2->ihe_Address);
        case 1: return stricmp(ihe1->ihe_Name, ihe2->ihe_Name);
        case 2: return stricmp(ihe1->ihe_Type, ihe2->ihe_Type);
        case 3: return PriorityCompare(ihe1->ihe_Pri, ihe2->ihe_Pri);
        case 4: return HexCompare(ihe1->ihe_Data, ihe2->ihe_Data);
        case 5: return HexCompare(ihe1->ihe_Code, ihe2->ihe_Code);
    }
}

HOOKPROTONHNO(inputlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct InputHandlerEntry *ihe1, *ihe2;
    ULONG col1, col2;

    ihe1 = (struct InputHandlerEntry *)msg->entry1;
    ihe2 = (struct InputHandlerEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = inputlist_cmp2colfunc(ihe2, ihe1, col1);
    } else {
        cmp = inputlist_cmp2colfunc(ihe1, ihe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = inputlist_cmp2colfunc(ihe2, ihe1, col2);
    } else {
        cmp = inputlist_cmp2colfunc(ihe1, ihe2, col2);
    }

    return cmp;
}
MakeStaticHook(inputlist_cmp2hook, inputlist_cmp2func);

STATIC struct InputEvent * ASM SAVEDS dummyIRQFunc( REG(a0, struct InputEvent *ie),
                                                    REG(a1, UNUSED APTR handlerData) )
{
    return ie;
}

STATIC void ReceiveList( void (* callback)( struct InputHandlerEntry *ihe, void *userData ),
                         void *userData )
{
    struct InputHandlerEntry *ihe;

    if ((ihe = tbAllocVecPooled(globalPool, sizeof(struct InputHandlerEntry))) != NULL) {
        if (SendDaemon("GetInputList")) {
            while (ReceiveDecodedEntry(ihe, sizeof(struct InputHandlerEntry))) {
                callback(ihe, userData);
            }
        }

        tbFreeVecPooled(globalPool, ihe);
    }
}

STATIC void IterateList( void (* callback)( struct InputHandlerEntry *ihe, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct InputHandlerEntry *ihe;
    struct MsgPort *mp;

    NewList((struct List *)&tmplist);

    if ((mp = CreateMsgPort()) != NULL) {
        struct IOStdReq *io;

        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(struct IOStdReq))) != NULL) {
            if (OpenDevice("input.device", 0, (struct IORequest *)io, 0) == 0) {
                struct Interrupt *myirq;

                if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
                    struct List *irqlist;
                    struct Interrupt *irq;

                    myirq->is_Node.ln_Type = NT_INTERRUPT;
                    myirq->is_Node.ln_Pri = 32;
                    myirq->is_Node.ln_Name = (STRPTR)"Scout";
                    myirq->is_Code = (void (*)())dummyIRQFunc;
                    myirq->is_Data = NULL;

                    io->io_Command = IND_ADDHANDLER;
                    io->io_Data = myirq;
                    DoIO((struct IORequest *)io);

                    Forbid();

                    irqlist = FindListOfNode((struct Node *)myirq);

                    ITERATE_LIST(irqlist, struct Interrupt *, irq) {
                        if (irq != myirq) {
                            if ((ihe = AllocVec(sizeof(struct InputHandlerEntry), MEMF_CLEAR)) != NULL) {
                                ihe->ihe_Addr = irq;

                                _snprintf(ihe->ihe_Address, sizeof(ihe->ihe_Address), ADDRESS_FORMAT, irq);
                                stccpy(ihe->ihe_Name, nonetest(irq->is_Node.ln_Name), sizeof(ihe->ihe_Name));
                                stccpy(ihe->ihe_Type, GetNodeType(irq->is_Node.ln_Type), sizeof(ihe->ihe_Type));
                                _snprintf(ihe->ihe_Pri, sizeof(ihe->ihe_Pri), "%4ld", irq->is_Node.ln_Pri);
                                _snprintf(ihe->ihe_Data, sizeof(ihe->ihe_Data), ADDRESS_FORMAT, irq->is_Data);
                                if (points2ram((APTR)irq->is_Code)) {
                                   _snprintf(ihe->ihe_Code, sizeof(ihe->ihe_Code), MUIX_PH ADDRESS_FORMAT MUIX_PT, irq->is_Code);
                                } else {
                                   _snprintf(ihe->ihe_Code, sizeof(ihe->ihe_Code), ADDRESS_FORMAT, irq->is_Code);
                                }

                                AddTail((struct List *)&tmplist, (struct Node *)ihe);
                            }
                        }
                    }

                    Permit();

                    io->io_Command = IND_REMHANDLER;
                    io->io_Data = myirq;
                    DoIO((struct IORequest *)io);

                    tbFreeVecPooled(globalPool, myirq);
                }

                CloseDevice((struct IORequest *)io);
            }

            DeleteIORequest((struct IORequest *)io);
        }

        DeleteMsgPort(mp);
    }

    ITERATE_LIST(&tmplist, struct InputHandlerEntry *, ihe) {
        callback(ihe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct InputHandlerEntry *ihe,
                            void *userData )
{
    struct InputCallbackUserData *ud = (struct InputCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, ihe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct InputHandlerEntry *ihe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtInputHandlersPrintLine, ihe->ihe_Address, ihe->ihe_Type, ihe->ihe_Pri, ihe->ihe_Data, StripMUIFormatting(ihe->ihe_Code), ihe->ihe_Name);
}

STATIC void SendCallback( struct InputHandlerEntry *ihe,
                          UNUSED void *userData )
{
    SendEncodedEntry(ihe, sizeof(struct InputHandlerEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR inputlist, inputtext, inputcount, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "InputHandlers",
        MUIA_Window_ID, MakeID('I','N','P','U'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&inputlist, MakeID('I','N','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR", &inputlist_con2hook, &inputlist_des2hook, &inputlist_dsp2hook, &inputlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&inputtext, &inputcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
        APTR parent;

        ihwd->ihwd_InputHandlerList = inputlist;
        ihwd->ihwd_InputHandlerText = inputtext;
        ihwd->ihwd_InputHandlerCount = inputcount;
        ihwd->ihwd_RemoveButton = removeButton;
        ihwd->ihwd_PriorityButton = priorityButton;
        ihwd->ihwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtInputHandlersTitle, ihwd->ihwd_Title, sizeof(ihwd->ihwd_Title)));
        set(obj, MUIA_Window_DefaultObject, inputlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, ihwd->ihwd_RemoveButton,
                                                          ihwd->ihwd_PriorityButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(inputlist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_InputHandlersWin_ListChange);
        DoMethod(inputlist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_InputHandlersWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_InputHandlersWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_InputHandlersWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_InputHandlersWin_Remove);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_InputHandlersWin_Priority);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_InputHandlersWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(ihwd->ihwd_InputHandlerList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
    struct InputCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(ihwd->ihwd_InputHandlerList, MUIA_NList_Quiet, TRUE);
    DoMethod(ihwd->ihwd_InputHandlerList, MUIM_NList_Clear);

    ud.ud_List = ihwd->ihwd_InputHandlerList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(ihwd->ihwd_InputHandlerCount, ud.ud_Count);
    MySetContents(ihwd->ihwd_InputHandlerText, "");

    set(ihwd->ihwd_InputHandlerList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, ihwd->ihwd_RemoveButton,
                                                      ihwd->ihwd_PriorityButton,
                                                      ihwd->ihwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintInputHandlers(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
    struct InputHandlerEntry *ihe;

    if ((ihe = (struct InputHandlerEntry *)GetActiveEntry(ihwd->ihwd_InputHandlerList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveInputHandler, ihe->ihe_Name)) {
            if (MyDoCommand("RemoveInputHandler %s", ihe->ihe_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_InputHandlersWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
    struct InputHandlerEntry *ihe;

    if ((ihe = (struct InputHandlerEntry *)GetActiveEntry(ihwd->ihwd_InputHandlerList)) != NULL) {
        LONG pri;

        pri = atol(ihe->ihe_Pri);
        if (GetPriority(ihe->ihe_Name, &pri)) {
            if (MyDoCommand("SetPriority INPUTHANDLER \"%s\" %ld", ihe->ihe_Name, pri) == RETURN_OK) {
                DoMethod(obj, MUIM_InputHandlersWin_Update);
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
        struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
        struct InputHandlerEntry *ihe;

        if ((ihe = (struct InputHandlerEntry *)GetActiveEntry(ihwd->ihwd_InputHandlerList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)InputHandlersDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                SetAttrs(detailWin, MUIA_InputHandlersDetailWin_InputHandler, ihe,
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
    struct InputHandlersWinData *ihwd = INST_DATA(cl, obj);
    struct InputHandlerEntry *ihe;

    if ((ihe = (struct InputHandlerEntry *)GetActiveEntry(ihwd->ihwd_InputHandlerList)) != NULL) {
        MySetContents(ihwd->ihwd_InputHandlerText, "%s \"%s\"", ihe->ihe_Address, ihe->ihe_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, ihwd->ihwd_RemoveButton,
                                                           ihwd->ihwd_PriorityButton,
                                                           (clientstate) ? NULL : ihwd->ihwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(InputHandlersWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                            return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                        return (mDispose(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_Priority:    return (mPriority(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_InputHandlersWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintInputHandlers( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtInputHandlersPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendInputList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeInputHandlersWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct InputHandlersWinData), ENTRY(InputHandlersWinDispatcher));
}
