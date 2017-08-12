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

struct ExpansionsWinData {
    TEXT ewd_Title[WINDOW_TITLE_LENGTH];
    APTR ewd_ExpansionList;
    APTR ewd_ExpansionText;
    APTR ewd_ExpansionCount;
    APTR ewd_MoreButton;
};

struct ExpansionCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

HOOKPROTONHNO(explist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct ExpansionEntry));
}
MakeStaticHook(explist_con2hook, explist_con2func);

HOOKPROTONHNO(explist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(explist_des2hook, explist_des2func);

HOOKPROTONHNO(explist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct ExpansionEntry *ee = (struct ExpansionEntry *)msg->entry;

    if (ee) {
        msg->strings[0] = ee->ee_Address;
        msg->strings[1] = ee->ee_BoardAddr;
        msg->strings[2] = ee->ee_Type;
        msg->strings[3] = ee->ee_ManufName;
        msg->strings[4] = ee->ee_ProdName;
        msg->strings[5] = ee->ee_ProdClass;
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtExpansionBoardAddr;
        msg->strings[2] = txtExpansionType;
        msg->strings[3] = txtExpansionManufacturer;
        msg->strings[4] = txtExpansionProduct;
        msg->strings[5] = txtExpansionClass;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(explist_dsp2hook, explist_dsp2func);

STATIC LONG explist_cmp2colfunc( struct ExpansionEntry *ee1,
                                 struct ExpansionEntry *ee2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(ee1->ee_Address, ee2->ee_Address);
        case 1: return HexCompare(ee1->ee_BoardAddr, ee2->ee_BoardAddr);
        case 2: return HexCompare(ee1->ee_Type, ee2->ee_Type);
        case 3: return stricmp(ee1->ee_ManufName, ee2->ee_ManufName);
        case 4: return stricmp(ee1->ee_ProdName, ee2->ee_ProdName);
        case 5: return stricmp(ee1->ee_ProdClass, ee2->ee_ProdClass);
    }
}

HOOKPROTONHNO(explist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct ExpansionEntry *ee1, *ee2;
    ULONG col1, col2;

    ee1 = (struct ExpansionEntry *)msg->entry1;
    ee2 = (struct ExpansionEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = explist_cmp2colfunc(ee2, ee1, col1);
    } else {
        cmp = explist_cmp2colfunc(ee1, ee2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = explist_cmp2colfunc(ee2, ee1, col2);
    } else {
        cmp = explist_cmp2colfunc(ee1, ee2, col2);
    }

    return cmp;
}
MakeStaticHook(explist_cmp2hook, explist_cmp2func);

STATIC void ReceiveList( void (* callback)( struct ExpansionEntry *ee, void *userData ),
                         void *userData )
{
    struct ExpansionEntry *ee;

    if ((ee = tbAllocVecPooled(globalPool, sizeof(struct ExpansionEntry))) != NULL) {
        if (SendDaemon("GetExpList")) {
            while (ReceiveDecodedEntry(ee, sizeof(struct ExpansionEntry))) {
                callback(ee, userData);
            }
        }

        tbFreeVecPooled(globalPool, ee);
    }
}

STATIC void IterateList( void (* callback)( struct ExpansionEntry *ee, void *userData ),
                         void *userData )
{
    struct ExpansionEntry *ee;

    if ((ee = tbAllocVecPooled(globalPool, sizeof(struct ExpansionEntry))) != NULL) {
        #ifdef __MORPHOS__
        struct Library *OpenPciBase;
        #endif
        struct ExpansionBase *ExpansionBase;

        if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary(EXPANSIONNAME, 37)) != NULL) {
        #if defined(__amigaos4__)
            struct ExpansionIFace *IExpansion;
            struct PCIIFace *IPCI;
            struct PCIDevice *pci;
            ULONG index;
        #endif

            if (GETINTERFACE(IExpansion, ExpansionBase)) {
                struct ConfigDev *cd;

                cd = NULL;
                while ((cd = FindConfigDev(cd, -1, -1)) != NULL) {
                    ee->ee_Addr = cd;
                    _snprintf(ee->ee_Address, sizeof(ee->ee_Address), ADDRESS_FORMAT, cd);
                    _snprintf(ee->ee_Flags, sizeof(ee->ee_Flags), "$%02lx", cd->cd_Flags);
                    _snprintf(ee->ee_BoardAddr, sizeof(ee->ee_BoardAddr), ADDRESS_FORMAT, cd->cd_BoardAddr);
                    _snprintf(ee->ee_BoardSize, sizeof(ee->ee_BoardSize), "$%08lx", cd->cd_BoardSize);
                    _snprintf(ee->ee_Type, sizeof(ee->ee_Type), "$%02lx", cd->cd_Rom.er_Type);
                    _snprintf(ee->ee_Product, sizeof(ee->ee_Product), "%lu", cd->cd_Rom.er_Product);
                    _snprintf(ee->ee_Manufacturer, sizeof(ee->ee_Manufacturer), "%lu", cd->cd_Rom.er_Manufacturer);
                    _snprintf(ee->ee_SerialNumber, sizeof(ee->ee_SerialNumber), "%lU", cd->cd_Rom.er_SerialNumber);
                    stccpy(ee->ee_HardwareType, "ZORRO", sizeof(ee->ee_HardwareType));

                    if (IdentifyBase) {
                        IdExpansionTags(IDTAG_ConfigDev, (IPTR)cd,
                                        IDTAG_ManufStr, (IPTR)ee->ee_ManufName,
                                        IDTAG_ProdStr, (IPTR)ee->ee_ProdName,
                                        IDTAG_ClassStr, (IPTR)ee->ee_ProdClass,
                                        IDTAG_Localize, FALSE,
                                        TAG_DONE);
                    } else {
                        _snprintf(ee->ee_ManufName, sizeof(ee->ee_ManufName), "%lu - %s", cd->cd_Rom.er_Manufacturer, txtNoIdentifyLibrary);
                        _snprintf(ee->ee_ProdName, sizeof(ee->ee_ProdName), "%lu - %s", cd->cd_Rom.er_Product, txtNoIdentifyLibrary);
                        ee->ee_ProdClass[0] = 0x00;
                    }

                    callback(ee, userData);
                }

                DROPINTERFACE(IExpansion);
            }

        #if defined(__amigaos4__)
            if ((IPCI = (struct PCIIFace *)GetInterface((struct Library *)ExpansionBase, "pci", 1, NULL)) != NULL) {
                index = 0;
                while ((pci = IPCI->FindDeviceTags(FDT_Index, index, TAG_DONE)) != NULL) {
                    ee->ee_Addr = (APTR)pci;
                    _snprintf(ee->ee_Address, sizeof(ee->ee_Address), "$%08lx", pci);
                    _snprintf(ee->ee_Flags, sizeof(ee->ee_Flags), txtNotAvailable);
                    _snprintf(ee->ee_BoardAddr, sizeof(ee->ee_BoardAddr), "---");
                    _snprintf(ee->ee_BoardSize, sizeof(ee->ee_BoardSize), txtNotAvailable);
                    _snprintf(ee->ee_Type, sizeof(ee->ee_Type), "PCI");
                    _snprintf(ee->ee_Product, sizeof(ee->ee_Product), "$%04lx", pci->ReadConfigWord(PCI_DEVICE_ID));
                    _snprintf(ee->ee_ProdName, sizeof(ee->ee_ProdName), "$%04lx", pci->ReadConfigWord(PCI_DEVICE_ID));
                    _snprintf(ee->ee_Manufacturer, sizeof(ee->ee_Manufacturer), "$%04lx", pci->ReadConfigWord(PCI_VENDOR_ID));
                    _snprintf(ee->ee_ManufName, sizeof(ee->ee_ManufName), "$%04lx", pci->ReadConfigWord(PCI_VENDOR_ID));
                    _snprintf(ee->ee_ProdClass, sizeof(ee->ee_ProdClass), "$%04lx", pci->ReadConfigWord(PCI_CLASS_DEVICE));
                    _snprintf(ee->ee_SerialNumber, sizeof(ee->ee_SerialNumber), txtNotAvailable);
                    stccpy(ee->ee_HardwareType, "PCI", sizeof(ee->ee_HardwareType));

                    IPCI->FreeDevice(pci);
                    index++;

                    callback(ee, userData);
                }

                DropInterface((struct Interface *)IPCI);
            }
        #endif

            CloseLibrary((struct Library *)ExpansionBase);
        }

    #if defined(__MORPHOS__)
        if ((OpenPciBase = OpenLibrary("openpci.library", 0)) != NULL) {
            struct Library *PCIIDSBase;
            struct pci_dev *dev = NULL;

            PCIIDSBase = OpenLibrary("pciids.library", 50);

            while ((dev = pci_find_device(0xffff, 0xffff, dev)) != NULL) {
                ee->ee_Addr = (APTR)dev;
                _snprintf(ee->ee_Address, sizeof(ee->ee_Address), "$%08lx", dev);
                _snprintf(ee->ee_Flags, sizeof(ee->ee_Flags), txtNotAvailable);
                _snprintf(ee->ee_BoardAddr, sizeof(ee->ee_BoardAddr), "---");
                _snprintf(ee->ee_BoardSize, sizeof(ee->ee_BoardSize), txtNotAvailable);
                _snprintf(ee->ee_Type, sizeof(ee->ee_Type), "PCI");
                _snprintf(ee->ee_Product, sizeof(ee->ee_Product), "$%04lx", dev->device);
                _snprintf(ee->ee_ProdName, sizeof(ee->ee_ProdName), "$%04lx", dev->device);
                _snprintf(ee->ee_Manufacturer, sizeof(ee->ee_Manufacturer), "$%04lx", dev->vendor);
                _snprintf(ee->ee_ManufName, sizeof(ee->ee_ManufName), "$%04lx", dev->vendor);
                /* New MorphOS SDK has this field renamed because of conflict with C++ keyword.
                   Perhaps m68k SDK should be upgraded too. */
            #if defined(__MORPHOS__)
                _snprintf(ee->ee_ProdClass, sizeof(ee->ee_ProdClass), "$%04lx", dev->devclass);
            #else
                _snprintf(ee->ee_ProdClass, sizeof(ee->ee_ProdClass), "$%04lx", dev->class);
            #endif
                _snprintf(ee->ee_SerialNumber, sizeof(ee->ee_SerialNumber), txtNotAvailable);
                stccpy(ee->ee_HardwareType, "PCI", sizeof(ee->ee_HardwareType));

                if (PCIIDSBase) {
                    CONST_STRPTR p;

                    if ((p = PCIIDS_GetVendorName(dev->vendor)))
                        stccpy(ee->ee_ManufName, p, sizeof(ee->ee_ManufName));

                    if ((p = PCIIDS_GetDeviceName(dev->vendor, dev->device)))
                        stccpy(ee->ee_ProdName, p, sizeof(ee->ee_ProdName));
                }

                callback(ee, userData);
            }
            CloseLibrary(PCIIDSBase);
            CloseLibrary(OpenPciBase);
        }
    #endif

        tbFreeVecPooled(globalPool, ee);
    }
}

STATIC void UpdateCallback( struct ExpansionEntry *ee,
                            void *userData )
{
    struct ExpansionCallbackUserData *ud = (struct ExpansionCallbackUserData *)userData;

    InsertBottomEntry(ud->ud_List, ee);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct ExpansionEntry *ee,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtExpansionsPrintLine, ee->ee_Address, ee->ee_Flags, ee->ee_BoardAddr, ee->ee_BoardSize, ee->ee_Type, ee->ee_Manufacturer, ee->ee_Product, ee->ee_SerialNumber, ee->ee_ManufName, ee->ee_ProdName, ee->ee_ProdClass);
}

