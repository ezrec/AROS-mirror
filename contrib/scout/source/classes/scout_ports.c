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

struct PortsWinData {
    TEXT pwd_Title[WINDOW_TITLE_LENGTH];
    APTR pwd_PortList;
    APTR pwd_PortText;
    APTR pwd_PortCount;
    APTR pwd_RemoveButton;
    APTR pwd_PriorityButton;
    APTR pwd_MoreButton;
};

struct PortsCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(portslist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct PortEntry));
}
MakeStaticHook(portslist_con2hook, portslist_con2func);

HOOKPROTONHNO(portslist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(portslist_des2hook, portslist_des2func);

HOOKPROTONHNO(portslist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct PortEntry *pe = (struct PortEntry *)msg->entry;

    if (pe) {
        msg->strings[0] = pe->pe_Address;
        msg->strings[1] = pe->pe_Name;
        msg->strings[2] = pe->pe_Pri;
        msg->strings[3] = pe->pe_Flags;
        msg->strings[4] = pe->pe_SigBit;
        msg->strings[5] = pe->pe_SigTask;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodePri;
        msg->strings[3] = txtPortFlags;
        msg->strings[4] = txtPortSigBit;
        msg->strings[5] = txtPortSigTask;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(portslist_dsp2hook, portslist_dsp2func);

STATIC LONG portslist_cmp2colfunc( struct PortEntry *pe1,
                                   struct PortEntry *pe2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(pe1->pe_Address, pe2->pe_Address);
        case 1: return stricmp(pe1->pe_Name, pe2->pe_Name);
        case 2: return PriorityCompare(pe1->pe_Pri, pe2->pe_Pri);
        case 3: return stricmp(pe1->pe_Flags, pe2->pe_Flags);
        case 4: return IntegerCompare(pe1->pe_SigBit, pe2->pe_SigBit);
        case 5: return stricmp(pe1->pe_SigTask, pe2->pe_SigTask);
    }
}

STATIC LONG portlist_cmpfunc( const struct Node *n1,
                              const struct Node *n2 )
{
    return stricmp(((struct PortEntry *)n1)->pe_Name, ((struct PortEntry *)n2)->pe_Name);
}

HOOKPROTONHNO(portslist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct PortEntry *pe1, *pe2;
    ULONG col1, col2;

    pe1 = (struct PortEntry *)msg->entry1;
    pe2 = (struct PortEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = portslist_cmp2colfunc(pe2, pe1, col1);
    } else {
        cmp = portslist_cmp2colfunc(pe1, pe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = portslist_cmp2colfunc(pe2, pe1, col2);
    } else {
        cmp = portslist_cmp2colfunc(pe1, pe2, col2);
    }

    return cmp;
}
MakeStaticHook(portslist_cmp2hook, portslist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct PortEntry *pe, void *userData ),
                         void *userData )
{
    struct PortEntry *pe;

    if ((pe = tbAllocVecPooled(globalPool, sizeof(struct PortEntry))) != NULL) {
        if (SendDaemon("GetPortList")) {
            while (ReceiveDecodedEntry(pe, sizeof(struct PortEntry))) {
                callback(pe, userData);
            }
        }

        tbFreeVecPooled(globalPool, pe);
    }
}

STATIC void IterateList( void (* callback)( struct PortEntry *pe, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MsgPort *mp;
    struct MinList tmplist;
    struct PortEntry *pe;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->PortList, struct MsgPort *, mp) {
        if ((pe = AllocVec(sizeof(struct PortEntry), MEMF_CLEAR)) != NULL) {
            pe->pe_Addr = mp;

            _snprintf(pe->pe_Address, sizeof(pe->pe_Address), ADDRESS_FORMAT, mp);
            stccpy(pe->pe_Name, nonetest(mp->mp_Node.ln_Name), sizeof(pe->pe_Name));
            _snprintf(pe->pe_Pri, sizeof(pe->pe_Pri), "%4ld", mp->mp_Node.ln_Pri);
            _snprintf(pe->pe_SigBit, sizeof(pe->pe_SigBit), "%2ld", mp->mp_SigBit);
            GetTaskName((struct Task *)mp->mp_SigTask, pe->pe_SigTask, sizeof(pe->pe_SigTask));
            healstring(pe->pe_SigTask);
            switch(mp->mp_Flags & PF_ACTION) {
                case PA_SIGNAL : stccpy(pe->pe_Flags, txtPortFlagsSignal, sizeof(pe->pe_Flags)); break;
                case PA_SOFTINT: stccpy(pe->pe_Flags, txtPortFlagsSoftInt, sizeof(pe->pe_Flags)); break;
                case PA_IGNORE : stccpy(pe->pe_Flags, txtPortFlagsIgnore, sizeof(pe->pe_Flags)); break;
            }

            AddTail((struct List *)&tmplist, (struct Node *)pe);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, portlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct PortEntry *, pe) {
        callback(pe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct PortEntry *pe,
                            void *userData )
{
    struct PortsCallbackUserData *ud = (struct PortsCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, pe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct PortEntry *pe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtPortsPrintLine, pe->pe_Address, pe->pe_Name, pe->pe_Pri, pe->pe_SigBit, pe->pe_SigTask);
}

STATIC void SendCallback( struct PortEntry *pe,
                          UNUSED void *userData )
{
    SendEncodedEntry(pe, sizeof(struct PortEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR portslist, portstext, portscount, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Ports",
        MUIA_Window_ID, MakeID('P','O','R','T'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&portslist, MakeID('P','O','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR", &portslist_con2hook, &portslist_des2hook, &portslist_dsp2hook, &portslist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&portstext, &portscount),

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
        struct PortsWinData *pwd = INST_DATA(cl, obj);
        APTR parent;

        pwd->pwd_PortList = portslist;
        pwd->pwd_PortText = portstext;
        pwd->pwd_PortCount = portscount;
        pwd->pwd_RemoveButton = removeButton;
        pwd->pwd_PriorityButton = priorityButton;
        pwd->pwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtPortsTitle, pwd->pwd_Title, sizeof(pwd->pwd_Title)));
        set(obj, MUIA_Window_DefaultObject, portslist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, pwd->pwd_RemoveButton,
                                                          pwd->pwd_PriorityButton,
                                                          pwd->pwd_MoreButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(portslist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_PortsWin_ListChange);
        DoMethod(portslist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_PortsWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PortsWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PortsWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PortsWin_Remove);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PortsWin_More);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_PortsWin_Priority);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(portslist,      MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct PortsWinData *pwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(pwd->pwd_PortList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct PortsWinData *pwd = INST_DATA(cl, obj);
    struct PortsCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(pwd->pwd_PortList, MUIA_NList_Quiet, TRUE);
    DoMethod(pwd->pwd_PortList, MUIM_NList_Clear);

    ud.ud_List = pwd->pwd_PortList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(pwd->pwd_PortCount, ud.ud_Count);
    MySetContents(pwd->pwd_PortText, "");

    set(pwd->pwd_PortList, MUIA_NList_Quiet, FALSE);
    set(pwd->pwd_PortList, MUIA_NList_Active, MUIV_NList_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, pwd->pwd_RemoveButton,
                                                      pwd->pwd_PriorityButton,
                                                      pwd->pwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintPorts(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct PortsWinData *pwd = INST_DATA(cl, obj);
    struct PortEntry *pe;

    if ((pe = (struct PortEntry *)GetActiveEntry(pwd->pwd_PortList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemovePort, pe->pe_Name)) {
            if (MyDoCommand("RemovePort %s", pe->pe_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_PortsWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct PortsWinData *pwd = INST_DATA(cl, obj);
    struct PortEntry *pe;

    if ((pe = (struct PortEntry *)GetActiveEntry(pwd->pwd_PortList)) != NULL) {
        LONG pri;

        pri = atol(pe->pe_Pri);
        if (GetPriority(pe->pe_Name, &pri)) {
            if (MyDoCommand("SetPriority PORT %s %ld", pe->pe_Address, pri) == RETURN_OK) {
                _snprintf(pe->pe_Pri, sizeof(pe->pe_Pri), "%4ld", pri);
                RedrawActiveEntry(pwd->pwd_PortList);
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
        struct PortsWinData *pwd = INST_DATA(cl, obj);
        struct PortEntry *pe;

        if ((pe = (struct PortEntry *)GetActiveEntry(pwd->pwd_PortList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)PortsDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                SetAttrs(detailWin, MUIA_PortsDetailWin_Port, pe,
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
    struct PortsWinData *pwd = INST_DATA(cl, obj);
    struct PortEntry *pe;

    if ((pe = (struct PortEntry *)GetActiveEntry(pwd->pwd_PortList)) != NULL) {
        MySetContents(pwd->pwd_PortText, "%s \"%s\"", pe->pe_Address, pe->pe_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, pwd->pwd_RemoveButton,
                                                           pwd->pwd_PriorityButton,
                                                           (clientstate) ? NULL : pwd->pwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(PortsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:               return (mDispose(cl, obj, (APTR)msg));
        case MUIM_PortsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_PortsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_PortsWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_PortsWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_PortsWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_PortsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintPorts( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtPortsPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendPortList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakePortsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct PortsWinData), ENTRY(PortsWinDispatcher));
}

