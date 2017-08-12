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

struct ResourcesDetailWinData {
    TEXT rdwd_Title[WINDOW_TITLE_LENGTH];
    APTR rdwd_Texts[12];
    struct ResourceEntry *rdwd_Resource;
    APTR rdwd_MainGroup;
    TEXT rdwd_DiskTaskName[TMP_STRING_LENGTH];
};

STATIC CONST struct LongFlag resourceFlags[] = {
    { LIBF_SUMMING,   "LIBF_SUMMING" },
    { LIBF_CHANGED,   "LIBF_CHANGED" },
    { LIBF_SUMUSED,   "LIBF_SUMUSED" },
    { LIBF_DELEXP,    "LIBF_DELEXP" },

#if defined(__MORPHOS__)
    { LIBF_RAMLIB,    "LIBF_RAMLIB" },
    { LIBF_QUERYINFO, "LIBF_QUERYINFO" },
#elif defined(__amigaos4__)
    { LIBF_EXP0CNT,   "LIBF_EXP0CNT" },
    { 0x20,           "< ??? >" },
#else
    { 0x10,           "< ??? >" },
    { 0x20,           "< ??? >" },
#endif
    { 0x40,           "< ??? >" },
    { 0x80,           "< ??? >" },
    { 0,              NULL },
};

STATIC CONST struct LongFlag diskResFlags[] = {
    { DRF_ALLOC0, "DRF_ALLOC0" },
    { DRF_ALLOC1, "DRF_ALLOC1" },
    { DRF_ALLOC2, "DRF_ALLOC2" },
    { DRF_ALLOC3, "DRF_ALLOC3" },
    { 0x10,       "< ??? >"    },
    { 0x20,       "< ??? >"    },
    { 0x40,       "< ??? >"    },
    { DRF_ACTIVE, "DRF_ACTIVE" },
    { 0,          NULL },
};

HOOKPROTONHNO(fselist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct FileSystemEntry));
}
MakeStaticHook(fselist_con2hook, fselist_con2func);