STATIC void SendCallback( struct ExpansionEntry *ee,
                          UNUSED void *userData )
{
    SendEncodedEntry(ee, sizeof(struct ExpansionEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR explist, exptext, updateButton, printButton, moreButton, exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Expansions",
        MUIA_Window_ID, MakeID('E','X','P','A'),
        WindowContents, VGroup,

            Child, (IPTR)MyNListviewObject(&explist, MakeID('E','X','L','V'), "BAR,BAR,BAR P=" MUIX_C ",BAR,BAR,BAR", &explist_con2hook, &explist_des2hook, &explist_dsp2hook, &explist_cmp2hook, TRUE),
            Child, (IPTR)(exptext = MyTextObject()),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton = MakeButton(txtUpdate)),
                Child, (IPTR)(printButton  = MakeButton(txtPrint)),
                Child, (IPTR)(moreButton   = MakeButton(txtMore)),
                Child, (IPTR)(exitButton   = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ExpansionsWinData *ewd = INST_DATA(cl, obj);
        APTR parent;

        ewd->ewd_ExpansionList = explist;
        ewd->ewd_ExpansionText = exptext;
        ewd->ewd_MoreButton = moreButton;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtExpansionsTitle, ewd->ewd_Title, sizeof(ewd->ewd_Title)));
        set(obj, MUIA_Window_DefaultObject, explist);
        set(moreButton, MUIA_Disabled, TRUE);

        DoMethod(parent,       MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,          MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(explist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_ExpansionsWin_ListChange);
        DoMethod(explist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_ExpansionsWin_More);
        DoMethod(updateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ExpansionsWin_Update);
        DoMethod(printButton,  MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ExpansionsWin_Print);
        DoMethod(moreButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_ExpansionsWin_More);
        DoMethod(exitButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct ExpansionsWinData *ewd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(ewd->ewd_ExpansionList, MUIM_NList_Clear);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct ExpansionsWinData *ewd = INST_DATA(cl, obj);
    struct ExpansionCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(ewd->ewd_ExpansionList, MUIA_NList_Quiet, TRUE);
    DoMethod(ewd->ewd_ExpansionList, MUIM_NList_Clear);

    ud.ud_List = ewd->ewd_ExpansionList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud);
    }

    MySetContents(ewd->ewd_ExpansionText, "");

    set(ewd->ewd_ExpansionList, MUIA_NList_Quiet, FALSE);
    set(ewd->ewd_MoreButton, MUIA_Disabled, TRUE);
    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintExpansions(NULL);

    return 0;
}

