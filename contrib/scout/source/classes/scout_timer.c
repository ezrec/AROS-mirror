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

struct TimersWinData {
    TEXT twd_Title[WINDOW_TITLE_LENGTH];
    APTR twd_TimerList;
    APTR twd_TimerText;
    APTR twd_TimerCount;
    APTR twd_AbortButton;
};

struct TimersCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

typedef union {
    struct TimeVal tv;
    struct EClockVal ev;
#if defined(USE_NATIVE_64BIT_MATH)
    uint64 u64;
#else
    bigint u64;
#endif
#if defined(__MORPHOS__)
    QUAD q64;
#endif
} EClockUnion;

HOOKPROTONHNO(timerlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct TimerEntry));
}
MakeStaticHook(timerlist_con2hook, timerlist_con2func);

HOOKPROTONHNO(timerlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(timerlist_des2hook, timerlist_des2func);

HOOKPROTONHNO(timerlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct TimerEntry *te = (struct TimerEntry *)msg->entry;

    if (te) {
        msg->strings[0] = te->te_Address;
        msg->strings[1] = te->te_Name;
        msg->strings[2] = te->te_Timeout;
        msg->strings[3] = te->te_Unit;
        msg->strings[4] = te->te_ReplyPort;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtTask;
        msg->strings[2] = txtTime;
        msg->strings[3] = txtUnit;
        msg->strings[4] = txtReplyPort;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(timerlist_dsp2hook, timerlist_dsp2func);

STATIC LONG timerlist_cmp2colfunc( struct TimerEntry *te1,
                                   struct TimerEntry *te2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(te1->te_Address, te2->te_Address);
        case 1: return stricmp(te1->te_Name, te2->te_Name);
        case 2: return stricmp(te1->te_Timeout, te2->te_Timeout);
        case 3: return stricmp(te1->te_Unit, te2->te_Unit);
        case 4: return HexCompare(te1->te_ReplyPort, te2->te_ReplyPort);
    }
}

STATIC LONG timerlist_cmpfunc( const struct Node *n1,
                               const struct Node *n2 )
{
    return stricmp(((struct TimerEntry *)n1)->te_Name, ((struct TimerEntry *)n2)->te_Name);
}

HOOKPROTONHNO(timerlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct TimerEntry *te1, *te2;
    ULONG col1, col2;

    te1 = (struct TimerEntry *)msg->entry1;
    te2 = (struct TimerEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = timerlist_cmp2colfunc(te2, te1, col1);
    } else {
        cmp = timerlist_cmp2colfunc(te1, te2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = timerlist_cmp2colfunc(te2, te1, col2);
    } else {
        cmp = timerlist_cmp2colfunc(te1, te2, col2);
    }

    return cmp;
}
MakeStaticHook(timerlist_cmp2hook, timerlist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct TimerEntry *te, void *userData ),
                         void *userData )
{
    struct TimerEntry *te;

    if ((te = tbAllocVecPooled(globalPool, sizeof(struct TimerEntry))) != NULL) {
        if (SendDaemon("GetTimerList")) {
            while (ReceiveDecodedEntry(te, sizeof(struct TimerEntry))) {
                callback(te, userData);
            }
        }

        tbFreeVecPooled(globalPool, te);
    }
}

STATIC void IterateList( void (* callback)( struct TimerEntry *te, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct TimerEntry *te;
    struct MsgPort *mp;

    NewList((struct List *)&tmplist);

    if ((mp = CreateMsgPort()) != NULL) {
        struct TimeRequest *mytr;

        if ((mytr = (struct TimeRequest *)CreateIORequest(mp, sizeof(struct TimeRequest))) != NULL) {
        #if !defined(USE_NATIVE_64BIT_MATH)
            bigint _hundret, _sixty;

            _hundret = i64_set(100);
            _sixty = i64_set(60);
        #endif

            if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)mytr, 0) == 0) {
                struct Device *TimerBase = mytr->Request.io_Device;
            #if defined(__amigaos4__)
                struct TimerIFace *ITimer;
            #endif

                if (GETINTERFACE(ITimer, TimerBase)) {
                    ULONG eclk;
                    struct List *trlist;
                    struct TimeRequest *tr;
                    EClockUnion ecu;
                #if !defined(USE_NATIVE_64BIT_MATH)
                    bigint _eclk;
                #endif

                    mytr->Request.io_Command = TR_ADDREQUEST;
                    mytr->Time.Seconds = 10;
                    mytr->Time.Microseconds = 0;

                    Forbid();

                    SendIO((struct IORequest *)mytr);

                    trlist = FindListOfNode((struct Node *)mytr);

                #if defined( __MORPHOS__)
                    eclk = ReadCPUClock(&ecu.q64);
                #else
                    eclk = ReadEClock(&ecu.ev);
                #endif
                #if !defined(USE_NATIVE_64BIT_MATH)
                    _eclk = i64_set(eclk);
                #endif

                    ITERATE_LIST(trlist, struct TimeRequest *, tr) {
                        if (tr != mytr) {
                            if ((te = AllocVec(sizeof(struct TimerEntry), MEMF_CLEAR)) != NULL) {
                            #if defined(USE_NATIVE_64BIT_MATH)
                                uint64 _time, _std, _min, _sec, _hun, quot, rem;

                                _time = (uint64)tr->Time.Seconds << 32 | tr->Time.Microseconds;

                                if (_time > ecu.u64)
                                    _time = _time - ecu.u64;
                                else
                                    _time = 0;

                                quot = _time / (uint64)eclk;
                                rem  = _time % eclk;

                                _min = quot / 60;
                                _sec = quot % 60;
                                _std = _min / 60;
                                _min = _min % 60;
                                _hun = (rem * 100) / eclk;
                            #else
                                bigint _time, _std, _min, _sec, _hun, _tmp, _rem;

                                _time.hi = tr->Time.Seconds;
                                _time.lo = tr->Time.Microseconds;

                                // time = time - eclock
                                _tmp = i64_sub(_time, ecu.u64);
                                i64_div(_tmp, _eclk, &_sec, &_rem);

                                // min = sec / 60; sec %= 60;
                                // std = min / 60; min %= 60;
                                i64_div(_sec, _sixty, &_min, &_sec);
                                i64_div(_min, _sixty, &_std, &_min);
                                _tmp = i64_mul(_rem, _hundret);
                                i64_div(_tmp, _eclk, &_hun, &_rem);
                            #endif

                                te->te_Addr = tr;
                                _snprintf(te->te_Address, sizeof(te->te_Address), ADDRESS_FORMAT, tr);
                                GetTaskName(tr->Request.io_Message.mn_ReplyPort->mp_SigTask, te->te_Name, sizeof(te->te_Name));
                                _snprintf(te->te_ReplyPort, sizeof(te->te_ReplyPort), ADDRESS_FORMAT, tr->Request.io_Message.mn_ReplyPort);
                                stccpy(te->te_Unit, txtTimerMicroHz, sizeof(te->te_Unit));
                            #if defined(USE_NATIVE_64BIT_MATH)
                                _snprintf(te->te_Timeout, sizeof(te->te_Timeout), "%2lld:%02lld'%02lld.%02lld\"", _std, _min, _sec, _hun);
                            #else
                                _snprintf(te->te_Timeout, sizeof(te->te_Timeout), "%2ld:%02ld'%02ld.%02ld\"", _std.lo, _min.lo, _sec.lo, _hun.lo);
                            #endif

                                AddTail((struct List *)&tmplist, (struct Node *)te);
                            }
                        }
                    }

                    Permit();

                    if (!CheckIO((struct IORequest *)mytr)) {
                        AbortIO((struct IORequest *)mytr);
                    }
                    WaitIO((struct IORequest *)mytr);

                    DROPINTERFACE(ITimer);
                }

                CloseDevice((struct IORequest *)mytr);
            }

        #if !defined(__amigaos4__) || defined(__MORPHOS__)
            if (!amigaOS4 && OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)mytr, 0) == 0) {
                struct Device *TimerBase = mytr->Request.io_Device;
            #if defined(__amigaos4__)
                struct TimerIFace *ITimer;
            #endif

                if (GETINTERFACE(ITimer, TimerBase)) {
                    ULONG eclk;
                    struct List *trlist;
                    struct TimeRequest *tr;
                    EClockUnion ecu;
                #if !defined(USE_NATIVE_64BIT_MATH)
                    bigint _eclk;
                #endif

                    mytr->Request.io_Command = TR_ADDREQUEST;
                    mytr->Time.Seconds = 10;
                    mytr->Time.Microseconds = 0;

                    Forbid();

                    SendIO((struct IORequest *)mytr);

                    trlist = FindListOfNode((struct Node *)mytr);

                #if defined( __MORPHOS__)
                    eclk = ReadCPUClock(&ecu.q64);
                #else
                    eclk = ReadEClock(&ecu.ev);
                #endif
                #if !defined(USE_NATIVE_64BIT_MATH)
                    _eclk = i64_set(eclk);
                #endif

                    ITERATE_LIST(trlist, struct TimeRequest *, tr) {
                        if (tr != mytr) {
                            if ((te = AllocVec(sizeof(struct TimerEntry), MEMF_CLEAR)) != NULL) {
                            #if defined(USE_NATIVE_64BIT_MATH)
                                uint64 _time, _std, _min, _sec, _hun, quot, rem;

                                _time = ((uint64)tr->Time.Seconds) << 32 | tr->Time.Microseconds;

                                if (_time < ecu.u64)
                                    _time = 0;
                                else
                                    _time = _time - ecu.u64;

                                quot = _time / eclk;
                                rem  = _time % eclk;

                                _min = quot / 60;
                                _sec = quot % 60;
                                _std = _min / 60;
                                _min = _min % 60;
                                _hun = (rem * 100) / eclk;
                            #else
                                bigint _time, _std, _min, _sec, _hun, _tmp, _rem;

                                _time.hi = tr->Time.Seconds;
                                _time.lo = tr->Time.Microseconds;

                                // time = time - eclock
                                _tmp = i64_sub(_time, ecu.u64);
                                i64_div(_tmp, _eclk, &_sec, &_rem);

                                // min = sec / 60; sec %= 60;
                                // std = min / 60; min %= 60;
                                i64_div(_sec, _sixty, &_min, &_sec);
                                i64_div(_min, _sixty, &_std, &_min);
                                _tmp = i64_mul(_rem, _hundret);
                                i64_div(_tmp, _eclk, &_hun, &_rem);
                            #endif

                                te->te_Addr = tr;
                                _snprintf(te->te_Address, sizeof(te->te_Address), ADDRESS_FORMAT, tr);
                                GetTaskName(tr->Request.io_Message.mn_ReplyPort->mp_SigTask, te->te_Name, sizeof(te->te_Name));
                                _snprintf(te->te_ReplyPort, sizeof(te->te_ReplyPort), ADDRESS_FORMAT, tr->Request.io_Message.mn_ReplyPort);
                                stccpy(te->te_Unit, txtTimerVBlank, sizeof(te->te_Unit));
                            #if defined(USE_NATIVE_64BIT_MATH)
                                _snprintf(te->te_Timeout, sizeof(te->te_Timeout), "%2lld:%02lld'%02lld.%02lld\"", _std, _min, _sec, _hun);
                            #else
                                _snprintf(te->te_Timeout, sizeof(te->te_Timeout), "%2ld:%02ld'%02ld.%02ld\"", _std.lo, _min.lo, _sec.lo, _hun.lo);
                            #endif

                                AddTail((struct List *)&tmplist, (struct Node *)te);
                            }
                        }
                    }

                    Permit();

                    if (!CheckIO((struct IORequest *)mytr)) {
                        AbortIO((struct IORequest *)mytr);
                    }
                    WaitIO((struct IORequest *)mytr);

                    DROPINTERFACE(ITimer);
                }
            }
        #endif

            DeleteIORequest((struct IORequest *)mytr);
        }

        DeleteMsgPort(mp);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, timerlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct TimerEntry *, te) {
        callback(te, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct TimerEntry *te,
                            void *userData )
{
    struct TimersCallbackUserData *ud = (struct TimersCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, te);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct TimerEntry *te,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtTimerPrintLine, te->te_Address, te->te_ReplyPort, te->te_Timeout, te->te_Unit, te->te_Name);
}