HOOKPROTONHNO(fselist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(fselist_des2hook, fselist_des2func);

HOOKPROTONHNO(fselist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct FileSystemEntry *fse = (struct FileSystemEntry *)msg->entry;

    if (fse) {
        msg->strings[0] = fse->fse_FileSystem;
        msg->strings[1] = fse->fse_DosType;
        msg->strings[2] = fse->fse_Version;
        msg->strings[3] = fse->fse_PatchFlags;
        msg->strings[4] = fse->fse_Creator;
    } else {
        msg->strings[0] = txtResourceFileSystem;
        msg->strings[1] = txtResourceFileSystemDosType;
        msg->strings[2] = txtResourceFileSystemVersion;
        msg->strings[3] = txtResourceFileSystemPatchFlags;
        msg->strings[4] = txtResourceFileSystemCreator;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(fselist_dsp2hook, fselist_dsp2func);

STATIC LONG fselist_cmp2colfunc( struct FileSystemEntry *fse1,
                                 struct FileSystemEntry *fse2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return stricmp(fse1->fse_FileSystem, fse2->fse_FileSystem);
        case 1: return HexCompare(fse1->fse_DosType, fse2->fse_DosType);
        case 2: return VersionCompare(fse1->fse_Version, fse2->fse_Version);
        case 3: return HexCompare(fse1->fse_PatchFlags, fse2->fse_PatchFlags);
        case 4: return stricmp(fse1->fse_Creator, fse2->fse_Creator);
    }
}

HOOKPROTONHNO(fselist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct FileSystemEntry *fse1, *fse2;
    ULONG col1, col2;

    fse1 = (struct FileSystemEntry *)msg->entry1;
    fse2 = (struct FileSystemEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = fselist_cmp2colfunc(fse2, fse1, col1);
    } else {
        cmp = fselist_cmp2colfunc(fse1, fse2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = fselist_cmp2colfunc(fse2, fse1, col2);
    } else {
        cmp = fselist_cmp2colfunc(fse1, fse2, col2);
    }

    return cmp;
}
MakeStaticHook(fselist_cmp2hook, fselist_cmp2func);

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct ResourcesDetailWinData *rdwd = INST_DATA(cl, obj);
    struct ResourceEntry *re = rdwd->rdwd_Resource;
    struct Library *lib = re->re_Addr;

    MySetContentsHealed(rdwd->rdwd_Texts[ 0], "%s", lib->lib_Node.ln_Name);
    MySetContents(rdwd->rdwd_Texts[ 1], ADDRESS_FORMAT, lib);
    MySetContents(rdwd->rdwd_Texts[ 2], "---");
    MySetContents(rdwd->rdwd_Texts[ 3], "%s", GetNodeType(lib->lib_Node.ln_Type));
    MySetContents(rdwd->rdwd_Texts[ 4], "%ld", lib->lib_Node.ln_Pri);
    MySetContents(rdwd->rdwd_Texts[ 5], "---");
    MySetContents(rdwd->rdwd_Texts[ 6], "---");
    set(rdwd->rdwd_Texts[7], MUIA_Disabled, TRUE);
    MySetContents(rdwd->rdwd_Texts[ 8], "---");
    MySetContents(rdwd->rdwd_Texts[ 9], "---");
    MySetContents(rdwd->rdwd_Texts[10], "---");
    MySetContents(rdwd->rdwd_Texts[11], "---");

    if (strcmp(re->re_Name, FSRNAME) == 0) {
        APTR subgroup, creator, fsrList;
        struct FileSysResource *fsr;
        struct FileSysEntry *fse;
        struct MinList tmplist;
        struct FileSystemEntry *fe;

        fsr = (struct FileSysResource *)lib;

        subgroup = (Object *)VGroup,
            GroupFrameT(txtResourceFileSystemSpecific),
            Child, (IPTR)HGroup,
                Child, (IPTR)MyLabel2(txtResourceFileSystemResCreator),
                Child, (IPTR)(creator = MyTextObject6()),
            End,
            Child, (IPTR)VGroup,
                GroupFrameT(txtResourceFileSystemResEntries),
                Child, (IPTR)MyNListviewObject(&fsrList, MakeID('F','S','L','V'), "BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",BAR P=" MUIX_C ",", &fselist_con2hook, &fselist_des2hook, &fselist_dsp2hook, &fselist_cmp2hook, FALSE),
            End,
        End;

        DoMethod(rdwd->rdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(rdwd->rdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(rdwd->rdwd_MainGroup, MUIM_Group_ExitChange);

        MySetContentsHealed(creator, "%s", fsr->fsr_Creator);

        NewList((struct List *)&tmplist);

        ApplicationSleep(TRUE);

        set(fsrList, MUIA_NList_Quiet, TRUE);
        DoMethod(fsrList, MUIM_NList_Clear);

        Forbid();

        ITERATE_LIST(&fsr->fsr_FileSysEntries, struct FileSysEntry *, fse) {
            if ((fe = AllocVec(sizeof(struct FileSystemEntry), MEMF_CLEAR)) != NULL) {
                stccpy(fe->fse_FileSystem, GetDiskType(fse->fse_DosType), sizeof(fe->fse_FileSystem));
                _snprintf(fe->fse_DosType, sizeof(fe->fse_DosType), "$%08lx", fse->fse_DosType);
                _snprintf(fe->fse_Version, sizeof(fe->fse_Version), "%ld.%ld", fse->fse_Version >> 16, fse->fse_Version & 0xffff);
                _snprintf(fe->fse_PatchFlags, sizeof(fe->fse_PatchFlags), "$%08lx", fse->fse_PatchFlags);
                stccpy(fe->fse_Creator, nonetest(fse->fse_Node.ln_Name), sizeof(fe->fse_Creator));
                healstring(fe->fse_Creator);

                AddTail((struct List *)&tmplist, (struct Node *)fe);
            }
        }

        Permit();

        ITERATE_LIST(&tmplist, struct FileSystemEntry *, fe) {
            InsertBottomEntry(fsrList, fe);
        }
        FreeLinkedList((struct List *)&tmplist);

        set(fsrList, MUIA_NList_Quiet, FALSE);

        ApplicationSleep(FALSE);

        DoMethod(fsrList, MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
    } else if (TestResource(lib)) {
        LONG cnt, total;

        // we are now dealing with an ordinary resource, except keymap.resource and FileSystem.resource
        MySetContentsHealed(rdwd->rdwd_Texts[ 2], "%s", nonetest(lib->lib_IdString));
        MySetContents(rdwd->rdwd_Texts[ 5], "%ld.%ld", lib->lib_Version, lib->lib_Revision);
        MySetContents(rdwd->rdwd_Texts[ 6], "%lD", lib->lib_OpenCnt);
        SetAttrs(rdwd->rdwd_Texts[ 7], MUIA_Disabled, FALSE,
                                       MUIA_FlagsButton_Flags, lib->lib_Flags,
                                       TAG_DONE);
        MySetContents(rdwd->rdwd_Texts[ 8], "%lD", lib->lib_NegSize);
        MySetContents(rdwd->rdwd_Texts[ 9], "%lD", lib->lib_PosSize);
        MySetContents(rdwd->rdwd_Texts[10], "$%08lx", lib->lib_Sum);

        GetRamPointerCount(lib, &cnt, &total);
        MySetContents(rdwd->rdwd_Texts[11], txtLibraryRPCFormat, cnt, total);

        if (strcmp(re->re_Name, DISKNAME) == 0) {
            // display disk.resource specific extensions
            APTR subgroup, text;
            struct DiscResource *dr = (struct DiscResource *)lib;

            subgroup = (Object *)VGroup,
                GroupFrameT(txtResourceDiskResSpecific),
                Child, (IPTR)ColGroup(2),
                    Child, (IPTR)MyLabel2(txtResourceDiskResFlags2),
                    Child, (IPTR)FlagsButtonObject,
                        MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                        MUIA_FlagsButton_Flags, dr->dr_Flags,
                        MUIA_FlagsButton_Title, (IPTR)txtResourceDiskResFlags,
                        MUIA_FlagsButton_BitArray, (IPTR)diskResFlags,
                        MUIA_FlagsButton_WindowTitle, (IPTR)txtResourceDiskResFlagsTitle,
                    End,
                    Child, (IPTR)MyLabel2(txtResourceDiskResCurrTask),
                    Child, (IPTR)(text = (Object *)TaskButtonObject,
                        MUIA_TaskButton_Task, (IPTR)dr->dr_CurrTask,
                    End),
                End,
            End;

            DoMethod(rdwd->rdwd_MainGroup, MUIM_Group_InitChange);
            DoMethod(rdwd->rdwd_MainGroup, OM_ADDMEMBER, subgroup);
            DoMethod(rdwd->rdwd_MainGroup, MUIM_Group_ExitChange);
        }
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtResourcesDetailTitle, re->re_Name, rdwd->rdwd_Title, sizeof(rdwd->rdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[12], exitButton, mainGroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Resources",
        MUIA_Window_ID, MakeID('.','R','E','C'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(mainGroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryIdString),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryVersion2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryOpenCount2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryFlags2),
                            Child, (IPTR)(texts[ 7] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtLibraryFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)resourceFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtResourceFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtLibraryNegSize),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryPosSize),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibrarySum),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtLibraryRPC2),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                        End,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct ResourcesDetailWinData *rdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, rdwd->rdwd_Texts, sizeof(rdwd->rdwd_Texts));
        rdwd->rdwd_MainGroup = mainGroup;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct ResourcesDetailWinData *rdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_ResourcesDetailWin_Resource:
                rdwd->rdwd_Resource = (struct ResourceEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(ResourcesDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeResourcesDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct ResourcesDetailWinData), ENTRY(ResourcesDetailWinDispatcher));
}

