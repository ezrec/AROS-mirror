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

struct LocksWinData {
    TEXT lwd_Title[WINDOW_TITLE_LENGTH];
    APTR lwd_LockList;
    APTR lwd_LockText;
    APTR lwd_StatusText;
    APTR lwd_RemoveButton;
    APTR lwd_PatternString;
    ULONG lwd_LocksShown;
    ULONG lwd_LocksHidden;
};

struct LocksCallbackUserData {
    APTR ud_List;
    APTR ud_Application;
    APTR ud_Status;
    CONST_STRPTR ud_Pattern;
    ULONG ud_Shown;
    ULONG ud_Hidden;
    BPTR ud_Handle;
};

HOOKPROTONHNO(locklist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct LockEntry));
}
MakeStaticHook(locklist_con2hook, locklist_con2func);

HOOKPROTONHNO(locklist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(locklist_des2hook, locklist_des2func);

HOOKPROTONHNO(locklist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct LockEntry *le = (struct LockEntry *)msg->entry;

    if (le) {
        msg->strings[0] = le->le_Address;
        msg->strings[1] = le->le_Access;
        msg->strings[2] = le->le_Path;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtLockAccess;
        msg->strings[2] = txtLockPath;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(locklist_dsp2hook, locklist_dsp2func);

STATIC LONG locklist_cmp2colfunc( struct LockEntry *le1,
                                  struct LockEntry *le2,
                                  ULONG column )
{
    LONG cmp;

    switch (column) {
        default:
        case 0: cmp = HexCompare(le1->le_Address, le2->le_Address); break;
        case 1: cmp = stricmp(le1->le_Access, le2->le_Access); break;
        case 2: cmp = stricmp(le1->le_Path, le2->le_Path); if (cmp == 0) cmp = HexCompare(le1->le_Address, le2->le_Address); break;
    }

    return cmp;
}

HOOKPROTONHNO(locklist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct LockEntry *le1, *le2;
    ULONG col1, col2;

    le1 = (struct LockEntry *)msg->entry1;
    le2 = (struct LockEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = locklist_cmp2colfunc(le2, le1, col1);
    } else {
        cmp = locklist_cmp2colfunc(le1, le2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = locklist_cmp2colfunc(le2, le1, col2);
    } else {
        cmp = locklist_cmp2colfunc(le1, le2, col2);
    }

    return cmp;
}
MakeStaticHook(locklist_cmp2hook, locklist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct LockEntry *le, void *userData ),
                         void *userData )
{
    struct LockEntry *le;

    if ((le = tbAllocVecPooled(globalPool, sizeof(struct LockEntry))) != NULL) {
        if (SendDaemon("GetLockList")) {
            while (ReceiveDecodedEntry(le, sizeof(struct LockEntry))) {
                callback(le, userData);
            }
        }

        tbFreeVecPooled(globalPool, le);
    }
}

STATIC void IterateList( void (* callback)( struct LockEntry *le, void *userData ),
                         void *userData )
{
    STRPTR ppattern, tmp;
    struct LockEntry *le;

    ppattern = tbAllocVecPooled(globalPool, PATTERN_LENGTH);
    tmp = tbAllocVecPooled(globalPool, PATH_LENGTH);
    le = tbAllocVecPooled(globalPool, sizeof(struct LockEntry));

    if (ppattern != NULL && tmp != NULL && le != NULL) {
        struct LocksCallbackUserData *ud = (struct LocksCallbackUserData *)userData;
        BOOL patternOk;

        if (ud->ud_Pattern == NULL || (ud->ud_Pattern != NULL && ud->ud_Pattern[0] == 0x00)) {
            patternOk = (ParsePatternNoCase("#?", ppattern, PATTERN_LENGTH) != -1);
        } else {
            patternOk = (ParsePatternNoCase(ud->ud_Pattern, ppattern, PATTERN_LENGTH) != -1);
        }

        if (patternOk) {
            struct FileInfoBlock *fib;

            if ((fib = AllocDosObject(DOS_FIB, TAG_DONE)) != NULL) {
                struct DosList *dol;

                NoReqOn();

                dol = LockDosList(LDF_READ | LDF_VOLUMES);

                while ((dol = NextDosEntry(dol, LDF_VOLUMES)) != NULL) {
                    BPTR lock;

                    _snprintf(tmp, PATH_LENGTH, "%b:", dol->dol_Name);

                    if ((lock = Lock(tmp, SHARED_LOCK)) != ZERO) {
                        BOOL erster;
                        BPTR next;

                        erster = TRUE;
                        next = (BPTR)NULL;

                        while ((next != (BPTR)NULL && next != lock) || erster) {
                            struct FileLock *flock;

                            if (erster) next = lock;
                            flock = (struct FileLock *)BADDR(next);

                            if (TypeOfMem(flock) == 0) break;
                            if (flock->fl_Volume != MKBADDR(dol)) break;

                            if (Examine(next, fib)) {
                                if (erster) {
                                    stccpy(le->le_Access, txtLockOwn, sizeof(le->le_Access));
                                } else if (flock->fl_Access == SHARED_LOCK) {
                                    stccpy(le->le_Access, txtLockShared, sizeof(le->le_Access));
                                } else if (flock->fl_Access == EXCLUSIVE_LOCK) {
                                    stccpy(le->le_Access, txtLockExclusive, sizeof(le->le_Access));
                                } else {
                                    _snprintf(le->le_Access, sizeof(le->le_Access), txtLockSharedInvalid, flock->fl_Access);
                                }

                                if (flock->fl_Access == EXCLUSIVE_LOCK) {
                                    LONG oldAccess;

                                    Forbid();

                                    oldAccess = flock->fl_Access;
                                    flock->fl_Access = SHARED_LOCK;

                                    NameFromLock(next, le->le_Path, sizeof(le->le_Path));

                                    flock->fl_Access = oldAccess;

                                    Permit();
                                } else {
                                    NameFromLock(next, le->le_Path, sizeof(le->le_Path));
                                }

                                if (MatchPatternNoCase(ppattern, le->le_Path)) {
                                    le->le_Addr = next;
                                    _snprintf(le->le_Address, sizeof(le->le_Access), ADDRESS_FORMAT, flock);
                                    ud->ud_Shown++;
                                } else {
                                    le->le_Addr = (BPTR)NULL;
                                    ud->ud_Hidden++;
                                }

                                callback(le, userData);
                            }

                            erster = FALSE;
                            next = flock->fl_Link;
                        }

                        UnLock(lock);
                    }
                }

                UnLockDosList(LDF_READ | LDF_VOLUMES);

                NoReqOff();

                FreeDosObject(DOS_FIB, fib);
            }
        }
    }

    if (ppattern) tbFreeVecPooled(globalPool, ppattern);
    if (tmp) tbFreeVecPooled(globalPool, tmp);
    if (le) tbFreeVecPooled(globalPool, le);
}

STATIC void UpdateCallback( struct LockEntry *le,
                            void *userData )
{
    struct LocksCallbackUserData *ud = (struct LocksCallbackUserData *)userData;

    if (le && le->le_Addr) InsertSortedEntry(ud->ud_List, le);

    if ((ud->ud_Shown + ud->ud_Hidden) % 32 == 0) {
        MySetContents(ud->ud_Status, msgLocksShownHiddenInProgress, ud->ud_Shown, ud->ud_Hidden);
        DoMethod(ud->ud_Application, MUIM_Application_InputBuffered);
    }
}

STATIC void PrintCallback( struct LockEntry *le,
                           void *userData )
{
    struct LocksCallbackUserData *ud = (struct LocksCallbackUserData *)userData;

    if (le->le_Addr) PrintFOneLine(ud->ud_Handle, txtLocksPrintLine, le->le_Address, le->le_Access, le->le_Path);
}

STATIC void SendCallback( struct LockEntry *le,
                          UNUSED void *userData )
{
    if (le->le_Addr) SendEncodedEntry(le, sizeof(struct LockEntry));
}

STATIC void CountCallback( UNUSED struct LockEntry *le,
                           UNUSED void *userData )
{
    // no op, locks are already counted in IterateList()
}

STATIC void RemoveCallback( struct LockEntry *le,
                            UNUSED void *userData )
{
    if (le->le_Addr) UnLock(le->le_Addr);
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR locklist, locktext, locktext2, lockpattern, updateButton, printButton, removeButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Locks",
        MUIA_Window_ID, MakeID('L','O','C','K'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&locklist, MakeID('L','O','L','V'), "BAR,BAR,BAR", &locklist_con2hook, &locklist_des2hook, &locklist_dsp2hook, &locklist_cmp2hook, TRUE),

            Child, (IPTR)(locktext = MyTextObject5(SPACE40)),

            Child, (IPTR)(locktext2 = MyTextObject()),

            Child, (IPTR)HGroup,
                Child, (IPTR)KeyLabel2(txtPattern, 'p'),
                Child, (IPTR)(lockpattern = (Object *)StringObject,
                    MUIA_String_Contents, (IPTR)"#?",
                    MUIA_ControlChar, 'p',
                    MUIA_String_MaxLen, TEXT_LENGTH,
                    MUIA_CycleChain, TRUE,
                    StringFrame,
                End),
            End,

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(removeButton = MakeButton(txtRemove)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct LocksWinData *lwd = INST_DATA(cl, obj);
        APTR parent;

        lwd->lwd_LockList = locklist;
        lwd->lwd_LockText = locktext;
        lwd->lwd_StatusText = locktext2;
        lwd->lwd_PatternString = lockpattern;
        lwd->lwd_RemoveButton = removeButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtLocksTitle, lwd->lwd_Title, sizeof(lwd->lwd_Title)));
        set(obj, MUIA_Window_DefaultObject, locklist);
        set(locklist, MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Default);
        set(removeButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(locklist,     MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_LocksWin_ListChange);
        DoMethod(lockpattern,  MUIM_Notify, MUIA_String_Acknowledge,  MUIV_EveryTime, obj,                     1, MUIM_LocksWin_Update);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LocksWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LocksWin_Print);
        DoMethod(removeButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LocksWin_Remove);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(locklist,     MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_2, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct LocksWinData *lwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(lwd->lwd_LockList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LocksWinData *lwd = INST_DATA(cl, obj);
    struct LocksCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(lwd->lwd_LockList, MUIA_NList_Quiet, TRUE);
    DoMethod(lwd->lwd_LockList, MUIM_NList_Clear);

    ud.ud_List = lwd->lwd_LockList;
    get(obj, MUIA_ApplicationObject, &ud.ud_Application);
    ud.ud_Status = lwd->lwd_StatusText;
    ud.ud_Shown = 0;
    ud.ud_Hidden = 0;
    get(lwd->lwd_PatternString, MUIA_String_Contents, &ud.ud_Pattern);

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    lwd->lwd_LocksShown = ud.ud_Shown;
    lwd->lwd_LocksHidden = ud.ud_Hidden;
    MySetContents(lwd->lwd_StatusText, msgLocksShownHiddenDone, ud.ud_Shown, ud.ud_Hidden);
    set(obj, MUIA_Window_ActiveObject, lwd->lwd_LockList);

    set(lwd->lwd_LockList, MUIA_NList_Quiet, FALSE);
    set(lwd->lwd_RemoveButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintLocks(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LocksWinData *lwd = INST_DATA(cl, obj);
    STRPTR tmp;

    if ((tmp = tbAllocVecPooled(globalPool, FILENAME_LENGTH)) != NULL) {
        ULONG id = MUIV_NList_NextSelected_Start;
        ULONG remMode = 1;

        while (TRUE) {
            struct LockEntry *le;

            DoMethod(lwd->lwd_LockList, MUIM_NList_NextSelected, &id);
            if ((LONG)id == MUIV_NList_NextSelected_End) break;
            if (remMode == 0) break;

            DoMethod(lwd->lwd_LockList, MUIM_NList_GetEntry, id, &le);
            if (le) {
                ULONG pos = 0;

                if (remMode != 2) remMode = MyRequest(msgYesAllNoAbort, msgWantToRemoveLock, le->le_Path);

                get(lwd->lwd_LockList, MUIA_NList_Active, &pos);
                if (pos == id) {
                    set(lwd->lwd_LockList, MUIA_NList_Active, MUIV_List_Active_Off);
                    set(lwd->lwd_RemoveButton, MUIA_Disabled, TRUE);
                }

                if (remMode == 1 || remMode == 2) {
                    if (MyDoCommand("RemoveLock %s", le->le_Address) == RETURN_OK) {
                        DoMethod(lwd->lwd_LockList, MUIM_NList_Remove, id);
                        id = MUIV_NList_NextSelected_Start;

                        lwd->lwd_LocksShown--;
                        _snprintf(tmp, FILENAME_LENGTH, msgLocksShownHiddenDone, lwd->lwd_LocksShown, lwd->lwd_LocksHidden);
                        set(lwd->lwd_StatusText, MUIA_Text_Contents, tmp);
                    }
                } else if (remMode == 3) {
                    DoMethod(lwd->lwd_LockList, MUIM_NList_Select, id, MUIV_NList_Select_Off, NULL);
                    id = MUIV_NList_NextSelected_Start;
                }
            }
        }

        DoMethod(obj, MUIM_LocksWin_Update);

        tbFreeVecPooled(globalPool, tmp);
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct LocksWinData *lwd = INST_DATA(cl, obj);
    struct LockEntry *le;

    if ((le = (struct LockEntry *)GetActiveEntry(lwd->lwd_LockList)) != NULL) {
        MySetContents(lwd->lwd_LockText, "%s \"%s\"", le->le_Address, le->le_Path);
        set(lwd->lwd_RemoveButton, MUIA_Disabled, FALSE);
    }

    return 0;
}

DISPATCHER(LocksWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                   return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:               return (mDispose(cl, obj, (APTR)msg));
        case MUIM_LocksWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_LocksWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_LocksWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_LocksWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintLocks( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        struct LocksCallbackUserData ud;

        ud.ud_Shown = 0;
        ud.ud_Hidden = 0;
        ud.ud_Pattern = "#?";
        ud.ud_Handle = handle;

        PrintFOneLine(handle, txtLocksPrintHeader);
        IterateList(PrintCallback, &ud);
    }

    HandlePrintStop();
}

void SendLockList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

ULONG CountLocks( CONST_STRPTR pattern )
{
    struct LocksCallbackUserData ud;

    ud.ud_Pattern = pattern;
    ud.ud_Shown = 0;
    ud.ud_Hidden = 0;

    IterateList(CountCallback, &ud);

    return ud.ud_Shown;
}

void RemoveLock( CONST_STRPTR addr )
{
    struct LocksCallbackUserData ud;

    ud.ud_Pattern = addr;
    ud.ud_Shown = 0;
    ud.ud_Hidden = 0;

    IterateList(RemoveCallback, &ud);
}

APTR MakeLocksWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct LocksWinData), ENTRY(LocksWinDispatcher));
}