STATIC void SendCallback( struct TimerEntry *te,
                          UNUSED void *userData )
{
    SendEncodedEntry(te, sizeof(struct TimerEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR timerlist, timertext, timercount, updateButton, printButton, abortButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Timer",
        MUIA_Window_ID, MakeID('T','I','M','R'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&timerlist, MakeID('T','I','L','V'), "BAR,BAR,BAR,BAR P=" MUIX_C ",BAR", &timerlist_con2hook, &timerlist_des2hook, &timerlist_dsp2hook, &timerlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&timertext, &timercount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(abortButton = MakeButton(txtAbort)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct TimersWinData *twd = INST_DATA(cl, obj);
        APTR parent;

        twd->twd_TimerList = timerlist;
        twd->twd_TimerText = timertext;
        twd->twd_TimerCount = timercount;
        twd->twd_AbortButton = abortButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtTimersTitle, twd->twd_Title, sizeof(twd->twd_Title)));
        set(obj, MUIA_Window_DefaultObject, timerlist);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(timerlist,    MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_TimersWin_ListChange);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TimersWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TimersWin_Print);
        DoMethod(abortButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TimersWin_Abort);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(timerlist,    MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct TimersWinData *twd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(twd->twd_TimerList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct TimersWinData *twd = INST_DATA(cl, obj);
    struct TimersCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(twd->twd_TimerList, MUIA_NList_Quiet, TRUE);
    DoMethod(twd->twd_TimerList, MUIM_NList_Clear);

    ud.ud_List = twd->twd_TimerList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(twd->twd_TimerCount, ud.ud_Count);
    MySetContents(twd->twd_TimerText, "");

    set(twd->twd_TimerList, MUIA_NList_Quiet, FALSE);
    set(twd->twd_AbortButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintTimer(NULL);

    return 0;
}

STATIC IPTR mAbort( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct TimersWinData *twd = INST_DATA(cl, obj);
    struct TimerEntry *te;

    if ((te = (struct TimerEntry *)GetActiveEntry(twd->twd_TimerList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToAbortTimer, te->te_Name)) {
            if (MyDoCommand("AbortTimer %s", te->te_Address)) {
                Delay(25);
                DoMethod(obj, MUIM_TimersWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct TimersWinData *twd = INST_DATA(cl, obj);
    struct TimerEntry *te;

    if ((te = (struct TimerEntry *)GetActiveEntry(twd->twd_TimerList)) != NULL) {
        MySetContents(twd->twd_TimerText, "%s \"%s\"", te->te_Address, te->te_Name);
        set(twd->twd_AbortButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(TimersWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                    return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                return (mDispose(cl, obj, (APTR)msg));
        case MUIM_TimersWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_TimersWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_TimersWin_Abort:      return (mAbort(cl, obj, (APTR)msg));
        case MUIM_TimersWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintTimer( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtTimerPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendTimerList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeTimersWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct TimersWinData), ENTRY(TimersWinDispatcher));
}

