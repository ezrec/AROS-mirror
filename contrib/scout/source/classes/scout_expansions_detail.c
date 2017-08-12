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

struct ExpansionsDetailWinData {
    TEXT edwd_Title[WINDOW_TITLE_LENGTH];
    APTR edwd_Texts[20];
    APTR edwd_MainGroup;
    struct ExpansionEntry *edwd_Expansion;
    TEXT edwd_Temp[PATH_LENGTH];
};

STATIC CONST struct LongFlag cdFlags[] = {
    { CDF_SHUTUP,    "CDF_SHUTUP" },
    { CDF_CONFIGME,  "CDF_CONFIGME" },
    { CDF_BADMEMORY, "CDF_BADMEMORY" },
    { CDF_PROCESSED, "CDF_PROCESSED" },
    { 0x10,          "< ??? >" },
    { 0x20,          "< ??? >" },
    { 0x40,          "< ??? >" },
    { 0x80,          "< ??? >" },
    { 0,             NULL },
};

STATIC CONST struct LongFlag erFlags[] = {
    { 0x01,           "< ??? >" },
    { 0x02,           "< ??? >" },
    { 0x04,           "< ??? >" },
    { 0x08,           "< ??? >" },
    { ERFF_ZORRO_III, "ERFF_ZORRO_III" },
    { ERFF_EXTENDED,  "ERFF_EXTENDED" },
    { ERFF_NOSHUTUP,  "ERFF_NOSHUTUP" },
    { ERFF_MEMSPACE,  "ERFF_MEMSPACE" },
    { 0,              NULL },
};

