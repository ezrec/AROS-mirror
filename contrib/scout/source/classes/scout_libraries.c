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

struct LibrariesWinData {
    TEXT lwd_Title[WINDOW_TITLE_LENGTH];
    APTR lwd_LibraryList;
    APTR lwd_LibraryText;
    APTR lwd_LibraryCount;
    APTR lwd_RemoveButton;
    APTR lwd_PriorityButton;
    APTR lwd_CloseButton;
    APTR lwd_MoreButton;
    APTR lwd_FunctionsButton;
};

struct LibrariesCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(liblist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct LibraryEntry));
}
MakeStaticHook(liblist_con2hook, liblist_con2func);

HOOKPROTONHNO(liblist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(liblist_des2hook, liblist_des2func);

HOOKPROTONHNO(liblist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct LibraryEntry *le = (struct LibraryEntry *)msg->entry;

    if (le) {
        msg->strings[0] = le->le_Address;
        msg->strings[1] = le->le_Name;
        msg->strings[2] = le->le_Type;
        msg->strings[3] = le->le_Pri;
        msg->strings[4] = le->le_Version;
        msg->strings[5] = le->le_OpenCount;
        msg->strings[6] = le->le_RAMPtrCount;
        msg->strings[7] = le->le_CodeType;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtLibraryVersion;
        msg->strings[5] = txtLibraryOpenCount;
        msg->strings[6] = txtLibraryRPC;
        msg->strings[7] = txtLibraryCodeType;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
        msg->preparses[7] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(liblist_dsp2hook, liblist_dsp2func);

STATIC LONG liblist_cmp2colfunc( struct LibraryEntry *le1,
                                 struct LibraryEntry *le2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(le1->le_Address, le2->le_Address);
        case 1: return stricmp(le1->le_Name, le2->le_Name);
        case 2: return stricmp(le1->le_Type, le2->le_Type);
        case 3: return PriorityCompare(le1->le_Pri, le2->le_Pri);
        case 4: return VersionCompare(le1->le_Version, le2->le_Version);
        case 5: return IntegerCompare(le1->le_OpenCount, le2->le_OpenCount);
        case 6: return stricmp(le1->le_RAMPtrCount, le2->le_RAMPtrCount);
        case 7: return stricmp(le1->le_CodeType, le2->le_CodeType);
    }
}

STATIC LONG liblist_cmpfunc( const struct Node *n1,
                             const struct Node *n2 )
{
    return stricmp(((struct LibraryEntry *)n1)->le_Name, ((struct LibraryEntry *)n2)->le_Name);
}

HOOKPROTONHNO(liblist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct LibraryEntry *le1, *le2;
    ULONG col1, col2;

    le1 = (struct LibraryEntry *)msg->entry1;
    le2 = (struct LibraryEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = liblist_cmp2colfunc(le2, le1, col1);
    } else {
        cmp = liblist_cmp2colfunc(le1, le2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = liblist_cmp2colfunc(le2, le1, col2);
    } else {
        cmp = liblist_cmp2colfunc(le1, le2, col2);
    }

    return cmp;
}
MakeStaticHook(liblist_cmp2hook, liblist_cmp2func);

void FlushLibraries( void )
{
    struct Library *lib;

    Forbid();

    lib = (struct Library *)SysBase->LibList.lh_Head;
    while (lib) {
        struct Library *_lib;

        _lib = (struct Library *)lib->lib_Node.ln_Succ;

        if (lib->lib_OpenCnt == 0) {
            RemLibrary(lib);
        }

        lib = _lib;
    }

    Permit();
}

STATIC void ReceiveList( void (* callback)( struct LibraryEntry *le, void *userData ),
                         void *userData )
{
    struct LibraryEntry *le;

    if ((le = tbAllocVecPooled(globalPool, sizeof(struct LibraryEntry))) != NULL) {
        if (SendDaemon("GetLibList")) {
            while (ReceiveDecodedEntry(le, sizeof(struct LibraryEntry))) {
                callback(le, userData);
            }
        }

        tbFreeVecPooled(globalPool, le);
    }
}

STATIC void IterateList( void (* callback)( struct LibraryEntry *le, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct Library *lib;
    struct MinList tmplist;
    struct LibraryEntry *le;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->LibList, struct Library *, lib) {
        if ((le = AllocVec(sizeof(struct LibraryEntry), MEMF_CLEAR)) != NULL) {
            LONG cnt, total;

            le->le_Addr = lib;

            _snprintf(le->le_Address, sizeof(le->le_Address), ADDRESS_FORMAT, lib);
            stccpy(le->le_Name, lib->lib_Node.ln_Name, sizeof(le->le_Name));
            _snprintf(le->le_Pri, sizeof(le->le_Pri), "%4ld", lib->lib_Node.ln_Pri);
            _snprintf(le->le_Version, sizeof(le->le_Version), "%ld.%ld", lib->lib_Version, lib->lib_Revision);
            _snprintf(le->le_OpenCount, sizeof(le->le_OpenCount), "%6lD", lib->lib_OpenCnt);

            GetRamPointerCount(lib, &cnt, &total);
            _snprintf(le->le_RAMPtrCount, sizeof(le->le_RAMPtrCount), "%4lD", cnt);

            stccpy(le->le_Type, GetNodeType(lib->lib_Node.ln_Type), sizeof(le->le_Type));

        #if defined(__AROS__)
            stccpy(le->le_CodeType, txtLibraryCodeTypeNative, sizeof(le->le_CodeType));
        #else
            #if defined(__amigaos4__)
                if (lib->lib_ABIVersion == LIBABI_MIFACE) {
            #elif defined(__MORPHOS__)
                if (*(UBYTE *)(((struct JumpEntry *)lib) - 1)->je_JumpAddress == 0xff) {
            #else
                // OS3 is 68k only
                if (FALSE) {
            #endif
                    stccpy(le->le_CodeType, txtLibraryCodeTypePPC, sizeof(le->le_CodeType));
                } else {
                    stccpy(le->le_CodeType, txtLibraryCodeType68k, sizeof(le->le_CodeType));
                }
        #endif

            AddTail((struct List *)&tmplist, (struct Node *)le);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, liblist_cmpfunc);

    ITERATE_LIST(&tmplist, struct LibraryEntry *, le) {
        callback(le, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct LibraryEntry *le,
                            void *userData )
{
    struct LibrariesCallbackUserData *ud = (struct LibrariesCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, le);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct LibraryEntry *le,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtLibrariesPrintLine, le->le_Address, le->le_Pri, le->le_OpenCount, le->le_RAMPtrCount, le->le_Name, le->le_Version, le->le_CodeType);
}

STATIC void SendCallback( struct LibraryEntry *le,
                          UNUSED void *userData )
{
    SendEncodedEntry(le, sizeof(struct LibraryEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR liblist, libtext, libcount, updateButton, printButton, removeButton, priorityButton, closeButton, moreButton, funcButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Libraries",
        MUIA_Window_ID, MakeID('L','I','B','S'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&liblist, MakeID('L','I','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C, &liblist_con2hook, &liblist_des2hook, &liblist_dsp2hook, &liblist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&libtext, &libcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(closeButton    = MakeButton(txtClose)),
                Child, (IPTR)(funcButton     = MakeButton(txtFunctions)),
            End,

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct LibrariesWinData *lwd = INST_DATA(cl, obj);
        APTR parent;

        lwd->lwd_LibraryList = liblist;
        lwd->lwd_LibraryText = libtext;
        lwd->lwd_LibraryCount = libcount;
        lwd->lwd_RemoveButton = removeButton;
        lwd->lwd_PriorityButton = priorityButton;
        lwd->lwd_CloseButton = closeButton;
        lwd->lwd_MoreButton = moreButton;
        lwd->lwd_FunctionsButton = funcButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtLibrariesTitle, lwd->lwd_Title, sizeof(lwd->lwd_Title)));
        set(obj, MUIA_Window_DefaultObject, liblist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, lwd->lwd_RemoveButton,
                                                          lwd->lwd_PriorityButton,
                                                          lwd->lwd_CloseButton,
                                                          lwd->lwd_MoreButton,
                                                          lwd->lwd_FunctionsButton,
                                                          NULL);

        DoMethod(parent,          MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,             MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(liblist,         MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_LibrariesWin_ListChange);
        DoMethod(liblist,         MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_LibrariesWin_More);
        DoMethod(updateButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Update);
        DoMethod(printButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Print);
        DoMethod(removeButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Remove);
        DoMethod(moreButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_More);
        DoMethod(closeButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Close);
        DoMethod(priorityButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Priority);
        DoMethod(funcButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_LibrariesWin_Functions);
        DoMethod(exitButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(liblist,         MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(lwd->lwd_LibraryList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibrariesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(lwd->lwd_LibraryList, MUIA_NList_Quiet, TRUE);
    DoMethod(lwd->lwd_LibraryList, MUIM_NList_Clear);

    ud.ud_List = lwd->lwd_LibraryList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(lwd->lwd_LibraryCount, ud.ud_Count);
    MySetContents(lwd->lwd_LibraryText, "");

    set(lwd->lwd_LibraryList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, lwd->lwd_RemoveButton,
                                                      lwd->lwd_PriorityButton,
                                                      lwd->lwd_CloseButton,
                                                      lwd->lwd_MoreButton,
                                                      lwd->lwd_FunctionsButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintLibraries(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibraryEntry *le;

    if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveLibrary, le->le_Name)) {
            if (MyDoCommand("RemoveLibrary \"%s\"", le->le_Name) == RETURN_OK) {
                DoMethod(obj, MUIM_LibrariesWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibraryEntry *le;

    if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
        LONG pri;

        pri = atol(le->le_Pri);
        if (GetPriority(le->le_Name, &pri)) {
            if (MyDoCommand("SetPriority LIBRARY \"%s\" %ld", le->le_Name, pri) == RETURN_OK) {
                _snprintf(le->le_Pri, sizeof(le->le_Pri), "%4ld", pri);
                RedrawActiveEntry(lwd->lwd_LibraryList);
            }
        }
    }

    return 0;
}

STATIC IPTR mClose( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibraryEntry *le;

    if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
        SIPTR cnt;

        if (IsDec(le->le_OpenCount, &cnt) && cnt > 0) {
            ULONG answer;

            if ((answer = MyRequest(msgOnceAllCancel, msgWantToCloseLibrary, le->le_Name)) != 0) {
                if (answer == 1) {
                    if (MyDoCommand("CloseLibrary %s", le->le_Address) == RETURN_OK) {
                        cnt--;
                    }
                } else if (answer == 2) {
                    while (cnt > 0) {
                        if (MyDoCommand("CloseLibrary %s", le->le_Address) != RETURN_OK) break;
                        cnt--;
                    }
                }

                _snprintf(le->le_OpenCount, sizeof(le->le_OpenCount), "%6lD", cnt);
                RedrawActiveEntry(lwd->lwd_LibraryList);
            }
        } else {
            MyRequest(msgErrorContinue, msgLibOpenCountIsZero);
        }
    }

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct LibrariesWinData *lwd = INST_DATA(cl, obj);
        struct LibraryEntry *le;

        if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)LibrariesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_LibrariesDetailWin_Library, le);
                set(detailWin, MUIA_Window_Open, TRUE);
                REISSUE_RETURNIDS;
            }
        }
    }

    return 0;
}

STATIC IPTR mFunctions( struct IClass *cl,
                         Object *obj,
                         UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibraryEntry *le;

    if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
        APTR funcWin;

        if ((funcWin = (Object *)FunctionsWindowObject,
                MUIA_Window_ParentWindow, (IPTR)obj,
            End) != NULL) {
            DoMethod(funcWin, MUIM_FunctionsWin_ShowFunctions, MUIV_FunctionsWin_NodeType_Library, le->le_Addr, le->le_Name);
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct LibrariesWinData *lwd = INST_DATA(cl, obj);
    struct LibraryEntry *le;

    if ((le = (struct LibraryEntry *)GetActiveEntry(lwd->lwd_LibraryList)) != NULL) {
        MySetContents(lwd->lwd_LibraryText, "%s \"%s\"", le->le_Address, le->le_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, lwd->lwd_RemoveButton,
                                                           lwd->lwd_PriorityButton,
                                                           lwd->lwd_CloseButton,
                                                           (clientstate) ? NULL : lwd->lwd_MoreButton,
                                                           (clientstate) ? NULL : lwd->lwd_FunctionsButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(LibrariesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Close:      return (mClose(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_Functions:  return (mFunctions(cl, obj, (APTR)msg));
        case MUIM_LibrariesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintLibraries( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtLibrariesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendLibList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeLibrariesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct LibrariesWinData), ENTRY(LibrariesWinDispatcher));
}

