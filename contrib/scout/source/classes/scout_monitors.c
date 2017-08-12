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
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#include "system_headers.h"

struct MonitorsWinData {
    TEXT mwd_Title[WINDOW_TITLE_LENGTH];
    APTR mwd_MonitorList;
    APTR mwd_MonitorText;
    APTR mwd_MonitorCount;
    APTR mwd_RemoveButton;
    APTR mwd_PriorityButton;
    APTR mwd_MoreButton;
};

struct InputCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

STRPTR GetNodeSubsystem( UBYTE type )
{
    STATIC TEXT invalidType[64];

    switch (type) {
        case SS_GRAPHICS: return txtNodeSubsystemGraphics;
        default:          _snprintf(invalidType, sizeof(invalidType), txtNodeTypeInvalid, type); return invalidType;
    };
}

STRPTR GetNodeSubtype( UBYTE type )
{
    STATIC TEXT invalidType[64];

    switch (type) {
        case VIEW_EXTRA_TYPE:      return txtNodeSubtypeViewExtra;
        case VIEWPORT_EXTRA_TYPE:  return txtNodeSubTypeViewPortExtra;
        case SPECIAL_MONITOR_TYPE: return txtNodeSubTypeSpecialMonitor;
        case MONITOR_SPEC_TYPE:    return txtNodeSubTypeMonitorSpec;
	default:                   _snprintf(invalidType, sizeof(invalidType), txtNodeTypeInvalid, type); return invalidType;
    };
}

HOOKPROTONHNO(monitorlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct MonitorEntry));
}
MakeStaticHook(monitorlist_con2hook, monitorlist_con2func);

