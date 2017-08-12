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

struct ResetHandlersWinData {
    TEXT rhwd_Title[WINDOW_TITLE_LENGTH];
    APTR rhwd_ResetHandlerList;
    APTR rhwd_ResetHandlerText;
    APTR rhwd_ResetHandlerCount;
    APTR rhwd_RemoveButton;
    APTR rhwd_PriorityButton;
    APTR rhwd_MoreButton;
};

struct ResetCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(resetlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct ResetHandlerEntry));
}
MakeStaticHook(resetlist_con2hook, resetlist_con2func);

HOOKPROTONHNO(resetlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(resetlist_des2hook, resetlist_des2func);

HOOKPROTONHNO(resetlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct ResetHandlerEntry *rhe = (struct ResetHandlerEntry *)msg->entry;

    if (rhe) {
        msg->strings[0] = rhe->rhe_Address;
        msg->strings[1] = rhe->rhe_Name;
        msg->strings[2] = rhe->rhe_Type;
        msg->strings[3] = rhe->rhe_Pri;
        msg->strings[4] = rhe->rhe_Data;
        msg->strings[5] = rhe->rhe_Code;
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
MakeStaticHook(resetlist_dsp2hook, resetlist_dsp2func);

STATIC LONG resetlist_cmp2colfunc( struct ResetHandlerEntry *rhe1,
                                   struct ResetHandlerEntry *rhe2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(rhe1->rhe_Address, rhe2->rhe_Address);
        case 1: return stricmp(rhe1->rhe_Name, rhe2->rhe_Name);
        case 2: return stricmp(rhe1->rhe_Type, rhe2->rhe_Type);
        case 3: return PriorityCompare(rhe1->rhe_Pri, rhe2->rhe_Pri);
        case 4: return HexCompare(rhe1->rhe_Data, rhe2->rhe_Data);
        case 5: return HexCompare(rhe1->rhe_Code, rhe2->rhe_Code);
    }
}

HOOKPROTONHNO(resetlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct ResetHandlerEntry *rhe1, *rhe2;
    ULONG col1, col2;

    rhe1 = (struct ResetHandlerEntry *)msg->entry1;
    rhe2 = (struct ResetHandlerEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = resetlist_cmp2colfunc(rhe2, rhe1, col1);
    } else {
        cmp = resetlist_cmp2colfunc(rhe1, rhe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = resetlist_cmp2colfunc(rhe2, rhe1, col2);
    } else {
        cmp = resetlist_cmp2colfunc(rhe1, rhe2, col2);
    }

    return cmp;
}
MakeStaticHook(resetlist_cmp2hook, resetlist_cmp2func);

STATIC void ASM SAVEDS dummyResetHandler( REG(a1, UNUSED APTR handlerData) )
{
    return;
}

#if defined(__amigaos4__)
STATIC uint32 dummyResetCallback( UNUSED struct ExceptionContext *ctx,
                                  UNUSED struct Library *ExecBase,
                                  UNUSED APTR is_Data )
{
    return 0;
}
#endif

STATIC void ReceiveList( void (* callback)( struct ResetHandlerEntry *rhe, void *userData ),
                         void *userData )
{
    struct ResetHandlerEntry *rhe;

    if ((rhe = tbAllocVecPooled(globalPool, sizeof(struct ResetHandlerEntry))) != NULL) {
        if (SendDaemon("GetResetList")) {
            while (ReceiveDecodedEntry(rhe, sizeof(struct ResetHandlerEntry))) {
                callback(rhe, userData);
            }
        }

        tbFreeVecPooled(globalPool, rhe);
    }
}

STATIC void IterateList( void (* callback)( struct ResetHandlerEntry *rhe, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct ResetHandlerEntry *rhe;
    struct MsgPort *mp;
    struct Interrupt *myirq;

    NewList((struct List *)&tmplist);

    if ((mp = CreateMsgPort()) != NULL) {
        struct IOStdReq *io;

        if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(struct IOStdReq))) != NULL) {
            if (OpenDevice("keyboard.device", 0, (struct IORequest *)io, 0) == 0) {
                if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
                    struct List *irqlist;
                    struct Interrupt *irq;

                    myirq->is_Node.ln_Type = NT_INTERRUPT;
                    myirq->is_Node.ln_Pri = 32;
                    myirq->is_Node.ln_Name = (STRPTR)"Scout";
                    myirq->is_Code = (void (*)())dummyResetHandler;
                    myirq->is_Data = NULL;

                    io->io_Command = KBD_ADDRESETHANDLER;
                    io->io_Data = myirq;

                    DoIO((struct IORequest *)io);

                    Forbid();

                    irqlist = FindListOfNode((struct Node *)myirq);

                    ITERATE_LIST(irqlist, struct Interrupt *, irq) {
                        if (irq != myirq) {
                            if ((rhe = AllocVec(sizeof(struct ResetHandlerEntry), MEMF_CLEAR)) != NULL) {
                                rhe->rhe_Addr = irq;

                                _snprintf(rhe->rhe_Address, sizeof(rhe->rhe_Address), ADDRESS_FORMAT, irq);
                                stccpy(rhe->rhe_Name, nonetest(irq->is_Node.ln_Name), sizeof(rhe->rhe_Name));
                                stccpy(rhe->rhe_Type, GetNodeType(irq->is_Node.ln_Type), sizeof(rhe->rhe_Type));
                                _snprintf(rhe->rhe_Pri, sizeof(rhe->rhe_Pri), "%4ld", irq->is_Node.ln_Pri);
                                _snprintf(rhe->rhe_Data, sizeof(rhe->rhe_Data), ADDRESS_FORMAT, irq->is_Data);
                                if (points2ram((APTR)irq->is_Code)) {
                                   _snprintf(rhe->rhe_Code, sizeof(rhe->rhe_Code), MUIX_PH ADDRESS_FORMAT MUIX_PT, irq->is_Code);
                                } else {
                                   _snprintf(rhe->rhe_Code, sizeof(rhe->rhe_Code), ADDRESS_FORMAT, irq->is_Code);
                                }

                                AddTail((struct List *)&tmplist, (struct Node *)rhe);
                            }
                        }
                    }

                    Permit();

                    io->io_Command = KBD_REMRESETHANDLER;
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

#if defined(__amigaos4__)
    if ((myirq = tbAllocVecPooled(globalPool, sizeof(struct Interrupt))) != NULL) {
        myirq->is_Node.ln_Type = NT_EXTINTERRUPT;
        myirq->is_Node.ln_Pri = 127;
        myirq->is_Node.ln_Name = (STRPTR)"Scout";
        myirq->is_Code = (void (*)())dummyResetCallback;
        myirq->is_Data = NULL;

        if (AddResetCallback(myirq)) {
            struct List *irqlist;
            struct Interrupt *irq;

            Forbid();

            irqlist = FindListOfNode((struct Node *)myirq);

            ITERATE_LIST(irqlist, struct Interrupt *, irq) {
                if (irq != myirq) {
                    if ((rhe = AllocVec(sizeof(struct ResetHandlerEntry), MEMF_CLEAR)) != NULL) {
                        rhe->rhe_Addr = irq;

                        _snprintf(rhe->rhe_Address, sizeof(rhe->rhe_Address), "$%08lx", irq);
                        stccpy(rhe->rhe_Name, nonetest(irq->is_Node.ln_Name), sizeof(rhe->rhe_Name));
                        stccpy(rhe->rhe_Type, GetNodeType(irq->is_Node.ln_Type), sizeof(rhe->rhe_Type));
                        _snprintf(rhe->rhe_Pri, sizeof(rhe->rhe_Pri), "%4ld", irq->is_Node.ln_Pri);
                        _snprintf(rhe->rhe_Data, sizeof(rhe->rhe_Data), "$%08lx", irq->is_Data);
                        if (points2ram((APTR)irq->is_Code)) {
                           _snprintf(rhe->rhe_Code, sizeof(rhe->rhe_Code), MUIX_PH "$%08lx" MUIX_PT, irq->is_Code);
                        } else {
                           _snprintf(rhe->rhe_Code, sizeof(rhe->rhe_Code), "$%08lx", irq->is_Code);
                        }

                        AddTail((struct List *)&tmplist, (struct Node *)rhe);
                    }
                }
            }

            Permit();

            RemResetCallback(myirq);
        }

        tbFreeVecPooled(globalPool, myirq);
    }
#endif

    ITERATE_LIST(&tmplist, struct ResetHandlerEntry *, rhe) {
        callback(rhe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct ResetHandlerEntry *rhe,
                            void *userData )
{
    struct ResetCallbackUserData *ud = (struct ResetCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, rhe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ResetHandlerEntry *rhe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtResetHandlersPrintLine, rhe->rhe_Address, rhe->rhe_Type, rhe->rhe_Pri, rhe->rhe_Data, StripMUIFormatting(rhe->rhe_Code), rhe->rhe_Name);
}

STATIC void SendCallback( struct ResetHandlerEntry *rhe,
                          UNUSED void *userData )
{
    SendEncodedEntry(rhe, sizeof(struct ResetHandlerEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR resetlist, resettext, resetcount, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "ResetHandlers",
        MUIA_Window_ID, MakeID('R','S','T','H'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&resetlist, MakeID('R','S','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR", &resetlist_con2hook, &resetlist_des2hook, &resetlist_dsp2hook, &resetlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&resettext, &resetcount),

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
        struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
        APTR parent;

        rhwd->rhwd_ResetHandlerList = resetlist;
        rhwd->rhwd_ResetHandlerText = resettext;
        rhwd->rhwd_ResetHandlerCount = resetcount;
        rhwd->rhwd_RemoveButton = removeButton;
        rhwd->rhwd_PriorityButton = priorityButton;
        rhwd->rhwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtResetHandlersTitle, rhwd->rhwd_Title, sizeof(rhwd->rhwd_Title)));
        set(obj, MUIA_Window_DefaultObject, resetlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, rhwd->rhwd_RemoveButton,
                                                          rhwd->rhwd_PriorityButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(resetlist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_ResetHandlersWin_ListChange);
        DoMethod(resetlist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_ResetHandlersWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResetHandlersWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResetHandlersWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResetHandlersWin_Remove);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResetHandlersWin_Priority);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResetHandlersWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(rhwd->rhwd_ResetHandlerList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
    struct ResetCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(rhwd->rhwd_ResetHandlerList, MUIA_NList_Quiet, TRUE);
    DoMethod(rhwd->rhwd_ResetHandlerList, MUIM_NList_Clear);

    ud.ud_List = rhwd->rhwd_ResetHandlerList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(rhwd->rhwd_ResetHandlerCount, ud.ud_Count);
    MySetContents(rhwd->rhwd_ResetHandlerText, "");

    set(rhwd->rhwd_ResetHandlerList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, rhwd->rhwd_RemoveButton,
                                                      rhwd->rhwd_PriorityButton,
                                                      rhwd->rhwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintResetHandlers(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
    struct ResetHandlerEntry *rhe;

    if ((rhe = (struct ResetHandlerEntry *)GetActiveEntry(rhwd->rhwd_ResetHandlerList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveResetHandler, rhe->rhe_Name)) {
            if (MyDoCommand("RemoveResetHandler %s", rhe->rhe_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_ResetHandlersWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
    struct ResetHandlerEntry *rhe;

    if ((rhe = (struct ResetHandlerEntry *)GetActiveEntry(rhwd->rhwd_ResetHandlerList)) != NULL) {
        LONG pri;

        pri = atol(rhe->rhe_Pri);
        if (GetPriority(rhe->rhe_Name, &pri)) {
            if (MyDoCommand("SetPriority RESETHANDLER \"%s\" %ld", rhe->rhe_Name, pri) == RETURN_OK) {
                DoMethod(obj, MUIM_ResetHandlersWin_Update);
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
        struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
        struct ResetHandlerEntry *rhe;

        if ((rhe = (struct ResetHandlerEntry *)GetActiveEntry(rhwd->rhwd_ResetHandlerList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)ResetHandlersDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                SetAttrs(detailWin, MUIA_ResetHandlersDetailWin_ResetHandler, rhe,
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
    struct ResetHandlersWinData *rhwd = INST_DATA(cl, obj);
    struct ResetHandlerEntry *rhe;

    if ((rhe = (struct ResetHandlerEntry *)GetActiveEntry(rhwd->rhwd_ResetHandlerList)) != NULL) {
        MySetContents(rhwd->rhwd_ResetHandlerText, "%s \"%s\"", rhe->rhe_Address, rhe->rhe_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, rhwd->rhwd_RemoveButton,
                                                           rhwd->rhwd_PriorityButton,
                                                           (clientstate) ? NULL : rhwd->rhwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(ResetHandlersWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                            return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                        return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_Priority:    return (mPriority(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_ResetHandlersWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintResetHandlers( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtResetHandlersPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendResetHandlersList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeResetHandlersWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ResetHandlersWinData), ENTRY(ResetHandlersWinDispatcher));
}