STATIC IPTR mMore( struct IClass *cl,
                    Object *obj,
                    UNUSED Msg msg )
{
    if (!clientstate) {
        struct ExpansionsWinData *ewd = INST_DATA(cl, obj);
        struct ExpansionEntry *ee;

        if ((ee = (struct ExpansionEntry *)GetActiveEntry(ewd->ewd_ExpansionList)) != NULL) {
            if (!strcmp(ee->ee_HardwareType, "ZORRO")) {
                APTR detailWin;

                if ((detailWin = (Object *)ExpansionsDetailWindowObject,
                    MUIA_Window_ParentWindow, (IPTR)obj,
                    End) != NULL) {
                    COLLECT_RETURNIDS;
                    set(detailWin, MUIA_ExpansionsDetailWin_Expansion, ee);
                    set(detailWin, MUIA_Window_Open, TRUE);
                    REISSUE_RETURNIDS;
                }
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct ExpansionsWinData *ewd = INST_DATA(cl, obj);
    struct ExpansionEntry *ee;

    if ((ee = (struct ExpansionEntry *)GetActiveEntry(ewd->ewd_ExpansionList)) != NULL) {
        MySetContents(ewd->ewd_ExpansionText, txtExpansionDetails, ee->ee_BoardSize, ee->ee_Flags, ee->ee_Manufacturer, ee->ee_Product, ee->ee_SerialNumber);
        if (!clientstate) set(ewd->ewd_MoreButton, MUIA_Disabled, strcmp(ee->ee_HardwareType, "ZORRO"));
    }

    return 0;
}

DISPATCHER(ExpansionsWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                        return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                    return (mDispose(cl, obj, (APTR)msg));
        case MUIM_ExpansionsWin_Update:     return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_ExpansionsWin_Print:      return (mPrint(cl, obj, (APTR)msg));
        case MUIM_ExpansionsWin_More:       return (mMore(cl, obj, (APTR)msg));
        case MUIM_ExpansionsWin_ListChange: return (mListChange(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintExpansions( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtExpansionsPrintHeader);
        IterateList(PrintCallback, (void *)handle);
    }

    HandlePrintStop();
}

void SendExpList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL);
}

APTR MakeExpansionsWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ExpansionsWinData), ENTRY(ExpansionsWinDispatcher));
}


