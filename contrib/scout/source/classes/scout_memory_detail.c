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

struct MemoryDetailWinData {
    char mdwd_Title[WINDOW_TITLE_LENGTH];
    APTR mdwd_Texts[12];
    APTR mdwd_ChunkListview;
    APTR mdwd_ChunkList;
    struct MemoryEntry *mdwd_Memory;
    APTR mdwd_MemoryPool;
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

HOOKPROTONHNO(memmorelist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct MemoryFreeEntry *mfe = (struct MemoryFreeEntry *)msg->entry;

    if (mfe) {
        msg->strings[0] = mfe->mfe_Lower;
        msg->strings[1] = mfe->mfe_Upper;
        msg->strings[2] = mfe->mfe_Size;
    } else {
        msg->strings[0] = txtMemoryLower3;
        msg->strings[1] = txtMemoryUpper3;
        msg->strings[2] = txtMemorySize;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(memmorelist_dsp2hook, memmorelist_dsp2func);

STATIC SIPTR memmorelist_cmp2colfunc( struct MemoryFreeEntry *mfe1,
                                     struct MemoryFreeEntry *mfe2,
                                     ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(mfe1->mfe_Lower, mfe2->mfe_Lower);
        case 1: return HexCompare(mfe1->mfe_Upper, mfe2->mfe_Upper);
        case 2: return -stricmp(mfe1->mfe_Size, mfe2->mfe_Size);
    }
}

HOOKPROTONHNO(memmorelist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    SIPTR cmp;
    struct MemoryFreeEntry *mfe1, *mfe2;
    ULONG col1, col2;

    mfe1 = (struct MemoryFreeEntry *)msg->entry1;
    mfe2 = (struct MemoryFreeEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = memmorelist_cmp2colfunc(mfe2, mfe1, col1);
    } else {
        cmp = memmorelist_cmp2colfunc(mfe1, mfe2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = memmorelist_cmp2colfunc(mfe2, mfe1, col2);
    } else {
        cmp = memmorelist_cmp2colfunc(mfe1, mfe2, col2);
    }

    return cmp;
}
MakeStaticHook(memmorelist_cmp2hook, memmorelist_cmp2func);

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct MemoryDetailWinData *mdwd = INST_DATA(cl, obj);
    struct MemoryEntry *me = mdwd->mdwd_Memory;
    struct MemHeader *mh = me->me_Header;
    IPTR total, inuse, largest = 0;

    total = (IPTR)mh->mh_Upper - (IPTR)mh->mh_Lower;
    inuse = total - (IPTR)mh->mh_Free;

    MySetContents(mdwd->mdwd_Texts[ 0], "%s", me->me_Name);
    MySetContents(mdwd->mdwd_Texts[ 1], "$%p", mh);
    MySetContents(mdwd->mdwd_Texts[ 2], "%s", GetNodeType(mh->mh_Node.ln_Type));
    MySetContents(mdwd->mdwd_Texts[ 3], "%ld", mh->mh_Node.ln_Pri);
    MySetContents(mdwd->mdwd_Texts[ 4], "$%p", mh->mh_Lower);
    MySetContents(mdwd->mdwd_Texts[ 5], "$%p", mh->mh_Upper);
    MySetContents(mdwd->mdwd_Texts[ 6], "$%p", mh->mh_First);

#if defined(__AROS__)
    if (!IsManagedMem(mh))
        largest = AvailMem(MEMF_LARGEST | mh->mh_Attributes);
    else
    {
        MySetContents(mdwd->mdwd_Texts[ 6], "%s", "N/A");
        if (((struct MemHeaderExt *)mh)->mhe_Avail)
            largest = ((struct MemHeaderExt *)mh)->mhe_Avail((struct MemHeaderExt *)mh, MEMF_LARGEST | mh->mh_Attributes);
    }
#else
    largest = AvailMem(MEMF_LARGEST | mh->mh_Attributes);
#endif

#if defined(__AROS__) && (__WORDSIZE == 64)
    MySetContents(mdwd->mdwd_Texts[ 7], "%15llD", inuse);
    MySetContents(mdwd->mdwd_Texts[ 8], "%15llD", mh->mh_Free);
    MySetContents(mdwd->mdwd_Texts[ 9], "%15llD", total);
    MySetContents(mdwd->mdwd_Texts[10], "%15llD", largest);
#else
    MySetContents(mdwd->mdwd_Texts[ 7], "%12lD", inuse);
    MySetContents(mdwd->mdwd_Texts[ 8], "%12lD", mh->mh_Free);
    MySetContents(mdwd->mdwd_Texts[ 9], "%12lD", total);
    MySetContents(mdwd->mdwd_Texts[10], "%12lD", largest);
#endif

    set(mdwd->mdwd_Texts[11], MUIA_FlagsButton_Flags, mh->mh_Attributes);

    DoMethod(mdwd->mdwd_ChunkList, MUIM_NList_Clear);

    if (mh->mh_First) {
        struct MemChunk *mc;
        struct MemoryFreeEntry *mfel;
        IPTR mccnt;

        mc = mh->mh_First;
        mccnt = 0;
        while (mc) {
            mccnt++;
            mc = mc->mc_Next;
        }
        mccnt += MAX(10, mccnt / 10);

        if ((mfel = tbAllocVecPooled(mdwd->mdwd_MemoryPool, mccnt * sizeof(struct MemoryFreeEntry))) != NULL) {
            struct MemoryFreeEntry *mfe;

#if defined(__AROS__)
            if (!IsManagedMem(mh))
            {
#endif
            set(mdwd->mdwd_ChunkList, MUIA_NList_Quiet, TRUE);

            mc = mh->mh_First;
            mfe = mfel;
            while (mc != NULL && mccnt > 0) {
                _snprintf(mfe->mfe_Lower, sizeof(mfe->mfe_Lower), "$%p", mc);
                _snprintf(mfe->mfe_Upper, sizeof(mfe->mfe_Upper), "$%p", (IPTR)mc + mc->mc_Bytes);
#if defined(__AROS__) && (__WORDSIZE == 64)
                _snprintf(mfe->mfe_Size, sizeof(mfe->mfe_Size), "%15llD", mc->mc_Bytes);
#else
                _snprintf(mfe->mfe_Size, sizeof(mfe->mfe_Size), "%12lD", mc->mc_Bytes);
#endif

                InsertBottomEntry(mdwd->mdwd_ChunkList, mfe);

                mfe++;
                mccnt--;

                mc = mc->mc_Next;
            }

            set(mdwd->mdwd_ChunkList, MUIA_NList_Quiet, FALSE);
#if defined(__AROS__)
        }
        else
        {
            /* we cannot obtain the chunk list for managed memory! */
            set(mdwd->mdwd_ChunkListview, MUIA_Disabled, TRUE);
        }
            
#endif
        }
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtMemoryDetailTitle, me->me_Name, mdwd->mdwd_Title, sizeof(mdwd->mdwd_Title)));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[12], exitButton, chunklistview, chunklist;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Memory",
        MUIA_Window_ID, MakeID('.','M','E','M'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
                            Child, (IPTR)MyLabel2(txtNodeName2),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtAddress2),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodeType2),
                            Child, (IPTR)(texts[ 2] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtNodePri2),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryLower2),
                            Child, (IPTR)(texts[ 4] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryUpper2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryFirst),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryInUse),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryFree),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryTotal),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryLargest),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtMemoryAttr2),
                            Child, (IPTR)(texts[11] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Word,
                                MUIA_FlagsButton_Title, (IPTR)txtMemoryAttr,
                                MUIA_FlagsButton_BitArray, (IPTR)memFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtMemoryFlagsTitle,
                            End),
                        End,
                        Child, (IPTR)VGroup,
                            GroupFrameT(txtMemoryChunks),
                            Child, (IPTR)(chunklistview = MyNListviewObject(&chunklist, MakeID('.','M','L','V'), "BAR,BAR,BAR P=" MUIX_R, NULL, NULL, &memmorelist_dsp2hook, &memmorelist_cmp2hook, FALSE)),
                        End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MemoryDetailWinData *mdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, mdwd->mdwd_Texts, sizeof(mdwd->mdwd_Texts));
        mdwd->mdwd_ChunkListview = chunklistview;
        mdwd->mdwd_ChunkList = chunklist;
        mdwd->mdwd_MemoryPool = tbCreatePool(MEMF_CLEAR, 4096, 4096);

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
    struct MemoryDetailWinData *mdwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(mdwd->mdwd_ChunkList, MUIM_NList_Clear);

    tbDeletePool(mdwd->mdwd_MemoryPool);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct MemoryDetailWinData *mdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_MemoryDetailWin_Memory:
                mdwd->mdwd_Memory = (struct MemoryEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(MemoryDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMemoryDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MemoryDetailWinData), ENTRY(MemoryDetailWinDispatcher));
}

