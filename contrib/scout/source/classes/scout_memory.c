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

struct MemoryWinData {
    TEXT mwd_Title[WINDOW_TITLE_LENGTH];
    APTR mwd_MemoryList;
    APTR mwd_MemoryText;
    APTR mwd_PriorityButton;
    APTR mwd_MoreButton;
};

struct MemoryCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(memlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct MemoryEntry));
}
MakeStaticHook(memlist_con2hook, memlist_con2func);

HOOKPROTONHNO(memlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(memlist_des2hook, memlist_des2func);

HOOKPROTONHNO(memlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct MemoryEntry *me = (struct MemoryEntry *)msg->entry;

    if (me) {
        msg->strings[0] = me->me_Address;
        msg->strings[1] = me->me_Name;
        msg->strings[2] = me->me_Type;
        msg->strings[3] = me->me_Pri;
        msg->strings[4] = me->me_Lower;
        msg->strings[5] = me->me_Upper;
        msg->strings[6] = me->me_Attributes;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtMemoryLower;
        msg->strings[5] = txtMemoryUpper;
        msg->strings[6] = txtMemoryAttr;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(memlist_dsp2hook, memlist_dsp2func);

STATIC SIPTR memlist_cmp2colfunc( struct MemoryEntry *me1,
                                 struct MemoryEntry *me2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(me1->me_Address, me2->me_Address);
        case 1: return stricmp(me1->me_Name, me2->me_Name);
        case 2: return stricmp(me1->me_Type, me2->me_Type);
        case 3: return PriorityCompare(me1->me_Pri, me2->me_Pri);
        case 4: return HexCompare(me1->me_Lower, me2->me_Lower);
        case 5: return HexCompare(me1->me_Upper, me2->me_Upper);
        case 6: return HexCompare(me1->me_Attributes, me2->me_Attributes);
    }
}

HOOKPROTONHNO(memlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    SIPTR cmp;
    struct MemoryEntry *me1, *me2;
    ULONG col1, col2;

    me1 = (struct MemoryEntry *)msg->entry1;
    me2 = (struct MemoryEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = memlist_cmp2colfunc(me2, me1, col1);
    } else {
        cmp = memlist_cmp2colfunc(me1, me2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = memlist_cmp2colfunc(me2, me1, col2);
    } else {
        cmp = memlist_cmp2colfunc(me1, me2, col2);
    }

    return cmp;
}
MakeStaticHook(memlist_cmp2hook, memlist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct MemoryEntry *me, void *userData ),
                         void *userData )
{
    struct MemoryEntry *me;

    if ((me = tbAllocVecPooled(globalPool, sizeof(struct MemoryEntry))) != NULL) {
        if (SendDaemon("GetMemList")) {
            while (ReceiveDecodedEntry(me, sizeof(struct MemoryEntry))) {
                callback(me, userData);
            }
        }

        tbFreeVecPooled(globalPool, me);
    }
}

STATIC void IterateList( void (* callback)( struct MemoryEntry *me, void *userData ),
                         void *userData )
{
    struct MinList tmplist;
    struct MemoryEntry *me;
    struct MemHeader *mh;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->MemList, struct MemHeader *, mh) {
        if ((me = AllocVec(sizeof(struct MemoryEntry), MEMF_CLEAR)) != NULL) {
            me->me_Header = mh;
            _snprintf(me->me_Address, sizeof(me->me_Address), "$%p", mh);
            stccpy(me->me_Name, nonetest(mh->mh_Node.ln_Name), sizeof(me->me_Name));
            stccpy(me->me_Type, GetNodeType(mh->mh_Node.ln_Type), sizeof(me->me_Type));
            _snprintf(me->me_Pri, sizeof(me->me_Pri), "%4ld", mh->mh_Node.ln_Pri);
            _snprintf(me->me_Lower, sizeof(me->me_Lower), "$%p", mh->mh_Lower);
            _snprintf(me->me_Upper, sizeof(me->me_Upper), "$%p", mh->mh_Upper);
            _snprintf(me->me_Attributes, sizeof(me->me_Attributes), "$%04lx", mh->mh_Attributes);

            AddTail((struct List *)&tmplist, (struct Node *)me);
        }
    }

    Permit();

    ITERATE_LIST(&tmplist, struct MemoryEntry *, me) {
        callback(me, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct MemoryEntry *me,
                            void *userData )
{
    struct MemoryCallbackUserData *ud = (struct MemoryCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, me);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct MemoryEntry *me,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtMemoryPrintLine, me->me_Address, me->me_Name, me->me_Type, me->me_Pri, me->me_Lower, me->me_Upper, me->me_Attributes);
}

STATIC void SendCallback( struct MemoryEntry *me,
                          UNUSED void *userData )
{
    SendEncodedEntry(me, sizeof(struct MemoryEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR memlist, memtext, updateButton, printButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Memory",
        MUIA_Window_ID, MakeID('M','E','M','O'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&memlist, MakeID('M','E','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR,BAR,BAR", &memlist_con2hook, &memlist_des2hook, &memlist_dsp2hook, &memlist_cmp2hook, TRUE),
            Child, (IPTR)(memtext = MyTextObject()),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MemoryWinData *mwd = INST_DATA(cl, obj);
        APTR parent;

        mwd->mwd_MemoryList = memlist;
        mwd->mwd_MemoryText = memtext;
        mwd->mwd_PriorityButton = priorityButton;
        mwd->mwd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtMemoryTitle, mwd->mwd_Title, sizeof(mwd->mwd_Title)));
        set(obj, MUIA_Window_DefaultObject, memlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, mwd->mwd_PriorityButton,
                                                          mwd->mwd_MoreButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(memlist,        MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_MemoryWin_ListChange);
        DoMethod(memlist,        MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_MemoryWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MemoryWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MemoryWin_Print);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MemoryWin_Priority);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_MemoryWin_More);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct MemoryWinData *mwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(mwd->mwd_MemoryList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct MemoryWinData *mwd = INST_DATA(cl, obj);
    struct MemoryCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(mwd->mwd_MemoryList, MUIA_NList_Quiet, TRUE);
    DoMethod(mwd->mwd_MemoryList, MUIM_NList_Clear);

    ud.ud_List = mwd->mwd_MemoryList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    MySetContents(mwd->mwd_MemoryText, "");

    set(mwd->mwd_MemoryList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, mwd->mwd_PriorityButton,
                                                      mwd->mwd_MoreButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintMemory(NULL);

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct MemoryWinData *mwd = INST_DATA(cl, obj);
    struct MemoryEntry *me;

    if ((me = (struct MemoryEntry *)GetActiveEntry(mwd->mwd_MemoryList)) != NULL) {
        LONG pri;

        pri = atol(me->me_Pri);
        if (GetPriority(me->me_Name, &pri)) {
            if (MyDoCommand("SetPriority MEMORY \"%s\" %ld", me->me_Name, pri) == RETURN_OK) {
                _snprintf(me->me_Pri, sizeof(me->me_Pri), "%4ld", pri);
                RedrawActiveEntry(mwd->mwd_MemoryList);
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
        struct MemoryWinData *mwd = INST_DATA(cl, obj);
        struct MemoryEntry *me;

        if ((me = (struct MemoryEntry *)GetActiveEntry(mwd->mwd_MemoryList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)MemoryDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_MemoryDetailWin_Memory, me);
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
    struct MemoryWinData *mwd = INST_DATA(cl, obj);
    struct MemoryEntry *me;

    if ((me = (struct MemoryEntry *)GetActiveEntry(mwd->mwd_MemoryList)) != NULL) {
        MySetContents(mwd->mwd_MemoryText, "%s \"%s\"", me->me_Address, me->me_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, mwd->mwd_PriorityButton,
                                                           (clientstate) ? NULL : mwd->mwd_MoreButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(MemoryWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                    return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                return (mDispose(cl, obj, (APTR)msg));
        case MUIM_MemoryWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_MemoryWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_MemoryWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_MemoryWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_MemoryWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintMemory( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtMemoryPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendMemList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeMemoryWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MemoryWinData), ENTRY(MemoryWinDispatcher));
}
