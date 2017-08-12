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

struct MountsDetailWinData {
    TEXT mdwd_Title[WINDOW_TITLE_LENGTH];
    APTR mdwd_Texts[12];
    APTR mdwd_MainGroup;
    struct MountEntry *mdwd_Mount;
};

STATIC CONST_STRPTR dolTypes[] = {
    "DLT_DEVICE",
    "DLT_DIRECTORY",
    "DLT_VOLUME",
    "DLT_LATE",
    "DLT_NONBINDING",
    "<unknown>",
};

STATIC CONST struct LongFlag memFlags[] = {
    { MEMF_PUBLIC,     "MEMF_PUBLIC" },
    { MEMF_CHIP,       "MEMF_CHIP" },
    { MEMF_FAST,       "MEMF_FAST" },
#if defined(__amigaos4__)
    { MEMF_VIRTUAL,    "MEMF_VIRTUAL" },
    { MEMF_EXECUTABLE, "MEMF_EXECUTABLE" },
#else
    { (1 << 3),        "< ??? >" },
    { (1 << 4),        "< ??? >" },
#endif
    { (1 << 5),        "< ??? >" },
    { (1 << 6),        "< ??? >" },
    { (1 << 7),        "< ??? >" },
    { MEMF_LOCAL,      "MEMF_LOCAL" },
    { MEMF_24BITDMA,   "MEMF_24BITDMA" },
    { MEMF_KICK,       "MEMF_KICK" },
#if defined(__amigaos4__)
    { MEMF_PRIVATE,    "MEMF_PRIVATE" },
    { MEMF_SHARED,     "MEMF_SHARED" },
#elif defined(__MORPHOS__)
    { (1 << 11),       "MEMF_SWAP" },
    { (1 << 12),       "MEMF_31BIT" },
#else
    { (1 << 11),       "< ??? >" },
    { (1 << 12),       "< ??? >" },
#endif
    { (1 << 13),       "< ??? >" },
    { (1 << 14),       "< ??? >" },
    { (1 << 15),       "< ??? >" },
    { MEMF_CLEAR,      "MEMF_CLEAR" },
    { MEMF_LARGEST,    "MEMF_LARGEST" },
    { MEMF_REVERSE,    "MEMF_REVERSE" },
    { MEMF_TOTAL,      "MEMF_TOTAL" },
#if defined(__amigaos4__)
    { MEMF_HWALIGNED,  "MEMF_HWALIGNED" },
    { MEMF_DELAYED,    "MEMF_DELAYED" },
#else
    { (1 << 20),       "< ??? >" },
    { (1 << 21),       "< ??? >" },
#endif
    { (1 << 22),       "< ??? >" },
    { (1 << 23),       "< ??? >" },
    { (1 << 24),       "< ??? >" },
    { (1 << 25),       "< ??? >" },
    { (1 << 26),       "< ??? >" },
    { (1 << 27),       "< ??? >" },
    { (1 << 28),       "< ??? >" },
    { (1 << 29),       "< ??? >" },
    { (1 << 30),       "< ??? >" },
#if defined(__amigaos4__)
    { MEMF_NO_EXPUNGE, "MEMF_NO_EXPUNGE" },
#else
    { (1 << 31),       "< ??? >" },
#endif
    { MEMF_ANY,        "MEMF_ANY" },
    { 0,               NULL }
};

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct MountsDetailWinData *mdwd = INST_DATA(cl, obj);
    struct MountEntry *me = mdwd->mdwd_Mount;
    struct DosList *dol = &me->me_dol;

    NoReqOn();

    MySetContents(mdwd->mdwd_Texts[ 0], "%s", me->me_Name);
    MySetContents(mdwd->mdwd_Texts[ 1], ADDRESS_FORMAT, me->me_Device);
    MySetContents(mdwd->mdwd_Texts[ 2], ADDRESS_FORMAT, BADDR(dol->dol_Next));
    MySetContents(mdwd->mdwd_Texts[ 3], "%s", dolTypes[MIN(dol->dol_Type, DLT_NONBINDING + 1)]);
    set(mdwd->mdwd_Texts[ 4], MUIA_TaskButton_Task, dol->dol_Task);
    MySetContents(mdwd->mdwd_Texts[ 5], ADDRESS_FORMAT, BADDR(dol->dol_Lock));
    MySetContents(mdwd->mdwd_Texts[ 6], "%s", me->me_Handler);
    MySetContents(mdwd->mdwd_Texts[ 7], "%lD", dol->dol_misc.dol_handler.dol_StackSize);
    MySetContents(mdwd->mdwd_Texts[ 8], "%lD", dol->dol_misc.dol_handler.dol_Priority);
    set(mdwd->mdwd_Texts[ 9], MUIA_DisassemblerButton_Address, BADDR(dol->dol_misc.dol_handler.dol_Startup));

    MySetContents(mdwd->mdwd_Texts[10], ADDRESS_FORMAT, BADDR(dol->dol_misc.dol_handler.dol_SegList));
    MySetContents(mdwd->mdwd_Texts[11], ADDRESS_FORMAT, BADDR(dol->dol_misc.dol_handler.dol_GlobVec));

