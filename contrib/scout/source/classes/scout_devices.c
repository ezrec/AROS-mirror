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

struct DevicesWinData {
    TEXT dwd_Title[WINDOW_TITLE_LENGTH];
    APTR dwd_DeviceList;
    APTR dwd_DeviceText;
    APTR dwd_DeviceCount;
    APTR dwd_RemoveButton;
    APTR dwd_PriorityButton;
    APTR dwd_MoreButton;
    APTR dwd_FunctionsButton;
};

struct DeviceCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(devlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct DeviceEntry));
}
MakeStaticHook(devlist_con2hook, devlist_con2func);

HOOKPROTONHNO(devlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(devlist_des2hook, devlist_des2func);

HOOKPROTONHNO(devlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct DeviceEntry *de = (struct DeviceEntry *)msg->entry;

    if (de) {
        msg->strings[0] = de->de_Address;
        msg->strings[1] = de->de_Name;
        msg->strings[2] = de->de_Type;
        msg->strings[3] = de->de_Pri;
        msg->strings[4] = de->de_Version;
        msg->strings[5] = de->de_OpenCount;
        msg->strings[6] = de->de_RAMPtrCount;
        msg->strings[7] = de->de_CodeType;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtDeviceVersion;
        msg->strings[5] = txtDeviceOpenCount;
        msg->strings[6] = txtDeviceRPC;
        msg->strings[7] = txtDeviceCodeType;
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
MakeStaticHook(devlist_dsp2hook, devlist_dsp2func);

STATIC LONG devlist_cmp2colfunc( struct DeviceEntry *de1,
                                 struct DeviceEntry *de2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(de1->de_Address, de2->de_Address);
        case 1: return stricmp(de1->de_Name, de2->de_Name);
        case 2: return stricmp(de1->de_Type, de2->de_Type);
        case 3: return PriorityCompare(de1->de_Pri, de2->de_Pri);
        case 4: return VersionCompare(de1->de_Version, de2->de_Version);
        case 5: return IntegerCompare(de1->de_OpenCount, de2->de_OpenCount);
        case 6: return stricmp(de1->de_RAMPtrCount, de2->de_RAMPtrCount);
        case 7: return stricmp(de1->de_CodeType, de2->de_CodeType);
    }
}

STATIC LONG devlist_cmpfunc( const struct Node *n1,
                             const struct Node *n2 )
{
    return stricmp(((struct DeviceEntry *)n1)->de_Name, ((struct DeviceEntry *)n2)->de_Name);
}

HOOKPROTONHNO(devlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct DeviceEntry *de1, *de2;
    ULONG col1, col2;

    de1 = (struct DeviceEntry *)msg->entry1;
    de2 = (struct DeviceEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = devlist_cmp2colfunc(de2, de1, col1);
    } else {
        cmp = devlist_cmp2colfunc(de1, de2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = devlist_cmp2colfunc(de2, de1, col2);
    } else {
        cmp = devlist_cmp2colfunc(de1, de2, col2);
    }

    return cmp;
}
MakeStaticHook(devlist_cmp2hook, devlist_cmp2func);

void FlushDevices( void )
{
    struct Device *dd;

    Forbid();

    dd = (struct Device *)SysBase->DeviceList.lh_Head;
    while (dd) {
        struct Device *_dd;

        _dd = (struct Device *)dd->dd_Library.lib_Node.ln_Succ;

        if (dd->dd_Library.lib_OpenCnt == 0) {
            RemDevice(dd);
        }

        dd = _dd;
    }

    Permit();
}

STATIC void ReceiveList( void (* callback)( struct DeviceEntry *de, void *userData ),
                         void *userData )
{
    struct DeviceEntry *de;

    if ((de = tbAllocVecPooled(globalPool, sizeof(struct DeviceEntry))) != NULL) {
        if (SendDaemon("GetDevList")) {
            while (ReceiveDecodedEntry(de, sizeof(struct DeviceEntry))) {
                callback(de, userData);
            }
        }

        tbFreeVecPooled(globalPool, de);
    }
}

STATIC void IterateList( void (* callback)( struct DeviceEntry *de, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct Device *dd;
    struct MinList tmplist;
    struct DeviceEntry *de;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->DeviceList, struct Device *, dd) {
        if ((de = AllocVec(sizeof(struct DeviceEntry), MEMF_CLEAR)) != NULL) {
            LONG cnt, total;

            de->de_Addr = dd;

            _snprintf(de->de_Address, sizeof(de->de_Address), ADDRESS_FORMAT, dd);
            stccpy(de->de_Name, dd->dd_Library.lib_Node.ln_Name, sizeof(de->de_Name));
            _snprintf(de->de_Pri, sizeof(de->de_Pri), "%4ld", dd->dd_Library.lib_Node.ln_Pri);
            _snprintf(de->de_Version, sizeof(de->de_Version), "%ld.%ld", dd->dd_Library.lib_Version, dd->dd_Library.lib_Revision);
            _snprintf(de->de_OpenCount, sizeof(de->de_OpenCount), "%6lD", dd->dd_Library.lib_OpenCnt);

            GetRamPointerCount(&dd->dd_Library, &cnt, &total);
            _snprintf(de->de_RAMPtrCount, sizeof(de->de_RAMPtrCount), "%4lD", cnt);

            stccpy(de->de_Type, GetNodeType(dd->dd_Library.lib_Node.ln_Type), sizeof(de->de_Type));

        #if defined(__AROS__)
            stccpy(de->de_CodeType, txtDeviceCodeTypeNative, sizeof(de->de_CodeType));
        #else
            #if defined(__amigaos4__)
                if (dd->dd_Library.lib_ABIVersion == LIBABI_MIFACE) {
            #elif defined(__MORPHOS__)
                if (*(UBYTE *)(((struct JumpEntry *)dd) - 1)->je_JumpAddress == 0xff) {
            #else
                // OS3 is 68k only
                if (FALSE) {
            #endif
                stccpy(de->de_CodeType, txtDeviceCodeTypePPC, sizeof(de->de_CodeType));
            } else {
                stccpy(de->de_CodeType, txtDeviceCodeType68k, sizeof(de->de_CodeType));
            }
        #endif // __AROS__

            AddTail((struct List *)&tmplist, (struct Node *)de);
        }
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, devlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct DeviceEntry *, de) {
        callback(de, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct DeviceEntry *de,
                            void *userData )
{
    struct DeviceCallbackUserData *ud = (struct DeviceCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, de);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct DeviceEntry *de,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtDevicesPrintLine, de->de_Address, de->de_Pri, de->de_OpenCount, de->de_RAMPtrCount, de->de_Name, de->de_Version, de->de_CodeType);
}

STATIC void SendCallback( struct DeviceEntry *de,
                          UNUSED void *userData )
{
    SendEncodedEntry(de, sizeof(struct DeviceEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR devlist, devtext, devcount, updateButton, printButton, removeButton, priorityButton, moreButton, funcButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Devices",
        MUIA_Window_ID, MakeID('D','E','V','S'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&devlist, MakeID('D','E','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C, &devlist_con2hook, &devlist_des2hook, &devlist_dsp2hook, &devlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&devtext, &devcount),

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
        struct DevicesWinData *dwd = INST_DATA(cl, obj);
        APTR parent;

        dwd->dwd_DeviceList = devlist;
        dwd->dwd_DeviceText = devtext;
        dwd->dwd_DeviceCount = devcount;
        dwd->dwd_RemoveButton = removeButton;
        dwd->dwd_PriorityButton = priorityButton;
        dwd->dwd_MoreButton = moreButton;
        dwd->dwd_FunctionsButton = funcButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtDevicesTitle, dwd->dwd_Title, sizeof(dwd->dwd_Title)));
        set(obj, MUIA_Window_DefaultObject, devlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, dwd->dwd_RemoveButton,
                                                          dwd->dwd_PriorityButton,
                                                          dwd->dwd_MoreButton,
                                                          dwd->dwd_FunctionsButton,
                                                          NULL);

        DoMethod(parent,          MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,             MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(devlist,         MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_DevicesWin_ListChange);
        DoMethod(devlist,         MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_DevicesWin_More);
        DoMethod(updateButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_Update);
        DoMethod(printButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_Print);
        DoMethod(removeButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_Remove);
        DoMethod(moreButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_More);
        DoMethod(priorityButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_Priority);
        DoMethod(funcButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_DevicesWin_Functions);
        DoMethod(exitButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(devlist,         MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct DevicesWinData *dwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(dwd->dwd_DeviceList, MUIM_NList_Clear);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct DevicesWinData *dwd = INST_DATA(cl, obj);
    struct DeviceCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(dwd->dwd_DeviceList, MUIA_NList_Quiet, TRUE);
    DoMethod(dwd->dwd_DeviceList, MUIM_NList_Clear);

    ud.ud_List = dwd->dwd_DeviceList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(dwd->dwd_DeviceCount, ud.ud_Count);
    MySetContents(dwd->dwd_DeviceText, "");

    set(dwd->dwd_DeviceList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, dwd->dwd_RemoveButton,
                                                      dwd->dwd_PriorityButton,
                                                      dwd->dwd_MoreButton,
                                                      dwd->dwd_FunctionsButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintDevices(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct DevicesWinData *dwd = INST_DATA(cl, obj);
    struct DeviceEntry *de;

    if ((de = (struct DeviceEntry *)GetActiveEntry(dwd->dwd_DeviceList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveDevice, de->de_Name)) {
            if (MyDoCommand("RemoveDevice \"%s\"", de->de_Name) == RETURN_OK) {
                DoMethod(obj, MUIM_DevicesWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct DevicesWinData *dwd = INST_DATA(cl, obj);
    struct DeviceEntry *de;

    if ((de = (struct DeviceEntry *)GetActiveEntry(dwd->dwd_DeviceList)) != NULL) {
        LONG pri;

        pri = atol(de->de_Pri);
        if (GetPriority(de->de_Name, &pri)) {
            if (MyDoCommand("SetPriority DEVICE \"%s\" %ld", de->de_Name, pri) == RETURN_OK) {
                _snprintf(de->de_Pri, sizeof(de->de_Pri), "%4ld", pri);
                RedrawActiveEntry(dwd->dwd_DeviceList);
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
        struct DevicesWinData *dwd = INST_DATA(cl, obj);
        struct DeviceEntry *de;

        if ((de = (struct DeviceEntry *)GetActiveEntry(dwd->dwd_DeviceList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)(DevicesDetailWindowObject,
                    MUIA_Window_Title, (IPTR)de->de_Name,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End)) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_DevicesDetailWin_Device, de);
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
    struct DevicesWinData *dwd = INST_DATA(cl, obj);
    struct DeviceEntry *de;

    if ((de = (struct DeviceEntry *)GetActiveEntry(dwd->dwd_DeviceList)) != NULL) {
        APTR funcWin;

        if ((funcWin = (Object *)FunctionsWindowObject,
                MUIA_Window_ParentWindow, (IPTR)obj,
            End) != NULL) {
            DoMethod(funcWin, MUIM_FunctionsWin_ShowFunctions, MUIV_FunctionsWin_NodeType_Device, de->de_Addr, de->de_Name);
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct DevicesWinData *dwd = INST_DATA(cl, obj);
    struct DeviceEntry *de;

    if ((de = (struct DeviceEntry *)GetActiveEntry(dwd->dwd_DeviceList)) != NULL) {
        MySetContents(dwd->dwd_DeviceText, "%s \"%s\"", de->de_Address, de->de_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, dwd->dwd_RemoveButton,
                                                           dwd->dwd_PriorityButton,
                                                           (clientstate) ? NULL : dwd->dwd_MoreButton,
                                                           (clientstate) ? NULL : dwd->dwd_FunctionsButton,
                                                           NULL);
    }

    return 0;
}

DISPATCHER(DevicesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                 return (mDispose(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_Functions:  return (mFunctions(cl, obj, (APTR)msg));
        case MUIM_DevicesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

void PrintDevices( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtDevicesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendDevList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeDevicesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct DevicesWinData), ENTRY(DevicesWinDispatcher));
}

