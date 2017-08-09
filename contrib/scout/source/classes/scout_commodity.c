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

struct CommoditiesWinData {
    TEXT cwd_Title[WINDOW_TITLE_LENGTH];
    APTR cwd_CxList;
    APTR cwd_CxText;
    APTR cwd_CxCount;
    APTR cwd_AppearButton;
    APTR cwd_DisappearButton;
    APTR cwd_EnableButton;
    APTR cwd_DisableButton;
    APTR cwd_KillButton;
    APTR cwd_ListChgButton;
    APTR cwd_UniqueButton;
    APTR cwd_RemoveButton;
    APTR cwd_PriorityButton;
    APTR cwd_MoreButton;
};

struct CxCallbackUserData {
    struct CommoditiesWinData *ud_ClassData;
    APTR ud_List;
    ULONG ud_Count;
};

struct MUIP_CommoditiesWin_CxCommand {
    STACKED ULONG method;
    STACKED UBYTE *command;
};

HOOKPROTONHNO(cxlist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct CxEntry));
}
MakeStaticHook(cxlist_con2hook, cxlist_con2func);

HOOKPROTONHNO(cxlist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(cxlist_des2hook, cxlist_des2func);

HOOKPROTONHNO(cxlist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct CxEntry *cxe = (struct CxEntry *)msg->entry;

    if (cxe) {
        msg->strings[0] = cxe->cxe_Address;
        msg->strings[1] = cxe->cxe_Name;
        msg->strings[2] = cxe->cxe_Type;
        msg->strings[3] = cxe->cxe_Pri;
        msg->strings[4] = cxe->cxe_Flags;
        msg->strings[5] = cxe->cxe_Port;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtCommodityName;
        msg->strings[2] = txtNodeType;
        msg->strings[3] = txtNodePri;
        msg->strings[4] = txtCommodityFlags;
        msg->strings[5] = txtCommodityPort;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(cxlist_dsp2hook, cxlist_dsp2func);

STATIC LONG cxlist_cmp2colfunc( struct CxEntry *cxe1,
                                struct CxEntry *cxe2,
                                ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(cxe1->cxe_Address, cxe2->cxe_Address);
        case 1: return stricmp(cxe1->cxe_Name, cxe2->cxe_Name);
        case 2: return stricmp(cxe1->cxe_Type, cxe2->cxe_Type);
        case 3: return PriorityCompare(cxe1->cxe_Pri, cxe2->cxe_Pri);
        case 4: return HexCompare(cxe1->cxe_Flags, cxe2->cxe_Flags);
        case 5: return HexCompare(cxe1->cxe_Port, cxe2->cxe_Port);
    }
}

STATIC LONG cxlist_cmpfunc( const struct Node *n1,
                            const struct Node *n2 )
{
    return stricmp(((struct CxEntry *)n1)->cxe_Name, ((struct CxEntry *)n2)->cxe_Name);
}

HOOKPROTONHNO(cxlist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct CxEntry *cxe1, *cxe2;
    ULONG col1, col2;

    cxe1 = (struct CxEntry *)msg->entry1;
    cxe2 = (struct CxEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = cxlist_cmp2colfunc(cxe2, cxe1, col1);
    } else {
        cmp = cxlist_cmp2colfunc(cxe1, cxe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = cxlist_cmp2colfunc(cxe2, cxe1, col2);
    } else {
        cmp = cxlist_cmp2colfunc(cxe1, cxe2, col2);
    }

    return cmp;
}
MakeStaticHook(cxlist_cmp2hook, cxlist_cmp2func);

STRPTR GetCxNodeType( UBYTE type )
{
    switch (type) {
        case CX_INVALID:    return txtCommodityTypeInvalid;
        case CX_FILTER:     return txtCommodityTypeFilter;
        case CX_TYPEFILTER: return txtCommodityTypeTypefilter;
        case CX_SEND:       return txtCommodityTypeSend;
        case CX_SIGNAL:     return txtCommodityTypeSignal;
        case CX_TRANSLATE:  return txtCommodityTypeTranslate;
        case CX_BROKER:     return txtCommodityTypeBroker;
        case CX_DEBUG:      return txtCommodityTypeDebug;
        case CX_CUSTOM:     return txtCommodityTypeCustom;
        case CX_ZERO:       return txtCommodityTypeZero;
        default:            return txtCommodityTypeUnknown;
    };
}

STATIC void ReceiveList( void (* callback)( struct CxEntry *cxe, void *userData ),
                         void *userData )
{
    struct CxEntry *cxe;

    if ((cxe = tbAllocVecPooled(globalPool, sizeof(struct CxEntry))) != NULL) {
        if (SendDaemon("GetCxList")) {
            while (ReceiveDecodedEntry(cxe, sizeof(struct CxEntry))) {
                callback(cxe, userData);
            }
        }

        tbFreeVecPooled(globalPool, cxe);
    }
}

STATIC void IterateList( void (* callback)( struct CxEntry *cxe, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct CxEntry *cxe;
    struct Library *CxBase;

    NewList((struct List *)&tmplist);

    if ((CxBase = OpenLibrary("commodities.library", 37)) != NULL) {
    #if defined(__amigaos4__)
        struct CommoditiesIFace *ICommodities;
    #endif

        if (GETINTERFACE(ICommodities, CxBase)) {
            struct MsgPort *cxport;

            if ((cxport = CreateMsgPort()) != NULL) {
                struct NewBroker cxnewbroker = { NB_VERSION, (STRPTR)"« Scout Dummy »", (STRPTR)"« Scout Dummy »", (STRPTR)"Dummy Broker", 0, 0, 0, NULL, 0 };
                struct PrivateCxObj *cxbroker;

                cxnewbroker.nb_Port = cxport;

                if ((cxbroker = (struct PrivateCxObj *)CxBroker(&cxnewbroker, NULL)) != NULL) {
                    struct List *cxlist;
                    struct PrivateCxObj *mco;

                    Forbid();

                    cxlist = FindListOfNode((struct Node *)cxbroker);

                    ITERATE_LIST(cxlist, struct PrivateCxObj*, mco) {
                        if (mco != cxbroker) {
                            if ((cxe = AllocVec(sizeof(struct CxEntry), MEMF_CLEAR)) != NULL) {
                                cxe->cxe_Addr = mco;
                                _snprintf(cxe->cxe_Address, sizeof(cxe->cxe_Address), "$%08lx", mco);
                                stccpy(cxe->cxe_Type, GetCxNodeType(CxObjType((CxObj *)mco)), sizeof(cxe->cxe_Type));
                                _snprintf(cxe->cxe_Pri, sizeof(cxe->cxe_Pri), "%4ld", mco->mco_Node.ln_Pri);
                                stccpy(cxe->cxe_Name, mco->mco_Name, sizeof(cxe->cxe_Name));
                                stccpy(cxe->cxe_Title, mco->mco_Title, sizeof(cxe->cxe_Title));
                                stccpy(cxe->cxe_Description, mco->mco_Descr, sizeof(cxe->cxe_Description));
                                _snprintf(cxe->cxe_Task, sizeof(cxe->cxe_Task), "$%08lx", mco->mco_Task);
                                _snprintf(cxe->cxe_Port, sizeof(cxe->cxe_Port), "$%08lx", mco->mco_Port);
                                _snprintf(cxe->cxe_Unique, sizeof(cxe->cxe_Unique), "$%08lx", mco->mco_dummy3);
                                cxe->cxe_FlagsInt = mco->mco_Flags;
                                _snprintf(cxe->cxe_Flags, sizeof(cxe->cxe_Flags), "$%02lx", mco->mco_Flags);

                                AddTail((struct List *)&tmplist, (struct Node *)cxe);
                            }
                        }
                    }

                    Permit();

                    DeleteCxObjAll((CxObj *)cxbroker);
                }

                DeleteMsgPort(cxport);
            }

            DROPINTERFACE(ICommodities);
        }

        CloseLibrary(CxBase);
    }

    if (sort) SortLinkedList((struct List *)&tmplist, cxlist_cmpfunc);

    ITERATE_LIST(&tmplist, struct CxEntry *, cxe) {
        callback(cxe, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct CxEntry *cxe,
                            void *userData )
{
    struct CxCallbackUserData *ud = (struct CxCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, cxe);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct CxEntry *cxe,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtCommoditiesPrintLine, cxe->cxe_Address, cxe->cxe_Type, cxe->cxe_Pri, cxe->cxe_Flags, cxe->cxe_Port, cxe->cxe_Name);
}

STATIC void SendCallback( struct CxEntry *cxe,
                          UNUSED void *userData )
{
    SendEncodedEntry(cxe, sizeof(struct CxEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR cxlist, cxtext, cxcount, appearButton, disappearButton, enableButton, disableButton, killButton, listChgButton, uniqueButton, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Commodities",
        MUIA_Window_ID, MakeID('C','D','I','T'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&cxlist, MakeID('C','X','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P="MUIX_C ",BAR", &cxlist_con2hook, &cxlist_des2hook, &cxlist_dsp2hook, &cxlist_cmp2hook, TRUE),
            Child, (IPTR)MyBelowListview(&cxtext, &cxcount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(appearButton    = MakeButton(txtAppear)),
                Child, (IPTR)(disappearButton = MakeButton(txtDisappear)),
                Child, (IPTR)(enableButton    = MakeButton(txtEnable)),
                Child, (IPTR)(disableButton   = MakeButton(txtDisable)),
                Child, (IPTR)(killButton      = MakeButton(txtKill)),
                Child, (IPTR)(listChgButton   = MakeButton(txtListChange)),
                Child, (IPTR)(uniqueButton    = MakeButton(txtUnique)),
            End,

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
        struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
        APTR parent;

        cwd->cwd_CxList = cxlist;
        cwd->cwd_CxText = cxtext;
        cwd->cwd_CxCount = cxcount;
        cwd->cwd_RemoveButton = removeButton;
        cwd->cwd_PriorityButton = priorityButton;
        cwd->cwd_MoreButton = moreButton;
        cwd->cwd_AppearButton = appearButton;
        cwd->cwd_DisappearButton = disappearButton;
        cwd->cwd_EnableButton = enableButton;
        cwd->cwd_DisableButton = disableButton;
        cwd->cwd_KillButton = killButton;
        cwd->cwd_ListChgButton = listChgButton;
        cwd->cwd_UniqueButton = uniqueButton;

        parent = (APTR)xget(obj, MUIA_Window_ParentWindow);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtCommoditiesTitle, cwd->cwd_Title, sizeof(cwd->cwd_Title)));
        set(obj, MUIA_Window_DefaultObject, cxlist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, cwd->cwd_RemoveButton,
                                                          cwd->cwd_PriorityButton,
                                                          cwd->cwd_MoreButton,
                                                          cwd->cwd_AppearButton,
                                                          cwd->cwd_DisappearButton,
                                                          cwd->cwd_EnableButton,
                                                          cwd->cwd_DisableButton,
                                                          cwd->cwd_KillButton,
                                                          cwd->cwd_ListChgButton,
                                                          cwd->cwd_UniqueButton,
                                                          NULL);

        DoMethod(parent,          MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,             MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(cxlist,          MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_CommoditiesWin_ListChange);
        DoMethod(cxlist,          MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_CommoditiesWin_More);
        DoMethod(updateButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommoditiesWin_Update);
        DoMethod(printButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommoditiesWin_Print);
        DoMethod(removeButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommoditiesWin_Remove);
        DoMethod(moreButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommoditiesWin_More);
        DoMethod(priorityButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_CommoditiesWin_Priority);
        DoMethod(exitButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(appearButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Appear");
        DoMethod(disappearButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Disappear");
        DoMethod(enableButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Enable");
        DoMethod(disableButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Disable");
        DoMethod(killButton,      MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Kill");
        DoMethod(listChgButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "ListChg");
        DoMethod(uniqueButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     2, MUIM_CommoditiesWin_CxCommand, "Unique");
        DoMethod(cxlist,          MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(cwd->cwd_CxList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
    struct CxCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(cwd->cwd_CxList, MUIA_NList_Quiet, TRUE);
    DoMethod(cwd->cwd_CxList, MUIM_NList_Clear);

    ud.ud_List = cwd->cwd_CxList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(cwd->cwd_CxCount, ud.ud_Count);
    MySetContents(cwd->cwd_CxText, "");

    set(cwd->cwd_CxList, MUIA_NList_Quiet, FALSE);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, cwd->cwd_RemoveButton,
                                                      cwd->cwd_PriorityButton,
                                                      cwd->cwd_MoreButton,
                                                      cwd->cwd_AppearButton,
                                                      cwd->cwd_DisappearButton,
                                                      cwd->cwd_EnableButton,
                                                      cwd->cwd_DisableButton,
                                                      cwd->cwd_KillButton,
                                                      cwd->cwd_ListChgButton,
                                                      cwd->cwd_UniqueButton,
                                                      NULL);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintCx(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
    struct CxEntry *cxe;

    if ((cxe = (struct CxEntry *)GetActiveEntry(cwd->cwd_CxList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveCommoditiy, cxe->cxe_Name)) {
            if (MyDoCommand("RemoveCx %s", cxe->cxe_Address) == RETURN_OK) {
                DoMethod(obj, MUIM_CommoditiesWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
    struct CxEntry *cxe;

    if ((cxe = (struct CxEntry *)GetActiveEntry(cwd->cwd_CxList)) != NULL) {
        LONG pri;

        pri = atol(cxe->cxe_Pri);
        if (GetPriority(cxe->cxe_Name, &pri)) {
            if (MyDoCommand("SetCxPri %s %ld", cxe->cxe_Address, pri) == RETURN_OK) {
                _snprintf(cxe->cxe_Pri, sizeof(cxe->cxe_Pri), "%4ld", pri);
                RedrawActiveEntry(cwd->cwd_CxList);
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
        struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
        struct CxEntry *cxe;

        if ((cxe = (struct CxEntry *)GetActiveEntry(cwd->cwd_CxList)) != NULL) {
            APTR detailWin;

            if ((detailWin = (Object *)(CommoditiesDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                End)) != NULL) {
                COLLECT_RETURNIDS;
                set(detailWin, MUIA_CommoditiesDetailWin_Commodity, cxe);
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
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
    struct CxEntry *cxe;

    if ((cxe = (struct CxEntry *)GetActiveEntry(cwd->cwd_CxList)) != NULL) {
        MySetContents(cwd->cwd_CxText, "%s \"%s\"", cxe->cxe_Address, cxe->cxe_Name);
        if (!clientstate) set(cwd->cwd_MoreButton, MUIA_Disabled, FALSE);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, cwd->cwd_RemoveButton,
                                                           cwd->cwd_PriorityButton,
                                                           cwd->cwd_EnableButton,
                                                           cwd->cwd_DisableButton,
                                                           cwd->cwd_KillButton,
                                                           cwd->cwd_ListChgButton,
                                                           cwd->cwd_UniqueButton,
                                                           NULL);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FLAG_IS_CLEAR(cxe->cxe_FlagsInt, COF_SHOW_HIDE), cwd->cwd_AppearButton,
                                                                                                     cwd->cwd_DisappearButton,
                                                                                                     NULL);
    }

    return 0;
}

STATIC IPTR mCxCommand( struct IClass *cl,
                         Object *obj,
                         struct MUIP_CommoditiesWin_CxCommand *msg )
{
    struct CommoditiesWinData *cwd = INST_DATA(cl, obj);
    struct CxEntry *cxe;

    if ((cxe = (struct CxEntry *)GetActiveEntry(cwd->cwd_CxList)) != NULL) {
        if (MyDoCommand("Cx%s \"%s\"", msg->command, cxe->cxe_Name) == RETURN_OK) {
            Delay(25);
            DoMethod(obj, MUIM_CommoditiesWin_Update);
        }
    }

    return 0;
}

DISPATCHER(CommoditiesWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                         return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                     return (mDispose(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_Remove:     return (mRemove(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_Priority:   return (mPriority(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
        case MUIM_CommoditiesWin_CxCommand:  return (mCxCommand(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintCx( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtCommoditiesPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendCxList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeCommoditiesWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct CommoditiesWinData), ENTRY(CommoditiesWinDispatcher));
}