#if defined(__MORPHOS__)
    if (me->me_ValidFSSM) {
#else
    if (IsFileSystem(me->me_Name) && me->me_ValidFSSM) {
#endif
        APTR subgroup, texts[26];
        STRPTR state = NULL, type = NULL;
        BPTR lock;
        struct DosEnvec *de = &me->me_fssmEnviron;
    #if defined(USE_NATIVE_64BIT_MATH)
        uint64 size, rem;
    #else
        bigint size, cmp, div, quot, rem;
    #endif
        STRPTR sizetext;

        subgroup = (Object *)VGroup,
            GroupFrameT(txtFSSMEnviron),
            Child, (IPTR)ColGroup(2),
                Child, (IPTR)MyLabel2(txtFSSMDevice),
                Child, (IPTR)(texts[ 0] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtFSSMDiskState),
                Child, (IPTR)(texts[ 1] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtFSSMDiskType),
                Child, (IPTR)(texts[ 2] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtFSSMFlags),
                Child, (IPTR)(texts[ 3] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtFSSMCapacity),
                Child, (IPTR)(texts[ 4] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtFSSMEnviron2),
                Child, (IPTR)(texts[ 5] = MyTextObject6()),
            End,
            Child, (IPTR)VGroup,
                GroupFrameT(txtFSSMEnviron),
                Child, (IPTR)ColGroup(2),
                    Child, (IPTR)MyLabel2(txtEnvecTableSize),
                    Child, (IPTR)(texts[ 6] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecSizeBlock),
                    Child, (IPTR)(texts[ 7] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecSecOrg),
                    Child, (IPTR)(texts[ 8] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecSurfaces),
                    Child, (IPTR)(texts[ 9] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecSectorPerBlock),
                    Child, (IPTR)(texts[10] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecBlocksPerTrack),
                    Child, (IPTR)(texts[11] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecReserved),
                    Child, (IPTR)(texts[12] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecPreAlloc),
                    Child, (IPTR)(texts[13] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecInterleave),
                    Child, (IPTR)(texts[14] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecLowCyl),
                    Child, (IPTR)(texts[15] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecHighCyl),
                    Child, (IPTR)(texts[16] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecNumBuffers),
                    Child, (IPTR)(texts[17] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecBufMemType2),
                    Child, (IPTR)(texts[18] = (Object *)FlagsButtonObject,
                        MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                        MUIA_FlagsButton_Title, (IPTR)txtEnvecBufMemType,
                        MUIA_FlagsButton_BitArray, (IPTR)memFlags,
                        MUIA_FlagsButton_WindowTitle, (IPTR)txtMemoryFlagsTitle,
                    End),
                    Child, (IPTR)MyLabel2(txtEnvecMaxTransfer),
                    Child, (IPTR)(texts[19] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecMask),
                    Child, (IPTR)(texts[20] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecBootPri),
                    Child, (IPTR)(texts[21] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecDosType),
                    Child, (IPTR)(texts[22] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecBaud),
                    Child, (IPTR)(texts[23] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtEnvecControl),
                    Child, (IPTR)(texts[24] = (Object *)DisassemblerButtonObject,
                        MUIA_DisassemblerButton_ForceHexDump, TRUE,
                    End),
                    Child, (IPTR)MyLabel2(txtEnvecBootBlocks),
                    Child, (IPTR)(texts[25] = MyTextObject6()),
                End,
            End,
        End;

        DoMethod(mdwd->mdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(mdwd->mdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(mdwd->mdwd_MainGroup, MUIM_Group_ExitChange);

        if ((lock = Lock(me->me_Name, SHARED_LOCK)) != ZERO) {
            struct InfoData *id;

            if ((id = tbAllocVecPooled(globalPool, sizeof(*id))) != NULL) {
                if (Info(lock, id)) {
                    state = GetDiskState(id->id_DiskState);
                    type = GetDiskType(id->id_DiskType);
                }

                tbFreeVecPooled(globalPool, id);
            }

            UnLock(lock);
        }

        if (state == NULL || type == NULL) {
            struct MsgPort *mp;

            if ((mp = CreateMsgPort()) != NULL) {
                struct IOStdReq *io;

                if ((io = (struct IOStdReq *)CreateIORequest(mp, sizeof(struct IOStdReq))) != NULL) {
                    if (OpenDevice(me->me_DeviceName, me->me_fssmUnit, (struct IORequest *)io, me->me_fssmFlags) == 0) {
                        io->io_Command = TD_CHANGESTATE;
                        DoIO((struct IORequest *)io);
                        if (io->io_Error == 0) {
                            if (io->io_Actual) {
                                state = txtNoDisk;
                            } else {
                                state = txtUnreadable;
                            }
                        } else {
                            state = txtNoDisk;
                        }

                        CloseDevice((struct IORequest *)io);
                    } else {
                       state = txtNoDisk;
                    }

                    DeleteIORequest((struct IORequest *)io);
                }

                DeleteMsgPort(mp);
            }

            type = (STRPTR)"---";
        }

    #if defined(USE_NATIVE_64BIT_MATH)
        size = 0;
    #else
        size = i64_set(0);
        rem = i64_set(0);
    #endif

        if (de->de_Surfaces) {
        #if defined(USE_NATIVE_64BIT_MATH)
            uint64 sizeBlock, surfaces, blocksPerTrack, cylinders;

            sizeBlock = de->de_SizeBlock << 2;
            surfaces = de->de_Surfaces;
            blocksPerTrack = de->de_BlocksPerTrack;
            cylinders = de->de_HighCyl - de->de_LowCyl + 1;
            size = sizeBlock * surfaces * blocksPerTrack * cylinders;
        #else
            bigint sizeBlock, surfaces, blocksPerTrack, cylinders;

            sizeBlock = i64_set(de->de_SizeBlock << 2);
            surfaces = i64_set(de->de_Surfaces);
            blocksPerTrack = i64_set(de->de_BlocksPerTrack);
            cylinders = i64_set(de->de_HighCyl - de->de_LowCyl + 1);
            size = i64_mul(sizeBlock, i64_mul(surfaces, i64_mul(blocksPerTrack, cylinders)));
        #endif
        }

    #if defined(USE_NATIVE_64BIT_MATH)
        if (size > 1024ULL * 1024ULL * 10000ULL) {
            sizetext = txtMountCapacityGigaBytes;
            size >>= 26;
        } else if (size > 1024ULL * 10000ULL) {
            sizetext = txtMountCapacityMegaBytes;
            size >>= 16;
        } else if (size > 10000ULL) {
            sizetext = txtMountCapacityKiloBytes;
            size >>= 6;
        } else {
            sizetext = txtMountCapacityBytes;
        }

        rem = size % 16;
        rem = (rem * 10) >> 4;
        size >>= 4;
    #else
    #if 1
        /* Oh yeah, 1024 * 1024 * 10000 doesn't quite fit 32bit... - Piru */
        cmp.hi = 0x00000002; cmp.lo = 0x71000000;
    #else
        cmp = i64_set(1024 * 1024 * 10000);
    #endif
        if (i64_cmp(size, cmp) == I64_GREATER) {
            sizetext = txtMountCapacityGigaBytes;
            size = i64_urshift(size, 26);
        } else {
            cmp = i64_set(1024 * 10000);
            if (i64_cmp(size, cmp) == I64_GREATER) {
                sizetext = txtMountCapacityMegaBytes;
                size = i64_urshift(size, 16);
            } else {
                cmp = i64_set(10000);
                if (i64_cmp(size, cmp) == I64_GREATER) {
                    sizetext = txtMountCapacityKiloBytes;
                    size = i64_urshift(size, 6);
                } else {
                    sizetext = txtMountCapacityBytes;
                }
            }
        }

        div = i64_set(16);
        i64_udiv(size, div, &quot, &rem);

        div = i64_set(10);
        rem = i64_urshift(i64_mul(rem, div), 4);

        size = i64_urshift(size, 4);
    #endif

        MySetContents(texts[ 0], "%s/%ld", me->me_DeviceName, me->me_fssmUnit);
        MySetContents(texts[ 1], "%s", state);
        MySetContents(texts[ 2], "%s", type);
        MySetContents(texts[ 3], "$%08lx", me->me_fssmFlags);
    #if defined(USE_NATIVE_64BIT_MATH)
        MySetContents(texts[ 4], "%lld.%lld%s", size, rem, sizetext);
    #else
        MySetContents(texts[ 4], "%ld.%ld%s", size.lo, rem.lo, sizetext);
    #endif
        MySetContents(texts[ 5], ADDRESS_FORMAT, de);
        MySetContents(texts[ 6], "%lD", de->de_TableSize);
        MySetContents(texts[ 7], "%lD", de->de_SizeBlock);
        MySetContents(texts[ 8], "%lD", de->de_SecOrg);
        MySetContents(texts[ 9], "%lD", de->de_Surfaces);
        MySetContents(texts[10], "%lD", de->de_SectorPerBlock);
        MySetContents(texts[11], "%lD", de->de_BlocksPerTrack);
        MySetContents(texts[12], "%lD", de->de_Reserved);
        MySetContents(texts[13], "%lD", de->de_PreAlloc);
        MySetContents(texts[14], "%lD", de->de_Interleave);
        MySetContents(texts[15], "%lD", de->de_LowCyl);
        MySetContents(texts[16], "%lD", de->de_HighCyl);
        MySetContents(texts[17], "%lD", de->de_NumBuffers);
        set(texts[18], MUIA_FlagsButton_Flags, de->de_BufMemType);
        MySetContents(texts[19], "$%08lx", de->de_MaxTransfer);
        if (de->de_TableSize >= DE_MASK) {
            MySetContents(texts[20], "$%08lx", de->de_Mask);
        } else {
            MySetContents(texts[20], "%s", "---");
        }
        if (de->de_TableSize >= DE_BOOTPRI) {
            MySetContents(texts[21], "%lD", de->de_BootPri);
        } else {
            MySetContents(texts[21], "%s", "---");
        }
        if (de->de_TableSize >= DE_DOSTYPE) {
            MySetContents(texts[22], "$%08lx = %s", de->de_DosType, GetDiskType(de->de_DosType));
        } else {
            MySetContents(texts[22], "%s", "---");
        }
        if (de->de_TableSize >= DE_BAUD) {
            MySetContents(texts[23], "%lD", de->de_Baud);
        } else {
            MySetContents(texts[23], "%s", "---");
        }
        set(texts[24], MUIA_DisassemblerButton_Address, (de->de_TableSize >= DE_CONTROL) ? BADDR(de->de_Control) : NULL);
        if (de->de_TableSize >= DE_BOOTBLOCKS) {
            MySetContents(texts[25], "%lD", de->de_BootBlocks);
        } else {
            MySetContents(texts[25], "%s", "---");
        }
    }

    NoReqOff();

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtMountsDetailTitle, me->me_Name, mdwd->mdwd_Title, sizeof(mdwd->mdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[12], exitButton, maingroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Mount",
        MUIA_Window_ID, MakeID('.','M','O','U'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(maingroup = (Object *)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountNext),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountType),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountTask),
                            Child, (IPTR)(texts[ 4] = (Object *)TaskButtonObject,
                                MUIA_TaskButton_IsProcessPointer, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtMountLock),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountHandler2),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountStackSize),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountPriority),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountStartup),
                            Child, (IPTR)(texts[ 9] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtMountSegList),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMountGlobVec),
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
        struct MountsDetailWinData *mdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, mdwd->mdwd_Texts, sizeof(mdwd->mdwd_Texts));
        mdwd->mdwd_MainGroup = maingroup;

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
    struct MountsDetailWinData *mdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_MountsDetailWin_Mount:
                mdwd->mdwd_Mount = (struct MountEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(MountsDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMountsDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MountsDetailWinData), ENTRY(MountsDetailWinDispatcher));
}