HOOKPROTONHNO(monitorlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(monitorlist_des2hook, monitorlist_des2func);

HOOKPROTONHNO(monitorlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct MonitorEntry *me = (struct MonitorEntry *)msg->entry;

    if (me) {
        msg->strings[0] = me->mon_Address;
        msg->strings[1] = me->mon_Name;
        msg->strings[2] = me->mon_Type;
        msg->strings[3] = me->mon_Pri;
        msg->strings[4] = me->mon_Subsystem;
        msg->strings[5] = me->mon_Subtype;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtNodeSubsystem;
        msg->strings[5] = txtNodeSubtype;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(monitorlist_dsp2hook, monitorlist_dsp2func);

STATIC LONG monitorlist_cmp2colfunc( struct MonitorEntry *me1,
                                   struct MonitorEntry *me2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(me1->mon_Address, me2->mon_Address);
        case 1: return stricmp(me1->mon_Name, me2->mon_Name);
        case 2: return stricmp(me1->mon_Type, me2->mon_Type);
        case 3: return PriorityCompare(me1->mon_Pri, me2->mon_Pri);
    }
}

HOOKPROTONHNO(monitorlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct MonitorEntry *me1, *me2;
    ULONG col1, col2;

    me1 = (struct MonitorEntry *)msg->entry1;
    me2 = (struct MonitorEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = monitorlist_cmp2colfunc(me2, me1, col1);
    } else {
        cmp = monitorlist_cmp2colfunc(me1, me2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = monitorlist_cmp2colfunc(me2, me1, col2);
    } else {
        cmp = monitorlist_cmp2colfunc(me1, me2, col2);
    }

    return cmp;
}
MakeStaticHook(monitorlist_cmp2hook, monitorlist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct MonitorEntry *me, void *userData ),
                         void *userData )
{
    struct MonitorEntry *me;

    if ((me = tbAllocVecPooled(globalPool, sizeof(struct MonitorEntry))) != NULL) {
        if (SendDaemon("GetMonitorList")) {
            while (ReceiveDecodedEntry(me, sizeof(struct MonitorEntry))) {
                callback(me, userData);
            }
        }

        tbFreeVecPooled(globalPool, me);
    }
}

STATIC void IterateList( void (* callback)( struct MonitorEntry *me, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct MonitorEntry *me;
    struct MonitorSpec *mspc;

    NewList((struct List *)&tmplist);

    ObtainSemaphoreShared(GfxBase->MonitorListSemaphore);

    ITERATE_LIST(&GfxBase->MonitorList, struct MonitorSpec *, mspc) {
        if ((me = AllocVec(sizeof(struct MonitorEntry), MEMF_CLEAR)) != NULL) {
            me->mon_Addr = mspc;

            _snprintf(me->mon_Address, sizeof(me->mon_Address), ADDRESS_FORMAT, mspc);
            stccpy(me->mon_Name, nonetest(mspc->ms_Node.xln_Name), sizeof(me->mon_Name));
            stccpy(me->mon_Type, GetNodeType(mspc->ms_Node.xln_Type), sizeof(me->mon_Type));
            _snprintf(me->mon_Pri, sizeof(me->mon_Pri), "%4ld", mspc->ms_Node.xln_Pri);
	    stccpy(me->mon_Subsystem, GetNodeSubsystem(mspc->ms_Node.xln_Subsystem), sizeof(me->mon_Subsystem));
	    stccpy(me->mon_Subtype, GetNodeSubtype(mspc->ms_Node.xln_Subtype), sizeof(me->mon_Subtype));

            AddTail((struct List *)&tmplist, (struct Node *)me);
        }
    }

    ReleaseSemaphore(GfxBase->MonitorListSemaphore);

    ITERATE_LIST(&tmplist, struct MonitorEntry *, me) {
        callback(me, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct MonitorEntry *me,
                            void *userData )
{
    struct InputCallbackUserData *ud = (struct InputCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, me);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct MonitorEntry *me,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtMonitorsPrintLine, me->mon_Address, me->mon_Type, me->mon_Pri, me->mon_Subsystem, me->mon_Subtype, me->mon_Name);
}

STATIC void SendCallback( struct MonitorEntry *me,
                          UNUSED void *userData )
{
    SendEncodedEntry(me, sizeof(struct MonitorEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR monitorlist, monitortext, inputcount, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Monitors",
        MUIA_Window_ID, MakeID('M','O','N','I'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&monitorlist, MakeID('M','N','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR", &monitorlist_con2hook, &monitorlist_des2hook, &monitorlist_dsp2hook, &monitorlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&monitortext, &inputcount),

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
        struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
        APTR parent;

        ihwd->mwd_MonitorList = monitorlist;
        ihwd->mwd_MonitorText = monitortext;
        ihwd->mwd_MonitorCount = inputcount;
        ihwd->mwd_RemoveButton = removeButton;
        ihwd->mwd_PriorityButton = priorityButton;
        ihwd->mwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtMonitorsTitle, ihwd->mwd_Title, sizeof(ihwd->mwd_Title)));
        set(obj, MUIA_Window_DefaultObject, monitorlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, ihwd->mwd_RemoveButton,
                                                          ihwd->mwd_PriorityButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(monitorlist,    MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_MonitorsWin_ListChange);
        DoMethod(monitorlist,    MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_MonitorsWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorsWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorsWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorsWin_Remove);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorsWin_Priority);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorsWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct MonitorsWinData *ihwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(ihwd->mwd_MonitorList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
    struct InputCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(ihwd->mwd_MonitorList, MUIA_NList_Quiet, TRUE);
    DoMethod(ihwd->mwd_MonitorList, MUIM_NList_Clear);

    ud.ud_List = ihwd->mwd_MonitorList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    SetCountText(ihwd->mwd_MonitorCount, ud.ud_Count);
    MySetContents(ihwd->mwd_MonitorText, "");

    set(ihwd->mwd_MonitorList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, ihwd->mwd_RemoveButton,
                                                      ihwd->mwd_PriorityButton,
                                                      ihwd->mwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintMonitors(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
    struct MonitorEntry *me;

    if ((me = (struct MonitorEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveMonitor, me->mon_Name)) {
            if (MyDoCommand("RemoveMonitor %s", me->mon_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_MonitorsWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
    struct MonitorEntry *me;

    if ((me = (struct MonitorEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {
        LONG pri;

        pri = atol(me->mon_Pri);
        if (GetPriority(me->mon_Name, &pri)) {
            if (MyDoCommand("SetPriority MONITOR \"%s\" %ld", me->mon_Name, pri) == RETURN_OK) {
                DoMethod(obj, MUIM_MonitorsWin_Update);
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
        struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
        struct MonitorEntry *me;

        if ((me = (struct MonitorEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {
            APTR detailWin;

	    if ((detailWin = (Object *)MonitorsDetailWindowObject,
		MUIA_Window_ParentWindow, (IPTR)obj,
		MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
		End) != NULL) {
		COLLECT_RETURNIDS;
		SetAttrs(detailWin, MUIA_MonitorsDetailWin_MonitorSpec, me->mon_Addr,
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
    struct MonitorsWinData *ihwd = INST_DATA(cl, obj);
    struct MonitorEntry *me;

    if ((me = (struct MonitorEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {
        MySetContents(ihwd->mwd_MonitorText, "%s \"%s\"", me->mon_Address, me->mon_Name);
	DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, ihwd->mwd_RemoveButton, ihwd->mwd_PriorityButton, NULL);
	if (!clientstate)
	    SetAttrs(ihwd->mwd_MoreButton, MUIA_Disabled, FALSE, TAG_DONE);
    }

    return 0;
}

DISPATCHER(MonitorsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_Remove:      return (mRemove(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_Priority:    return (mPriority(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_MonitorsWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintMonitors( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtMonitorsPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendMonitorsList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeMonitorsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MonitorsWinData), ENTRY(MonitorsWinDispatcher));
}
