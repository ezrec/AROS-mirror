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

struct MonitorClassWinData {
    TEXT mwd_Title[WINDOW_TITLE_LENGTH];
    APTR mwd_MonitorList;
    APTR mwd_MonitorText;
    APTR mwd_MonitorCount;
    APTR mwd_MoreButton;
};

struct InputCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(monitorlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct MonitorClassEntry));
}
MakeStaticHook(monitorlist_con2hook, monitorlist_con2func);

HOOKPROTONHNO(monitorlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(monitorlist_des2hook, monitorlist_des2func);

HOOKPROTONHNO(monitorlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct MonitorClassEntry *me = (struct MonitorClassEntry *)msg->entry;

    if (me) {
        msg->strings[0] = me->mon_Address;
        msg->strings[1] = me->mon_Name;
   } else {
        msg->strings[0] = txtAddress;
	msg->strings[1] = txtName;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
   }
}
MakeStaticHook(monitorlist_dsp2hook, monitorlist_dsp2func);

STATIC LONG monitorlist_cmp2colfunc( struct MonitorClassEntry *me1,
                                   struct MonitorClassEntry *me2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(me1->mon_Address, me2->mon_Address);
        case 1: return stricmp(me1->mon_Name, me2->mon_Name);
    }
}

HOOKPROTONHNO(monitorlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct MonitorClassEntry *me1, *me2;
    ULONG col1, col2;

    me1 = (struct MonitorClassEntry *)msg->entry1;
    me2 = (struct MonitorClassEntry *)msg->entry2;
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

STATIC void ReceiveList( void (* callback)( struct MonitorClassEntry *me, void *userData ),
                         void *userData )
{
    struct MonitorClassEntry *me;

    if ((me = tbAllocVecPooled(globalPool, sizeof(struct MonitorClassEntry))) != NULL) {
	if (SendDaemon("GetMonitorClassList")) {
            while (ReceiveDecodedEntry(me, sizeof(struct MonitorClassEntry))) {
                callback(me, userData);
            }
        }

        tbFreeVecPooled(globalPool, me);
    }
}

STATIC void IterateList( void (* callback)( struct MonitorClassEntry *me, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct MonitorClassEntry *me;
    Object **monList;

#ifdef HAVE_MONITORCLASS
    if (IntuitionBase->LibNode.lib_Version < 50)
	return;

    NewList((struct List *)&tmplist);

    monList = GetMonitorList(NULL);
    if (monList) {
	ULONG i;

	for (i = 0; monList[i]; i++) {
	    me = AllocVec(sizeof(struct MonitorClassEntry), MEMF_CLEAR);
	    if (me) {
		STRPTR name = NULL;

		GetAttr(MA_MonitorName, monList[i], (IPTR *)&name);
		me->mon_Addr = monList[i];

		_snprintf(me->mon_Address, sizeof(me->mon_Address), ADDRESS_FORMAT, monList[i]);
		stccpy(me->mon_Name, nonetest(name), sizeof(me->mon_Name));

		AddTail((struct List *)&tmplist, (struct Node *)me);
	    }
	}
	FreeMonitorList(monList);
    }
    ITERATE_LIST(&tmplist, struct MonitorClassEntry *, me) {
        callback(me, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
#endif
}

STATIC void UpdateCallback( struct MonitorClassEntry *me,
                            void *userData )
{
    struct InputCallbackUserData *ud = (struct InputCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, me);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct MonitorClassEntry *me,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtMonitorClassPrintLine, me->mon_Address, me->mon_Name);
}

STATIC void SendCallback( struct MonitorClassEntry *me,
                          UNUSED void *userData )
{
    SendEncodedEntry(me, sizeof(struct MonitorClassEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR monitorlist, monitortext, inputcount, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "MonitorClass",
	MUIA_Window_ID, MakeID('M','C','L','S'),
        WindowContents, VGroup,

	    Child, (IPTR)MyNListviewObject(&monitorlist, MakeID('M','C','L','V'), "BAR,BAR", &monitorlist_con2hook, &monitorlist_des2hook, &monitorlist_dsp2hook, &monitorlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&monitortext, &inputcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MonitorClassWinData *ihwd = INST_DATA(cl, obj);
        APTR parent;

        ihwd->mwd_MonitorList = monitorlist;
        ihwd->mwd_MonitorText = monitortext;
        ihwd->mwd_MonitorCount = inputcount;
        ihwd->mwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

	set(obj, MUIA_Window_Title, MyGetWindowTitle(txtMonitorClassTitle, ihwd->mwd_Title, sizeof(ihwd->mwd_Title)));
        set(obj, MUIA_Window_DefaultObject, monitorlist);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(monitorlist,    MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_MonitorClassWin_ListChange);
        DoMethod(monitorlist,    MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_MonitorClassWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorClassWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorClassWin_Print);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MonitorClassWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct MonitorClassWinData *ihwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(ihwd->mwd_MonitorList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct MonitorClassWinData *ihwd = INST_DATA(cl, obj);
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
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, ihwd->mwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintMonitorClass(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct MonitorClassWinData *ihwd = INST_DATA(cl, obj);
        struct MonitorClassEntry *me;

        if ((me = (struct MonitorClassEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {
            APTR detailWin;

	    if ((detailWin = (Object *)MonitorClassDetailWindowObject,
		    MUIA_Window_ParentWindow, (IPTR)obj,
		    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
		End) != NULL) {
		COLLECT_RETURNIDS;
		SetAttrs(detailWin, MUIA_MonitorClassDetailWin_MonitorObject, me->mon_Addr,
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
    struct MonitorClassWinData *ihwd = INST_DATA(cl, obj);
    struct MonitorClassEntry *me;

    if ((me = (struct MonitorClassEntry *)GetActiveEntry(ihwd->mwd_MonitorList)) != NULL) {

        MySetContents(ihwd->mwd_MonitorText, "%s \"%s\"", me->mon_Address, me->mon_Name);
	if (!clientstate)
	    SetAttrs(ihwd->mwd_MoreButton, MUIA_Disabled, FALSE, TAG_DONE);
    }

    return 0;
}

DISPATCHER(MonitorClassWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_MonitorClassWin_Update:      return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_MonitorClassWin_Print:       return (mPrint(cl, obj, (APTR)msg));
        case MUIM_MonitorClassWin_More:        return (mMore(cl, obj, (APTR)msg));
        case MUIM_MonitorClassWin_ListChange:  return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintMonitorClass( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtMonitorClassPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendMonitorClassList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeMonitorClassWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MonitorClassWinData), ENTRY(MonitorClassWinDispatcher));
}