STATIC CONST struct MaskedLongFlag diagConfigs[] = {
    { DAC_NIBBLEWIDE, DAC_BUSWIDTH, "DAC_NIBBLEWIDE" },
    { DAC_BYTEWIDE,   DAC_BUSWIDTH, "DAC_BYTEWIDE" },
    { DAC_WORDWIDE,   DAC_BUSWIDTH, "DAC_WORDWIDE" },
    { DAC_BUSWIDTH,   DAC_BUSWIDTH, "DAC_WORDWIDE" },
    { DAC_NEVER,      DAC_BOOTTIME, "DAC_NEVER" },
    { DAC_CONFIGTIME, DAC_BOOTTIME, "DAC_CONFIGTIME" },
    { DAC_BINDTIME,   DAC_BOOTTIME, "DAC_BINDTIME" },
    { DAC_BOOTTIME,   DAC_BOOTTIME, "DAC_BINDTIME" },
    { 0,              0,            NULL },
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct ExpansionsDetailWinData *edwd = INST_DATA(cl, obj);
    struct ExpansionEntry *ee = edwd->edwd_Expansion;
    struct ConfigDev *cd = ee->ee_Addr;
    ULONG i, memsize;
    STATIC TEXT ertype[] = { ERTF_MEMLIST, ERTF_DIAGVALID, ERTF_CHAINEDCONFIG, 0 };
    STATIC CONST_STRPTR ertypetext[] = { "ERTF_MEMLIST", "ERTF_DIAGVALID", "ERTF_CHAINEDCONFIG" };
    STATIC CONST_STRPTR z3subsize[] = { "SAME", "AUTO", "64K", "128K", "256K", "512K", "1M", "2M", "4M", "6M", "8M", "10M", "12M", "14M", "RESERVED", "RESERVED" };

    MySetContents(edwd->edwd_Texts[ 0], " %4lu ", cd->cd_Rom.er_Manufacturer);
    MySetContents(edwd->edwd_Texts[ 2], " %4lu ", cd->cd_Rom.er_Product);
    if (IdentifyBase) {
        MySetContents(edwd->edwd_Texts[ 1], "%s", ee->ee_ManufName);
        MySetContents(edwd->edwd_Texts[ 3], "%s", ee->ee_ProdName);
        MySetContents(edwd->edwd_Texts[ 4], "%s", ee->ee_ProdClass);
    } else {
        MySetContents(edwd->edwd_Texts[ 1], MUIX_PH "%s", txtNoIdentifyLibrary);
        MySetContents(edwd->edwd_Texts[ 3], MUIX_PH "%s", txtNoIdentifyLibrary);
        MySetContents(edwd->edwd_Texts[ 4], MUIX_PH "%s", txtNoIdentifyLibrary);
    }

    MySetContents(edwd->edwd_Texts[ 5], ADDRESS_FORMAT, cd);
    MySetContents(edwd->edwd_Texts[ 6], ADDRESS_FORMAT, cd->cd_BoardAddr);
    MySetContents(edwd->edwd_Texts[ 7], "$%08lx", cd->cd_BoardSize); // ULONG
    MySetContents(edwd->edwd_Texts[ 8], "$%08lx", cd->cd_SlotAddr); // UWORD
    MySetContents(edwd->edwd_Texts[ 9], "%lD", cd->cd_SlotSize);
    MySetContents(edwd->edwd_Texts[10], ADDRESS_FORMAT, cd->cd_Driver);
    MySetContents(edwd->edwd_Texts[11], ADDRESS_FORMAT, cd->cd_NextCD);
    set(edwd->edwd_Texts[12], MUIA_FlagsButton_Flags, cd->cd_Flags);

    edwd->edwd_Temp[0] = 0x00;
    if ((cd->cd_Rom.er_Type & ERT_TYPEMASK) == ERT_ZORROIII) {
        _strcatn(edwd->edwd_Temp, "ERT_ZORRO3, ", sizeof(edwd->edwd_Temp));
    } else {
        _strcatn(edwd->edwd_Temp, "ERT_ZORRO2, ", sizeof(edwd->edwd_Temp));
    }

    i = 0;
    while (ertype[i]) {
        if (FLAG_IS_SET(cd->cd_Rom.er_Type, ertype[i])) {
            _snprintf(edwd->edwd_Temp, sizeof(edwd->edwd_Temp), "%s%s, ", edwd->edwd_Temp, ertypetext[i]);
        }
        i++;
    }

    memsize = cd->cd_Rom.er_Type & ERT_MEMMASK;
    _strcatn(edwd->edwd_Temp, "MEMSIZE=", sizeof(edwd->edwd_Temp));
    if (FLAG_IS_SET(cd->cd_Rom.er_Flags, ERFF_EXTENDED)) {
        switch (memsize) {
            case 0x07: _strcatn(edwd->edwd_Temp, "?", sizeof(edwd->edwd_Temp)); break;
            case 0x06: _strcatn(edwd->edwd_Temp, "1G", sizeof(edwd->edwd_Temp)); break;
            default: _snprintf(edwd->edwd_Temp, sizeof(edwd->edwd_Temp), "%s%lDM", edwd->edwd_Temp, (1 << memsize) << 4); break;
        }
    } else {
        switch (memsize) {
            case 0x00: _strcatn(edwd->edwd_Temp, "8M", sizeof(edwd->edwd_Temp)); break;
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04: _snprintf(edwd->edwd_Temp, sizeof(edwd->edwd_Temp), "%s%lDK", edwd->edwd_Temp, (1 << (memsize - 1) << 6)); break;
            default: _snprintf(edwd->edwd_Temp, sizeof(edwd->edwd_Temp), "%s%lDM", edwd->edwd_Temp, (1 << (memsize - 5))); break;
        }
    }

    if ((cd->cd_Rom.er_Type & ERT_TYPEMASK) == ERT_ZORROIII) {
        _snprintf(edwd->edwd_Temp, sizeof(edwd->edwd_Temp), "%s, SUBSIZE=%s", edwd->edwd_Temp, z3subsize[cd->cd_Rom.er_Flags & 0x0f]);
    }

    MySetContents(edwd->edwd_Texts[13], " $%02lx ", cd->cd_Rom.er_Type);
    MySetContents(edwd->edwd_Texts[14], MUIX_L "%s", edwd->edwd_Temp);
    set(edwd->edwd_Texts[15], MUIA_FlagsButton_Flags, cd->cd_Rom.er_Flags);
    MySetContents(edwd->edwd_Texts[16], "$%08lx", cd->cd_Rom.er_Reserved03);  // UBYTE
    MySetContents(edwd->edwd_Texts[17], "$%08lx", cd->cd_Rom.er_SerialNumber); // ULONG
    MySetContents(edwd->edwd_Texts[18], "%lU", cd->cd_Rom.er_SerialNumber);
    MySetContents(edwd->edwd_Texts[19], "$%08lx", cd->cd_Rom.er_InitDiagVec); // ULONG

    if (FLAG_IS_SET(cd->cd_Rom.er_Type, ERTF_DIAGVALID)) {
        APTR subgroup, texts[7];
        struct DiagArea *da = (struct DiagArea *)((IPTR)cd->cd_BoardAddr + cd->cd_Rom.er_InitDiagVec);

        subgroup = (Object *)VGroup,
            GroupFrameT(txtExpansionDiagAreaTitle),
            Child, (IPTR)ColGroup(2),
                Child, (IPTR)MyLabel2(txtExpansionDiagArea),
                Child, (IPTR)(texts[0] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaConfig2),
                Child, (IPTR)(texts[1] = (Object *)FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                    MUIA_FlagsButton_Title, (IPTR)txtExpansionDiagAreaConfig,
                    MUIA_FlagsButton_MaskArray, (IPTR)diagConfigs,
                    MUIA_FlagsButton_WindowTitle, (IPTR)txtExpansionDiagAreaConfigTitle,
                End),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaFlags2),
                Child, (IPTR)(texts[2] = (Object *)FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                    MUIA_FlagsButton_Title, (IPTR)txtExpansionDiagAreaFlags,
                    MUIA_FlagsButton_BitArray, 0, // no flags defined yet
                    MUIA_FlagsButton_WindowTitle, (IPTR)txtExpansionDiagAreaFlagsTitle,
                End),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaSize),
                Child, (IPTR)(texts[3] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaDiagPoint),
                Child, (IPTR)(texts[4] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaBootPoint),
                Child, (IPTR)(texts[5] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtExpansionDiagAreaName),
                Child, (IPTR)(texts[6] = MyTextObject6()),
            End,
        End;

        DoMethod(edwd->edwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(edwd->edwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(edwd->edwd_MainGroup, MUIM_Group_ExitChange);

        MySetContents(texts[0], ADDRESS_FORMAT, da);
        set(texts[1], MUIA_FlagsButton_Flags, da->da_Config);
        set(texts[2], MUIA_FlagsButton_Flags, da->da_Flags);
        MySetContents(texts[3], "$%04lx", da->da_Size);
        MySetContents(texts[4], "$%04lx", da->da_Name);
        MySetContents(texts[5], "$%04lx", da->da_DiagPoint);
        MySetContents(texts[6], "$%04lx", da->da_BootPoint);
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtExpansionsDetailTitle, ee->ee_ProdName, edwd->edwd_Title, sizeof(edwd->edwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[20], exitButton, mainGroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Expansions",
        MUIA_Window_ID, MakeID('.','E','X','P'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(mainGroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtExpansionManufacturer2),
                            Child, (IPTR)HGroup,
                                Child, (IPTR)(texts[ 0] = MyTextObject2()),
                                Child, (IPTR)MyLabel2(" = "),
                                Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            End,
                            Child, (IPTR)MyLabel2(txtExpansionProduct2),
                            Child, (IPTR)HGroup,
                                Child, (IPTR)(texts[ 2] = MyTextObject2()),
                                Child, (IPTR)MyLabel2(" = "),
                                Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            End,
                            Child, (IPTR)MyLabel2(txtExpansionClass2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionBoardAddr2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionBoardSize),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionSlotAddr),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionSlotSize),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionDriver),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionNextCD),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionFlags2),
                            Child, (IPTR)(texts[12] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtExpansionFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)cdFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtExpansionFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtExpansionROMType),
                            Child, (IPTR)HGroup,
                                Child, (IPTR)(texts[13] = MyTextObject2()),
                                Child, (IPTR)MyLabel2(" = "),
                                Child, (IPTR)(texts[14] = MyTextObject6()),
                            End,
                            Child, (IPTR)MyLabel2(txtExpansionROMFlags2),
                            Child, (IPTR)(texts[15] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtExpansionROMFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)erFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtExpansionROMFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtExpansionROMReserved),
                            Child, (IPTR)(texts[16] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtExpansionROMSerialNumber),
                            Child, (IPTR)HGroup,
                                Child, (IPTR)(texts[17] = MyTextObject6()),
                                Child, (IPTR)MyLabel2(" = "),
                                Child, (IPTR)(texts[18] = MyTextObject6()),
                            End,
                            Child, (IPTR)MyLabel2(txtExpansionROMInitDiagVec),
                            Child, (IPTR)(texts[19] = MyTextObject6()),
                        End,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ExpansionsDetailWinData *edwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, edwd->edwd_Texts, sizeof(edwd->edwd_Texts));
        edwd->edwd_MainGroup = mainGroup;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,     MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,        MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton, MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct ExpansionsDetailWinData *edwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_ExpansionsDetailWin_Expansion:
                edwd->edwd_Expansion = (struct ExpansionEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(ExpansionsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeExpansionsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ExpansionsDetailWinData), ENTRY(ExpansionsDetailWinDispatcher));
}


