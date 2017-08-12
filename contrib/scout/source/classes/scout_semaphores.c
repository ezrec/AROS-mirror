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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOse->  See the
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

struct SemaphoresWinData {
    TEXT swd_Title[WINDOW_TITLE_LENGTH];
    APTR swd_SemaphoreList;
    APTR swd_SemaphoreText;
    APTR swd_SemaphoreCount;
    APTR swd_ObtainButton;
    APTR swd_ReleaseButton;
    APTR swd_RemoveButton;
    APTR swd_PriorityButton;
};

struct SemaphoresCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(semalist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct SemaphoreEntry));
}
MakeStaticHook(semalist_con2hook, semalist_con2func);

HOOKPROTONHNO(semalist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(semalist_des2hook, semalist_des2func);

HOOKPROTONHNO(semalist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct SemaphoreEntry *se = (struct SemaphoreEntry *)msg->entry;

    if (se) {
        msg->strings[0] = se->se_Address;
        msg->strings[1] = se->se_Name;
        msg->strings[2] = se->se_Pri;
        msg->strings[3] = se->se_NestCount;
        msg->strings[4] = se->se_QueueCount;
        msg->strings[5] = se->se_Owner;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodePri;
        msg->strings[3] = txtSemaphoreNestCount;
        msg->strings[4] = txtSemaphoreQueueCount;
        msg->strings[5] = txtSemaphoreOwner;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(semalist_dsp2hook, semalist_dsp2func);

STATIC LONG semalist_cmp2colfunc( struct SemaphoreEntry *se1,
                                  struct SemaphoreEntry *se2,
                                  ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(se1->se_Address, se2->se_Address);
        case 1: return stricmp(se1->se_Name, se2->se_Name);
        case 2: return PriorityCompare(se1->se_Pri, se2->se_Pri);
        case 3: return HexCompare(se1->se_NestCount, se2->se_NestCount);
        case 4: return HexCompare(se1->se_QueueCount, se2->se_QueueCount);
        case 5: return stricmp(se1->se_Owner, se2->se_Owner);
    }
}

STATIC LONG semalist_cmpfunc( const struct Node *n1,
                              const struct Node *n2 )
{
    return stricmp(((struct SemaphoreEntry *)n1)->se_Name, ((struct SemaphoreEntry *)n2)->se_Name);
}

HOOKPROTONHNO(semalist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct SemaphoreEntry *se1, *se2;
    ULONG col1, col2;

    se1 = (struct SemaphoreEntry *)msg->entry1;
    se2 = (struct SemaphoreEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = semalist_cmp2colfunc(se2, se1, col1);
    } else {
        cmp = semalist_cmp2colfunc(se1, se2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = semalist_cmp2colfunc(se2, se1, col2);
    } else {
        cmp = semalist_cmp2colfunc(se1, se2, col2);
    }

    return cmp;
}
MakeStaticHook(semalist_cmp2hook, semalist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct SemaphoreEntry *se, void *userData ),
                         void *userData )
{
    struct SemaphoreEntry *se;

    if ((se = tbAllocVecPooled(globalPool, sizeof(struct SemaphoreEntry))) != NULL) {
        if (SendDaemon("GetSemList")) {
            while (ReceiveDecodedEntry(se, sizeof(struct SemaphoreEntry))) {
                callback(se, userData);
            }
        }

        tbFreeVecPooled(globalPool, se);
    }
}

STATIC void IterateList( void (* callback)( struct SemaphoreEntry *se, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct SignalSemaphore *ss;
    struct MinList tmplist;
    struct SemaphoreEntry *se;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->SemaphoreList, struct SignalSemaphore *, ss) {
        if ((se = AllocVec(sizeof(struct SemaphoreEntry), MEMF_CLEAR)) != NULL) {
            se->se_Addr = ss;

            _snprintf(se->se_Address, sizeof(se->se_Address), ADDRESS_FORMAT, ss);
            stccpy(se->se_Name, nonetest (ss->ss_Link.ln_Name), sizeof(se->se_Name));
            _snprintf(se->se_Pri, sizeof(se->se_Pri), "%4ld", ss->ss_Link.ln_Pri);
            _snprintf(se->se_NestCount, sizeof(se->se_NestCount), "%4ld", ss->ss_NestCount);
            _snprintf(se->se_QueueCount, sizeof(se->se_QueueCount), "%4ld", ss->ss_QueueCount);
            if (ss->ss_Owner) {
                GetTaskName(ss->ss_Owner, se->se_Owner, sizeof(se->se_Owner));
            } else {
                stccpy(se->se_Owner, "---", sizeof(se->se_Owner));
            }

            AddTail((struct List *)&tmplist, (struct Node *)se);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, semalist_cmpfunc);

    ITERATE_LIST(&tmplist, struct SemaphoreEntry *, se) {
        callback(se, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct SemaphoreEntry *se,
                            void *userData )
{
    struct SemaphoresCallbackUserData *ud = (struct SemaphoresCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, se);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct SemaphoreEntry *se,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtSemaphorePrintLine, se->se_Address, se->se_Name, se->se_Pri, se->se_NestCount, se->se_QueueCount, se->se_Owner);
}

STATIC void SendCallback( struct SemaphoreEntry *se,
                          UNUSED void *userData )
{
    SendEncodedEntry(se, sizeof(struct SemaphoreEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR semlist, semtext, semcount, updateButton, printButton, obtainButton, releaseButton, removeButton, priorityButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Semaphores",
        MUIA_Window_ID, MakeID('S','E','M','A'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&semlist, MakeID('S','E','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR", &semalist_con2hook, &semalist_des2hook, &semalist_dsp2hook, &semalist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&semtext, &semcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(obtainButton   = MakeButton(txtObtain)),
                Child, (IPTR)(releaseButton  = MakeButton(txtRelease)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct SemaphoresWinData *swd = INST_DATA(cl, obj);
        APTR parent;

        swd->swd_SemaphoreList = semlist;
        swd->swd_SemaphoreText = semtext;
        swd->swd_SemaphoreCount = semcount;
        swd->swd_ObtainButton = obtainButton;
        swd->swd_ReleaseButton = releaseButton;
        swd->swd_RemoveButton = removeButton;
        swd->swd_PriorityButton = priorityButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtSemaphoresTitle, swd->swd_Title, sizeof(swd->swd_Title)));
        set(obj, MUIA_Window_DefaultObject, semlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, swd->swd_ObtainButton,
                                                          swd->swd_ReleaseButton,
                                                          swd->swd_RemoveButton,
                                                          swd->swd_PriorityButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(semlist,        MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_SemaphoresWin_ListChange);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Print);
        DoMethod(obtainButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Obtain);
        DoMethod(releaseButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Release);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Remove);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_SemaphoresWin_Priority);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(semlist,        MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(swd->swd_SemaphoreList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoresCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(swd->swd_SemaphoreList, MUIA_NList_Quiet, TRUE);
    DoMethod(swd->swd_SemaphoreList, MUIM_NList_Clear);

    ud.ud_List = swd->swd_SemaphoreList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(swd->swd_SemaphoreCount, ud.ud_Count);
    MySetContents(swd->swd_SemaphoreText, "");

    set(swd->swd_SemaphoreList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, swd->swd_ObtainButton,
                                                      swd->swd_ReleaseButton,
                                                      swd->swd_RemoveButton,
                                                      swd->swd_PriorityButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintSemaphores(NULL);

    return 0;
}

STATIC IPTR mObtain( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoreEntry *se;

    if ((se = (struct SemaphoreEntry *)GetActiveEntry(swd->swd_SemaphoreList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToObtainSemaphore, se->se_Name)) {
            if (MyDoCommand("ObtainSemaphore %s", se->se_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_SemaphoresWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mRelease( struct IClass *cl,
                       Object *obj,
                       UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoreEntry *se;

    if ((se = (struct SemaphoreEntry *)GetActiveEntry(swd->swd_SemaphoreList)) != NULL) {
        SIPTR nest;

        if (IsDec(se->se_NestCount, &nest) && nest > 0) {
            if (MyRequest(msgYesNo, msgWantToReleaseSemaphore, se->se_Name)) {
                if (MyDoCommand("ReleaseSemaphore %s", se->se_Address) == RETURN_OK) {
                    DoMethod(obj, MUIM_SemaphoresWin_Update);
                }
            }
        } else {
            MyRequest(msgErrorContinue, msgNestCountIsZero);
        }
    }

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoreEntry *se;

    if ((se = (struct SemaphoreEntry *)GetActiveEntry(swd->swd_SemaphoreList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveSemaphore, se->se_Name)) {
            if (MyDoCommand("RemoveSemaphore %s", se->se_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_SemaphoresWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoreEntry *se;

    if ((se = (struct SemaphoreEntry *)GetActiveEntry(swd->swd_SemaphoreList)) != NULL) {
        LONG pri;

        pri = atol(se->se_Pri);
        if (GetPriority(se->se_Name, &pri)) {
            if (MyDoCommand("SetPriority SEMAPHORE \"%s\" %ld", se->se_Name, pri) == RETURN_OK) {
                _snprintf(se->se_Pri, sizeof(se->se_Pri), "%4ld", pri);
                RedrawActiveEntry(swd->swd_SemaphoreList);
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct SemaphoresWinData *swd = INST_DATA(cl, obj);
    struct SemaphoreEntry *se;

    if ((se = (struct SemaphoreEntry *)GetActiveEntry(swd->swd_SemaphoreList)) != NULL) {
        MySetContents(swd->swd_SemaphoreText, "%s \"%s\"", se->se_Address, se->se_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, swd->swd_ObtainButton,
                                                           swd->swd_ReleaseButton,
                                                           swd->swd_RemoveButton,
                                                           swd->swd_PriorityButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(SemaphoresWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                        return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                    return (mDispose(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Obtain:     return (mObtain(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Release:    return (mRelease(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_SemaphoresWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintSemaphores( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtSemaphorePrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendSemList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeSemaphoresWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct SemaphoresWinData), ENTRY(SemaphoresWinDispatcher));
}

