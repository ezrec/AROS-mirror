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
 * @author Pavel Fedin <pavel_fedin@mail.ru>
 */

#include "system_headers.h"

struct ResourcesWinData {
    TEXT rwd_Title[WINDOW_TITLE_LENGTH];
    APTR rwd_ResourceList;
    APTR rwd_ResourceText;
    APTR rwd_ResourceCount;
    APTR rwd_RemoveButton;
    APTR rwd_PriorityButton;
    APTR rwd_MoreButton;
    APTR rwd_FunctionsButton;
};

struct ResourcesCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

CONST_STRPTR forbiddenResources[] = {
    FSRNAME,
    KEYMAPRESNAME,
#if defined(__amigaos4__)
    "emulator.resource",
    "performancemonitor.resource",
    "radeonrom.resource",
    "newmemory.resource",
    "newfilesystem.resource",
    "biosversion.resource",
#elif defined(__AROS__)
    "bootloader.resource",
    "bootmenu.resource",
    "dosboot.resource",
    "hostlib.resource",
    "kernel.resource",
#endif
    NULL,
};

HOOKPROTONHNO(reslist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct ResourceEntry));
}
MakeStaticHook(reslist_con2hook, reslist_con2func);

HOOKPROTONHNO(reslist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(reslist_des2hook, reslist_des2func);

HOOKPROTONHNO(reslist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct ResourceEntry *re = (struct ResourceEntry *)msg->entry;

    if (re) {
        msg->strings[0] = re->re_Address;
        msg->strings[1] = re->re_Name;
        msg->strings[2] = re->re_Type;
        msg->strings[3] = re->re_Pri;
        msg->strings[4] = re->re_Version;
        msg->strings[5] = re->re_OpenCount;
        msg->strings[6] = re->re_RAMPtrCount;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtResourceVersion;
        msg->strings[5] = txtResourceOpenCount;
        msg->strings[6] = txtResourceRPC;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(reslist_dsp2hook, reslist_dsp2func);

STATIC LONG reslist_cmp2colfunc( struct ResourceEntry *re1,
                                 struct ResourceEntry *re2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(re1->re_Address, re2->re_Address);
        case 1: return stricmp(re1->re_Name, re2->re_Name);
        case 2: return stricmp(re1->re_Type, re2->re_Type);
        case 3: return PriorityCompare(re1->re_Pri, re2->re_Pri);
        case 4: return VersionCompare(re1->re_Version, re2->re_Version);
        case 5: return IntegerCompare(re1->re_OpenCount, re2->re_OpenCount);
        case 6: return stricmp(re1->re_RAMPtrCount, re2->re_RAMPtrCount);
    }
}

STATIC LONG reslist_cmpfunc( const struct Node *n1,
                             const struct Node *n2 )
{
    return stricmp(((struct ResourceEntry *)n1)->re_Name, ((struct ResourceEntry *)n2)->re_Name);
}

HOOKPROTONHNO(reslist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct ResourceEntry *re1, *re2;
    ULONG col1, col2;

    re1 = (struct ResourceEntry *)msg->entry1;
    re2 = (struct ResourceEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = reslist_cmp2colfunc(re2, re1, col1);
    } else {
        cmp = reslist_cmp2colfunc(re1, re2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = reslist_cmp2colfunc(re2, re1, col2);
    } else {
        cmp = reslist_cmp2colfunc(re1, re2, col2);
    }

    return cmp;
}
MakeStaticHook(reslist_cmp2hook, reslist_cmp2func);

BOOL TestResource( struct Library *lib )
{
    ULONG i;

    // check for known resources that don't contain a valid resource structure
    i = 0;
    while (forbiddenResources[i] != NULL) {
        if (stricmp(lib->lib_Node.ln_Name, forbiddenResources[i]) == 0) {
            return FALSE;
        }
        i++;
    };

    // leave out all resources whose type is unknown
    if (lib->lib_Node.ln_Type != NT_RESOURCE && lib->lib_Node.ln_Type != NT_LIBRARY) {
        return FALSE;
    }

    return TRUE;
}

STATIC void ReceiveList( void (* callback)( struct ResourceEntry *re, void *userData ),
                         void *userData )
{
    struct ResourceEntry *re;

    if ((re = tbAllocVecPooled(globalPool, sizeof(struct ResourceEntry))) != NULL) {
        if (SendDaemon("GetResList")) {
            while (ReceiveDecodedEntry(re, sizeof(struct ResourceEntry))) {
                callback(re, userData);
            }
        }

        tbFreeVecPooled(globalPool, re);
    }
}

STATIC void IterateList( void (* callback)( struct ResourceEntry *re, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct Library *res;
    struct MinList tmplist;
    struct ResourceEntry *re;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->ResourceList, struct Library *, res) {
        if ((re = AllocVec(sizeof(struct ResourceEntry), MEMF_CLEAR)) != NULL) {
            re->re_Addr = res;

            _snprintf(re->re_Address, sizeof(re->re_Address), ADDRESS_FORMAT, res);
            stccpy(re->re_Name, res->lib_Node.ln_Name, sizeof(re->re_Name));
            _snprintf(re->re_Pri, sizeof(re->re_Pri), "%4ld", res->lib_Node.ln_Pri);

            if (TestResource(res)) {
                LONG cnt, total;

                _snprintf(re->re_Version, sizeof(re->re_Version), "%ld.%ld", res->lib_Version, res->lib_Revision);
                _snprintf(re->re_OpenCount, sizeof(re->re_OpenCount), "%6lD", res->lib_OpenCnt);

                GetRamPointerCount(res, &cnt, &total);
                _snprintf(re->re_RAMPtrCount, sizeof(re->re_RAMPtrCount), "%4lD", cnt);
            } else {
                stccpy(re->re_Version, "---", sizeof(re->re_Version));
                stccpy(re->re_OpenCount, "---", sizeof(re->re_OpenCount));
                stccpy(re->re_RAMPtrCount, "---", sizeof(re->re_RAMPtrCount));
            }

            stccpy(re->re_Type, GetNodeType(res->lib_Node.ln_Type), sizeof(re->re_Type));

            AddTail((struct List *)&tmplist, (struct Node *)re);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, reslist_cmpfunc);

    ITERATE_LIST(&tmplist, struct ResourceEntry *, re) {
        callback(re, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct ResourceEntry *re,
                            void *userData )
{
    struct ResourcesCallbackUserData *ud = (struct ResourcesCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, re);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ResourceEntry *re,
                           void *userData )
{
   PrintFOneLine((BPTR)userData, txtResourcesPrintLine, re->re_Address, re->re_Pri, re->re_OpenCount, re->re_RAMPtrCount, re->re_Name, re->re_Version);
}

STATIC void SendCallback( struct ResourceEntry *re,
                          UNUSED void *userData )
{
    SendEncodedEntry(re, sizeof(struct ResourceEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR reslist, restext, rescount, updateButton, printButton, removeButton, priorityButton, moreButton, funcButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Resources",
        MUIA_Window_ID, MakeID('R','E','S','O'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&reslist, MakeID('R','S','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_R "", &reslist_con2hook, &reslist_des2hook, &reslist_dsp2hook, &reslist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&restext, &rescount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
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
        struct ResourcesWinData *rwd = INST_DATA(cl, obj);
        APTR parent;

        rwd->rwd_ResourceList = reslist;
        rwd->rwd_ResourceText = restext;
        rwd->rwd_ResourceCount = rescount;
        rwd->rwd_RemoveButton = removeButton;
        rwd->rwd_PriorityButton = priorityButton;
        rwd->rwd_MoreButton = moreButton;
        rwd->rwd_FunctionsButton = funcButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtResourcesTitle, rwd->rwd_Title, sizeof(rwd->rwd_Title)));
        set(obj, MUIA_Window_DefaultObject, reslist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, rwd->rwd_RemoveButton,
                                                          rwd->rwd_PriorityButton,
                                                          rwd->rwd_MoreButton,
                                                          rwd->rwd_FunctionsButton,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(reslist,        MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_ResourcesWin_ListChange);
        DoMethod(reslist,        MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_ResourcesWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_Remove);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_More);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_Priority);
        DoMethod(funcButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ResourcesWin_Functions);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(reslist,        MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(rwd->rwd_ResourceList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);
    struct ResourcesCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(rwd->rwd_ResourceList, MUIA_NList_Quiet, TRUE);
    DoMethod(rwd->rwd_ResourceList, MUIM_NList_Clear);

    ud.ud_List = rwd->rwd_ResourceList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(rwd->rwd_ResourceCount, ud.ud_Count);
    MySetContents(rwd->rwd_ResourceText, "");

    set(rwd->rwd_ResourceList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, rwd->rwd_RemoveButton,
                                                      rwd->rwd_PriorityButton,
                                                      rwd->rwd_MoreButton,
                                                      rwd->rwd_FunctionsButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintResources(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);
    struct ResourceEntry *re;

    if ((re = (struct ResourceEntry *)GetActiveEntry(rwd->rwd_ResourceList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveResource, re->re_Name)) {
            if (MyDoCommand("RemoveResource \"%s\"", re->re_Name) == RETURN_OK) {
                DoMethod(obj, MUIM_ResourcesWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);
    struct ResourceEntry *re;

    if ((re = (struct ResourceEntry *)GetActiveEntry(rwd->rwd_ResourceList)) != NULL) {
        LONG pri;

        pri = atol(re->re_Pri);
        if (GetPriority(re->re_Name, &pri)) {
            if (MyDoCommand("SetPriority RESOURCE \"%s\" %ld", re->re_Name, pri) == RETURN_OK) {
                _snprintf(re->re_Pri, sizeof(re->re_Pri), "%4ld", pri);
                RedrawActiveEntry(rwd->rwd_ResourceList);
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
        struct ResourcesWinData *rwd = INST_DATA(cl, obj);
        struct ResourceEntry *re;

        if ((re = (struct ResourceEntry *)GetActiveEntry(rwd->rwd_ResourceList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)ResourcesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_ResourcesDetailWin_Resource, re);
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
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);
    struct ResourceEntry *re;

    if ((re = (struct ResourceEntry *)GetActiveEntry(rwd->rwd_ResourceList)) != NULL) {
        APTR funcWin;

        if ((funcWin = (Object *)FunctionsWindowObject,
                MUIA_Window_ParentWindow, (IPTR)obj,
            End) != NULL) {
            DoMethod(funcWin, MUIM_FunctionsWin_ShowFunctions, MUIV_FunctionsWin_NodeType_Resource, re->re_Addr, re->re_Name);
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct ResourcesWinData *rwd = INST_DATA(cl, obj);
    struct ResourceEntry *re;

    if ((re = (struct ResourceEntry *)GetActiveEntry(rwd->rwd_ResourceList)) != NULL) {
        MySetContents(rwd->rwd_ResourceText, "%s \"%s\"", re->re_Address, re->re_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, rwd->rwd_RemoveButton,
                                                           rwd->rwd_PriorityButton,
                                                           (clientstate) ? NULL : rwd->rwd_MoreButton,
                                                           NULL);
        set(rwd->rwd_FunctionsButton, MUIA_Disabled, clientstate || !TestResource(re->re_Addr));
    }

    return 0;
}

DISPATCHER(ResourcesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_Functions:  return (mFunctions(cl, obj, (APTR)msg));
        case MUIM_ResourcesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintResources( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtResourcesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendResList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeResourcesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ResourcesWinData), ENTRY(ResourcesWinDispatcher));
}

