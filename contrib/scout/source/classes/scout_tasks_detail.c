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

struct TasksDetailWinData {
    TEXT tdwd_Title[WINDOW_TITLE_LENGTH];
    APTR tdwd_Texts[26];
    APTR tdwd_MainGroup;
    struct TaskEntry *tdwd_Task;
};

STATIC CONST struct LongFlag taskFlags[] = {
    { TF_PROCTIME,  "TF_PROCTIME" },
    { 0x02,         "< ??? >"     },
    { 0x04,         "< ??? >"     },
    { TF_PROCTIME,  "TF_PROCTIME" },
    { TF_ETASK,     "TF_ETASK"    },
    { TF_STACKCHK,  "TF_STACKCHK" },
    { TF_EXCEPT,    "TF_EXCEPT"   },
    { TF_SWITCH,    "TF_SWITCH"   },
    { TF_LAUNCH,    "TF_LAUNCH"   },
    { 0,            NULL }
};

#if defined(PRF_FREEARGS_OBSOLETE)
    #define PRF_FREEARGS            PRF_FREEARGS_OBSOLETE
#endif

STATIC CONST struct LongFlag processFlags[] = {
    { PRF_FREESEGLIST,  "PRF_FREESEGLIST"  },
    { PRF_FREECURRDIR,  "PRF_FREECURRDIR"  },
    { PRF_FREECLI,      "PRF_FREECLI"      },
    { PRF_CLOSEINPUT,   "PRF_CLOSEINPUT"   },
    { PRF_CLOSEOUTPUT,  "PRF_CLOSEOUTPUT"  },
    { PRF_FREEARGS,     "PRF_FREEARGS"     },
#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    { PRF_CLOSEERROR,   "PRF_CLOSEERROR"   },
#else
    { 0x00000040,       "< ??? >"          },
#endif
#if defined(__amigaos4__)
    { PRF_LOCKSTACK,    "PRF_LOCKSTACK"    },
#else
    { 0x00000080,       "< ??? >"          },
#endif
    { 0x00000100,       "< ??? >"          },
    { 0x00000200,       "< ??? >"          },
    { 0x00000400,       "< ??? >"          },
    { 0x00000800,       "< ??? >"          },
    { 0x00001000,       "< ??? >"          },
    { 0x00002000,       "< ??? >"          },
    { 0x00004000,       "< ??? >"          },
    { 0x00008000,       "< ??? >"          },
    { 0x00010000,       "< ??? >"          },
    { 0x00020000,       "< ??? >"          },
    { 0x00040000,       "< ??? >"          },
    { 0x00080000,       "< ??? >"          },
    { 0x00100000,       "< ??? >"          },
    { 0x00200000,       "< ??? >"          },
    { 0x00400000,       "< ??? >"          },
    { 0x00800000,       "< ??? >"          },
#if defined(__amigaos4__)
    { PRF_CHILDPROCESS, "PRF_CHILDPROCESS" },
    { PRF_HASCHILDREN,  "PRF_HASCHILDREN"  },
    { PRF_HASDLNOTIFY,  "PRF_HASDLNOTIFY"  },
    { PRF_HASPLNOTIFY,  "PRF_HASPLNOTIFY"  },
#else
    { 0x01000000,       "< ??? >"          },
    { 0x02000000,       "< ??? >"          },
    { 0x03000000,       "< ??? >"          },
    { 0x04000000,       "< ??? >"          },
#endif
    { 0x10000000,       "< ??? >"          },
    { 0x20000000,       "< ??? >"          },
#if defined(__amigaos4__)
    { PRF_SHELLPROCESS, "PRF_SHELLPROCESS" },
    { PRF_RESERVED,     "PRF_RESERVED"     },
#else
    { 0x40000000,       "< ??? >"          },
    { 0x80000000,       "< ??? >"          },
#endif
    { 0,                NULL }
};

#if !defined(__MORPHOS__) && !defined(__AROS__)
struct ETask
{
    struct Message et_Message;
    struct Task *et_Parent;
    ULONG et_UniqueID;
    struct MinList et_Children;
    UWORD et_TrapAlloc;    /* real TrapAlloc! */
    UWORD et_TrapAble;     /* real TrapAble! */
    ULONG et_Result1;
    void *et_Result2;
    struct MsgPort et_MsgPort;
};
#endif

#if defined(__amigaos4__)
// some structure members have changed their names :)
#define pr_SegList                      pr_SegArray
#define pr_GlobVec                      pr_gv
#define pr_StackBase                    pr_sb
#define pr_ReturnAddr                   pr_ra
#endif

HOOKPROTONHNO(seglist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct SegListEntry));
}
MakeStaticHook(seglist_con2hook, seglist_con2func);

HOOKPROTONHNO(seglist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(seglist_des2hook, seglist_des2func);

HOOKPROTONHNO(seglist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct SegListEntry *sle = (struct SegListEntry *)msg->entry;

    if (sle) {
        msg->strings[0] = sle->sle_Lower;
        msg->strings[1] = sle->sle_Upper;
        msg->strings[2] = sle->sle_Size;
    } else {
        msg->strings[0] = txtSegListLower;
        msg->strings[1] = txtSegListUpper;
        msg->strings[2] = txtSegListSize;
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
    }
}
MakeStaticHook(seglist_dsp2hook, seglist_dsp2func);

STATIC LONG seglist_cmp2colfunc( struct SegListEntry *sle1,
                                 struct SegListEntry *sle2,
                                 ULONG column )
{
    switch (column) {
        default:
        case 0: return stricmp(sle1->sle_Lower, sle2->sle_Lower);
        case 1: return stricmp(sle1->sle_Upper, sle2->sle_Upper);
        case 2: return stricmp(sle1->sle_Size, sle2->sle_Size);
    }
}

HOOKPROTONHNO(seglist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct SegListEntry *sle1, *sle2;
    ULONG col1, col2;

    sle1 = (struct SegListEntry *)msg->entry1;
    sle2 = (struct SegListEntry *)msg->entry2;
    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = seglist_cmp2colfunc(sle2, sle1, col1);
    } else {
        cmp = seglist_cmp2colfunc(sle1, sle2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = seglist_cmp2colfunc(sle2, sle1, col2);
    } else {
        cmp = seglist_cmp2colfunc(sle1, sle2, col2);
    }

    return cmp;
}
MakeStaticHook(seglist_cmp2hook, seglist_cmp2func);

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
    struct TasksDetailWinData *tdwd = INST_DATA(cl, obj);
    struct TaskEntry *te = tdwd->tdwd_Task;
    struct Task *tc = te->te_Addr;
#if !defined(__amigaos4__)
    UWORD trapAlloc, trapAble;
#endif

    MySetContents(tdwd->tdwd_Texts[ 0], "%s", te->te_Name);
    MySetContents(tdwd->tdwd_Texts[ 1], ADDRESS_FORMAT, tc);
    MySetContents(tdwd->tdwd_Texts[ 2], "%s", GetNodeType(tc->tc_Node.ln_Type));
    MySetContents(tdwd->tdwd_Texts[ 3], "%ld", tc->tc_Node.ln_Pri);
    set(tdwd->tdwd_Texts[ 4], MUIA_FlagsButton_Flags, tc->tc_Flags);
    MySetContents(tdwd->tdwd_Texts[ 5], "%s", GetTaskState(tc->tc_State, tc->tc_SigWait));
    MySetContents(tdwd->tdwd_Texts[ 6], "%lD", tc->tc_IDNestCnt);
    MySetContents(tdwd->tdwd_Texts[ 7], "%lD", tc->tc_TDNestCnt);
    MySetContents(tdwd->tdwd_Texts[ 8], "$%08lx", tc->tc_SigAlloc); // ULONG
    MySetContents(tdwd->tdwd_Texts[ 9], "$%08lx", tc->tc_SigWait); // ULONG
    MySetContents(tdwd->tdwd_Texts[10], "$%08lx", tc->tc_SigRecvd); // ULONG
    MySetContents(tdwd->tdwd_Texts[11], "$%08lx", tc->tc_SigExcept); // ULONG

#if defined(__MORPHOS__)
    trapAlloc = tc->tc_ETask->TrapAlloc;
    trapAble = tc->tc_ETask->TrapAble;
#elif defined(__AROS__)
    trapAlloc = GetTrapAlloc(tc);
    trapAble = GetTrapAble(tc);
#elif !defined(__amigaos4__)
    if (FLAG_IS_SET(tc->tc_Flags, TF_ETASK)) {
        struct ETask *et;

        et = (struct ETask *)(tc->tc_TrapAlloc << 16 | tc->tc_TrapAble);
        trapAlloc = et->et_TrapAlloc;
        trapAble = et->et_TrapAble;
    } else {
        trapAlloc = tc->tc_TrapAlloc;
        trapAble = tc->tc_TrapAble;
    }
#endif

#if defined(__amigaos4__)
    set(tdwd->tdwd_Texts[12], MUIA_DisassemblerButton_Address, tc->tc_ETask);
#else
    MySetContents(tdwd->tdwd_Texts[12], "$%04lx", trapAlloc);
    MySetContents(tdwd->tdwd_Texts[13], "$%04lx", trapAble);
#endif
    set(tdwd->tdwd_Texts[14], MUIA_DisassemblerButton_Address, tc->tc_ExceptData);
    set(tdwd->tdwd_Texts[15], MUIA_DisassemblerButton_Address, tc->tc_ExceptCode);
    set(tdwd->tdwd_Texts[16], MUIA_DisassemblerButton_Address, tc->tc_TrapData);
    set(tdwd->tdwd_Texts[17], MUIA_DisassemblerButton_Address, tc->tc_TrapCode);
    MySetContents(tdwd->tdwd_Texts[18], ADDRESS_FORMAT, tc->tc_SPReg);
    MySetContents(tdwd->tdwd_Texts[19], ADDRESS_FORMAT, tc->tc_SPLower);
    MySetContents(tdwd->tdwd_Texts[20], ADDRESS_FORMAT, tc->tc_SPUpper);
    MySetContents(tdwd->tdwd_Texts[21], "%lD", (ULONG)((IPTR)tc->tc_SPUpper - (IPTR)tc->tc_SPLower));
    set(tdwd->tdwd_Texts[22], MUIA_DisassemblerButton_Address, tc->tc_Switch);
    set(tdwd->tdwd_Texts[23], MUIA_DisassemblerButton_Address, tc->tc_Launch);
    MySetContents(tdwd->tdwd_Texts[24], ADDRESS_FORMAT, tc->tc_MemEntry);
    set(tdwd->tdwd_Texts[25], MUIA_DisassemblerButton_Address, tc->tc_UserData);

#if defined(__MORPHOS__)
    {
        APTR subgroup, texts[9];

        subgroup = VGroup,
            GroupFrameT(txtETask),
            Child, (IPTR)ColGroup(2),
                Child, (IPTR)MyLabel2(txtETaskPPCStackSize),
                Child, (IPTR)texts[ 0] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskPPCStackUsed),
                Child, (IPTR)texts[ 1] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskTrapMsgPort),
                Child, (IPTR)texts[ 2] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskStartupMsg),
                Child, (IPTR)texts[ 3] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskMsgPort),
                Child, (IPTR)texts[ 4] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskPoolPtr),
                Child, (IPTR)texts[ 5] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskPoolMemFlags),
                Child, (IPTR)texts[ 6] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskPoolPuddleSize),
                Child, (IPTR)texts[ 7] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtETaskPoolThreshSize),
                Child, (IPTR)texts[ 8] = MyTextObject6(),
            End,
        End;

        if (subgroup)
        {
            ULONG stacksize, stackused, trapmsgport, startupmsg, taskmsgport;
            ULONG poolptr, poolmemflags, puddlesize, threshsize;

            NewGetTaskAttrsA(tc, &stacksize, sizeof(ULONG), TASKINFOTYPE_STACKSIZE, NULL);
            NewGetTaskAttrsA(tc, &stackused, sizeof(ULONG), TASKINFOTYPE_USEDSTACKSIZE, NULL);
            NewGetTaskAttrsA(tc, &trapmsgport, sizeof(ULONG), TASKINFOTYPE_TRAPMSGPORT, NULL);
            NewGetTaskAttrsA(tc, &startupmsg, sizeof(ULONG), TASKINFOTYPE_STARTUPMSG, NULL);
            NewGetTaskAttrsA(tc, &taskmsgport, sizeof(ULONG), TASKINFOTYPE_TASKMSGPORT, NULL);
            NewGetTaskAttrsA(tc, &poolptr, sizeof(ULONG), TASKINFOTYPE_POOLPTR, NULL);
            NewGetTaskAttrsA(tc, &poolmemflags, sizeof(ULONG), TASKINFOTYPE_POOLMEMFLAGS, NULL);
            NewGetTaskAttrsA(tc, &puddlesize, sizeof(ULONG), TASKINFOTYPE_POOLPUDDLESIZE, NULL);
            NewGetTaskAttrsA(tc, &threshsize, sizeof(ULONG), TASKINFOTYPE_POOLTHRESHSIZE, NULL);

            MySetContents(texts[ 0], "%lU", stacksize);
            MySetContents(texts[ 1], "%lU", stackused);
            MySetContents(texts[ 2], "$%08lx", trapmsgport);
            MySetContents(texts[ 3], "$%08lx", startupmsg);
            MySetContents(texts[ 4], "$%08lx", taskmsgport);
            MySetContents(texts[ 5], "$%08lx", poolptr);
            MySetContents(texts[ 6], "$%08lx", poolmemflags);   /* fixme */
            MySetContents(texts[ 7], "%lU", puddlesize);
            MySetContents(texts[ 8], "%lU", threshsize);

            DoMethod(tdwd->tdwd_MainGroup, MUIM_Group_InitChange);
            DoMethod(tdwd->tdwd_MainGroup, OM_ADDMEMBER, subgroup);
            DoMethod(tdwd->tdwd_MainGroup, MUIM_Group_ExitChange);
        }
    }
#endif

    if (tc->tc_Node.ln_Type != NT_PROCESS) {
        set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtTasksDetailTaskTitle, te->te_Name, tdwd->tdwd_Title, sizeof(tdwd->tdwd_Title)));
    } else {
        APTR subgroup, texts[40], seglist;
        struct Process *pr = (struct Process *)te->te_Addr;
        STRPTR path;
        BPTR *seg = NULL;
        struct MinList tmplist;
        struct SegListEntry *sle;

        subgroup = (Object *)VGroup,
            GroupFrameT(txtTaskProcessSpecific),
            Child, (IPTR)ColGroup(2),
            #if defined(__amigaos4__)
                Child, (IPTR)MyLabel2(txtTaskProcessCodeType),
                Child, (IPTR)texts[ 2] = MyTextObject6(),
            #endif
                Child, (IPTR)MyLabel2(txtTaskProcessSegList2),
                Child, (IPTR)(texts[ 0] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessStackSize),
                Child, (IPTR)(texts[ 1] = MyTextObject6()),
            #if !defined(__amigaos4__)
                Child, (IPTR)MyLabel2(txtTaskProcessGlobVec),
                Child, (IPTR)(texts[ 2] = MyTextObject6()),
            #endif
                Child, (IPTR)MyLabel2(txtTaskProcessTaskNum),
                Child, (IPTR)(texts[ 3] = MyTextObject6()),
            #if !defined(__amigaos4__)
                Child, (IPTR)MyLabel2(txtTaskProcessStackBase),
                Child, (IPTR)(texts[ 4] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
            #endif
                Child, (IPTR)MyLabel2(txtTaskProcessResult2),
                Child, (IPTR)(texts[ 5] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessCurrentDir),
                Child, (IPTR)(texts[ 6] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessCIS),
                Child, (IPTR)(texts[ 7] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessCOS),
                Child, (IPTR)(texts[ 8] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessConsoleTask),
                Child, (IPTR)(texts[ 9] = (Object *)TaskButtonObject,
                    MUIA_TaskButton_IsProcessPointer, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessFileSystemTask),
                Child, (IPTR)(texts[10] = (Object *)TaskButtonObject,
                    MUIA_TaskButton_IsProcessPointer, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessCLI2),
                Child, (IPTR)(texts[11] = MyTextObject6()),
            #if !defined(__amigaos4__)
                Child, (IPTR)MyLabel2(txtTaskProcessReturnAddr),
                Child, (IPTR)(texts[12] = (Object *)DisassemblerButtonObject,
                End),
            #endif
                Child, (IPTR)MyLabel2(txtTaskProcessPktWait),
                Child, (IPTR)(texts[13] = (Object *)DisassemblerButtonObject,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessWindowPtr),
                Child, (IPTR)(texts[14] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessHomeDir),
                Child, (IPTR)(texts[15] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessFlags2),
                Child, (IPTR)(texts[16] = (Object *)FlagsButtonObject,
                    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
                    MUIA_FlagsButton_Title, (IPTR)txtTaskProcessFlags,
                    MUIA_FlagsButton_BitArray, (IPTR)processFlags,
                    MUIA_FlagsButton_WindowTitle, (IPTR)txtTaskProcessFlagsTitle,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessExitCode),
                Child, (IPTR)(texts[17] = (Object *)DisassemblerButtonObject,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessExitData),
                Child, (IPTR)(texts[18] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessArguments),
                Child, (IPTR)(texts[19] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessLocalVars),
                Child, (IPTR)(texts[20] = MyTextObject6()),
                Child, (IPTR)MyLabel2(txtTaskProcessShellPrivate),
                Child, (IPTR)(texts[21] = (Object *)DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End),
                Child, (IPTR)MyLabel2(txtTaskProcessCES),
                Child, (IPTR)(texts[22] = MyTextObject6()),
            #if defined(__amigaos4__)
                Child, (IPTR)MyLabel2(txtTaskProcessPrData),
                Child, (IPTR)texts[23] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessCurrentSeg),
                Child, (IPTR)texts[24] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessEmulPrivate),
                Child, (IPTR)texts[25] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcess68kPrivate),
                Child, (IPTR)texts[26] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessParentID),
                Child, (IPTR)texts[27] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtTaskProcessProcessID),
                Child, (IPTR)texts[28] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtTaskProcessOGLContextPtr),
                Child, (IPTR)texts[29] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessCLibData),
                Child, (IPTR)texts[30] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessDeathSigTask),
                Child, (IPTR)texts[31] = TaskButtonObject,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessDeathSigBit),
                Child, (IPTR)texts[32] = MyTextObject6(),
                Child, (IPTR)MyLabel2(txtTaskProcessDeathMessage),
                Child, (IPTR)texts[33] = DisassemblerButtonObject,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessEntryCode),
                Child, (IPTR)texts[34] = DisassemblerButtonObject,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessEntryData),
                Child, (IPTR)texts[35] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessFinalCode),
                Child, (IPTR)texts[36] = DisassemblerButtonObject,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessFinalData),
                Child, (IPTR)texts[37] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessDLNotifyData),
                Child, (IPTR)texts[38] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
                Child, (IPTR)MyLabel2(txtTaskProcessPLNotifyData),
                Child, (IPTR)texts[39] = DisassemblerButtonObject,
                    MUIA_DisassemblerButton_ForceHexDump, TRUE,
                End,
            #endif
            End,
            Child, (IPTR)VGroup,
                GroupFrameT(txtTaskProcessSegList),
                Child, (IPTR)MyNListviewObject(&seglist, MakeID('S','L','L','V'), "BAR,BAR,BAR P=" MUIX_R, &seglist_con2hook, &seglist_des2hook, &seglist_dsp2hook, &seglist_cmp2hook, FALSE),
            End,
        End;

        DoMethod(tdwd->tdwd_MainGroup, MUIM_Group_InitChange);
        DoMethod(tdwd->tdwd_MainGroup, OM_ADDMEMBER, subgroup);
        DoMethod(tdwd->tdwd_MainGroup, MUIM_Group_ExitChange);

        path = tbAllocVecPooled(globalPool, PATH_LENGTH);

        MySetContents(texts[ 0], ADDRESS_FORMAT, BADDR(pr->pr_SegList));
        MySetContents(texts[ 1], "%lD", pr->pr_StackSize);
    #if defined(__amigaos4__)
        MySetContents(texts[2], "%s", txtTaskProcessCodeTypeUnknown);
        if (GetSegListInfoTags(pr->pr_CurrentSeg, GSLI_Native, NULL, TAG_DONE)) {
            MySetContents(texts[2], "%s", txtTaskProcessCodeTypePPC);
        }
        if (GetSegListInfoTags(pr->pr_CurrentSeg, GSLI_ElfHandle, NULL, TAG_DONE)) {
            MySetContents(texts[2], "%s", txtTaskProcessCodeTypePPCELF32);
        } else if (GetSegListInfoTags(pr->pr_CurrentSeg, GSLI_68KHUNK, NULL, TAG_DONE)) {
            MySetContents(texts[2], "%s", txtTaskProcessCodeType68kHunk);
        } else if (GetSegListInfoTags(pr->pr_CurrentSeg, GSLI_68KOVLAY, NULL, TAG_DONE)) {
            MySetContents(texts[2], "%s", txtTaskProcessCodeType68kOverlay);
        } else if (GetSegListInfoTags(pr->pr_CurrentSeg, GSLI_68KPS, NULL, TAG_DONE)) {
            MySetContents(texts[2], "%s", txtTaskProcessCodeType68kSegList);
        }
    #else
        MySetContents(texts[ 2], ADDRESS_FORMAT, pr->pr_GlobVec);
    #endif
        MySetContents(texts[ 3], "%s", te->te_Num);
    #if !defined(__amigaos4__)
        set(texts[ 4], MUIA_DisassemblerButton_Address, BADDR(pr->pr_StackBase));
    #endif
        if (path != NULL) {
            if (pr->pr_Result2 == 0) {
                stccpy(path, nonetest(NULL), PATH_LENGTH);
            } else {
                Fault(pr->pr_Result2, NULL, path, PATH_LENGTH);
            }
            MySetContents(texts[ 5], "%lD (%s)", pr->pr_Result2, path);
        } else {
            MySetContents(texts[ 5], "%lD", pr->pr_Result2);
        }

        if (pr->pr_CurrentDir != ZERO && path != NULL) {
            NameFromLock(pr->pr_CurrentDir, path, PATH_LENGTH);
            MySetContents(texts[ 6], "%s", path);
        } else {
            MySetContents(texts[ 6], "%s", nonetest(NULL));
        }
        MySetContents(texts[ 7], ADDRESS_FORMAT, BADDR(pr->pr_CIS));
        MySetContents(texts[ 8], ADDRESS_FORMAT, BADDR(pr->pr_COS));
        set(texts[ 9], MUIA_TaskButton_Task, pr->pr_ConsoleTask);
        set(texts[10], MUIA_TaskButton_Task, pr->pr_FileSystemTask);
        MySetContents(texts[11], ADDRESS_FORMAT, BADDR(pr->pr_CLI));
    #if !defined(__amigaos4__)
        set(texts[12], MUIA_DisassemblerButton_Address, pr->pr_ReturnAddr);
    #endif
        set(texts[13], MUIA_DisassemblerButton_Address, pr->pr_PktWait);
        MySetContents(texts[14], ADDRESS_FORMAT, pr->pr_WindowPtr);
        if (pr->pr_HomeDir != ZERO && path != NULL) {
            NameFromLock(pr->pr_HomeDir, path, PATH_LENGTH);
            MySetContents(texts[ 15], "%s", path);
        } else {
            MySetContents(texts[ 15], "%s", nonetest(NULL));
        }
        set(texts[16], MUIA_FlagsButton_Flags, pr->pr_Flags);
        set(texts[17], MUIA_DisassemblerButton_Address, pr->pr_ExitCode);
        set(texts[18], MUIA_DisassemblerButton_Address, pr->pr_ExitData);
        set(texts[19], MUIA_DisassemblerButton_Address, pr->pr_Arguments);
        MySetContents(texts[20], ADDRESS_FORMAT, pr->pr_LocalVars);
        set(texts[21], MUIA_DisassemblerButton_Address, pr->pr_ShellPrivate);
        MySetContents(texts[22], ADDRESS_FORMAT, BADDR(pr->pr_CES));
    #if defined(__amigaos4__)
        set(texts[23], MUIA_DisassemblerButton_Address, pr->pr_PrData);
        set(texts[24], MUIA_DisassemblerButton_Address, BADDR(pr->pr_CurrentSeg));
        set(texts[25], MUIA_DisassemblerButton_Address, pr->pr_EmulPrivate);
        set(texts[26], MUIA_DisassemblerButton_Address, pr->pr_68kPrivate);
        MySetContents(texts[27], "%lU", pr->pr_ParentID);
        MySetContents(texts[28], "%lU", pr->pr_ProcessID);
        set(texts[29], MUIA_DisassemblerButton_Address, pr->pr_OGLContextPtr);
        set(texts[30], MUIA_DisassemblerButton_Address, pr->pr_CLibData);
        set(texts[32], MUIA_TaskButton_Task, pr->pr_DeathSigTask);
        MySetContents(texts[32], "%s", GetSigBitName(pr->pr_DeathSigBit));
        set(texts[33], MUIA_DisassemblerButton_Address, pr->pr_DeathMessage);
        set(texts[34], MUIA_DisassemblerButton_Address, pr->pr_EntryCode);
        set(texts[35], MUIA_DisassemblerButton_Address, pr->pr_EntryData);
        set(texts[36], MUIA_DisassemblerButton_Address, pr->pr_FinalCode);
        set(texts[37], MUIA_DisassemblerButton_Address, pr->pr_FinalData);
        set(texts[38], MUIA_DisassemblerButton_Address, pr->pr_DLNotifyData);
        set(texts[39], MUIA_DisassemblerButton_Address, pr->pr_PLNotifyData);
    #endif

        NewList((struct List *)&tmplist);

        ApplicationSleep(TRUE);

        set(seglist, MUIA_NList_Quiet, TRUE);
        DoMethod(seglist, MUIM_NList_Clear);

        Forbid();

    #if defined(__amigaos4__)
        seg = (LONG *)BADDR(GetProcSegList(pr, GPSLF_CLI | GPSLF_SEG));
    #else
        if (pr->pr_CLI != ZERO) {
            seg = (BPTR *)BADDR(((struct CommandLineInterface *)BADDR(pr->pr_CLI))->cli_Module);
        } else if (pr->pr_SegList) {
            seg = (BPTR *)BADDR(((BPTR *)BADDR(pr->pr_SegList))[3]);
        }
        if (!points2ram((APTR)seg)) {
            seg = NULL;
        }
    #endif
        if (seg) {
            while (seg) {
                if ((sle = AllocVec(sizeof(struct SegListEntry), MEMF_CLEAR)) != NULL) {
                    ULONG size;

                    size = ((ULONG *)seg)[-1];
                    _snprintf(sle->sle_Lower, sizeof(sle->sle_Lower), ADDRESS_FORMAT, (IPTR)seg + 4);
                    _snprintf(sle->sle_Upper, sizeof(sle->sle_Upper), ADDRESS_FORMAT, (IPTR)seg + size - 4);
                    _snprintf(sle->sle_Size, sizeof(sle->sle_Size), "%12lD", size);

                    AddTail((struct List *)&tmplist, (struct Node *)sle);
                }

                seg = (BPTR *)BADDR(*seg);
            }
        }

        Permit();

        ITERATE_LIST(&tmplist, struct SegListEntry *, sle) {
            InsertBottomEntry(seglist, sle);
        }
        FreeLinkedList((struct List *)&tmplist);

        set(seglist, MUIA_NList_Quiet, FALSE);

        ApplicationSleep(FALSE);

        if (pr->pr_CLI != ZERO) {
            struct CommandLineInterface *cli = BADDR(pr->pr_CLI);
            APTR cligroup;

            cligroup = (Object *)VGroup,
                GroupFrameT(txtTaskProcessCLI),
                Child, (IPTR)ColGroup(2),
                    Child, (IPTR)MyLabel2(txtTaskCLIResult2),
                    Child, (IPTR)(texts[ 0] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLISetName),
                    Child, (IPTR)(texts[ 1] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLICommandDir),
                    Child, (IPTR)(texts[ 2] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIReturnCode),
                    Child, (IPTR)(texts[ 3] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLICommandName),
                    Child, (IPTR)(texts[ 4] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIFailLevel),
                    Child, (IPTR)(texts[ 5] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIPrompt),
                    Child, (IPTR)(texts[ 6] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIStandardInput),
                    Child, (IPTR)(texts[ 7] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLICurrentInput),
                    Child, (IPTR)(texts[ 8] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLICommandFile),
                    Child, (IPTR)(texts[ 9] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIInteractive),
                    Child, (IPTR)(texts[10] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIBackground),
                    Child, (IPTR)(texts[11] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLICurrentOutput),
                    Child, (IPTR)(texts[12] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIDefaultStack),
                    Child, (IPTR)(texts[13] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIStandardOutput),
                    Child, (IPTR)(texts[14] = MyTextObject6()),
                    Child, (IPTR)MyLabel2(txtTaskCLIModule),
                    Child, (IPTR)(texts[15] = MyTextObject6()),
                End,
            End;

            DoMethod(subgroup, MUIM_Group_InitChange);
            DoMethod(subgroup, OM_ADDMEMBER, cligroup);
            DoMethod(subgroup, MUIM_Group_ExitChange);

            if (path != NULL) {
                if (cli->cli_Result2 == 0) {
                    stccpy(path, nonetest(NULL), PATH_LENGTH);
                } else {
                    Fault(cli->cli_Result2, NULL, path, PATH_LENGTH);
                }
                MySetContents(texts[ 0], "%lD (%s)", cli->cli_Result2, path);
            } else {
                MySetContents(texts[ 0], "%lD", cli->cli_Result2);
            }
            MySetContents(texts[ 1], "\"%b\"", cli->cli_SetName);
            MySetContents(texts[ 2], ADDRESS_FORMAT, BADDR(cli->cli_CommandDir));
            MySetContents(texts[ 3], "%lD", cli->cli_ReturnCode);
            MySetContents(texts[ 4], "\"%b\"", cli->cli_CommandName);
            MySetContents(texts[ 5], "%lD", cli->cli_FailLevel);
            MySetContents(texts[ 6], "\"%b\"", cli->cli_Prompt);
            MySetContents(texts[ 7], ADDRESS_FORMAT, BADDR(cli->cli_StandardInput));
            MySetContents(texts[ 8], ADDRESS_FORMAT, BADDR(cli->cli_CurrentInput));
            MySetContents(texts[ 9], "\"%b\"", cli->cli_CommandFile);
            MySetContents(texts[10], "%s", (cli->cli_Interactive) ? txtCLIBatch : txtCLIInteractive);
            MySetContents(texts[11], "%s", (cli->cli_Background) ? txtCLIBackground : txtCLIForeground);
            MySetContents(texts[12], ADDRESS_FORMAT, BADDR(cli->cli_CurrentOutput));
            MySetContents(texts[13], "%lU", cli->cli_DefaultStack);
            MySetContents(texts[14], ADDRESS_FORMAT, BADDR(cli->cli_StandardOutput));
            MySetContents(texts[15], ADDRESS_FORMAT, BADDR(cli->cli_Module));
        }

        if (path != NULL) {
            tbFreeVecPooled(globalPool, path);
        }

        set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtTasksDetailProcessTitle, te->te_Name, tdwd->tdwd_Title, sizeof(tdwd->tdwd_Title)));
    }
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[26], exitButton, maingroup;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, "Tasks",
        MUIA_Window_ID, MakeID('.','T','A','S'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)(maingroup = (Object *)VGroup,
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
                            Child, (IPTR)MyLabel2(txtTaskFlags2),
                            Child, (IPTR)(texts[ 4] = (Object *)FlagsButtonObject,
                                MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Byte,
                                MUIA_FlagsButton_Title, (IPTR)txtTaskFlags,
                                MUIA_FlagsButton_BitArray, (IPTR)taskFlags,
                                MUIA_FlagsButton_WindowTitle, (IPTR)txtTaskFlagsTitle,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskState2),
                            Child, (IPTR)(texts[ 5] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskIDNestCnt),
                            Child, (IPTR)(texts[ 6] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskTDNestCnt),
                            Child, (IPTR)(texts[ 7] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSigAlloc),
                            Child, (IPTR)(texts[ 8] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSigWait2),
                            Child, (IPTR)(texts[ 9] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSigRecvd),
                            Child, (IPTR)(texts[10] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSigExcept),
                            Child, (IPTR)(texts[11] = MyTextObject6()),
                        #if defined(__amigaos4__)
                            Child, (IPTR)MyLabel2(txtTaskETask),
                            Child, (IPTR)texts[12] = DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End,
                        #else
                            Child, (IPTR)MyLabel2(txtTaskTrapAlloc),
                            Child, (IPTR)(texts[12] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskTrapAble),
                            Child, (IPTR)(texts[13] = MyTextObject6()),
                        #endif
                            Child, (IPTR)MyLabel2(txtTaskExceptData),
                            Child, (IPTR)(texts[14] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskExceptCode),
                            Child, (IPTR)(texts[15] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskTrapData),
                            Child, (IPTR)(texts[16] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskTrapCode),
                            Child, (IPTR)(texts[17] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskSPReg),
                            Child, (IPTR)(texts[18] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSPLower),
                            Child, (IPTR)(texts[19] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSPUpper),
                            Child, (IPTR)(texts[20] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskStacksize),
                            Child, (IPTR)(texts[21] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskSwitch),
                            Child, (IPTR)(texts[22] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskLaunch),
                            Child, (IPTR)(texts[23] = (Object *)DisassemblerButtonObject,
                            End),
                            Child, (IPTR)MyLabel2(txtTaskMemEntry),
                            Child, (IPTR)(texts[24] = MyTextObject6()),
                            Child, (IPTR)MyLabel2(txtTaskUserData),
                            Child, (IPTR)(texts[25] = (Object *)DisassemblerButtonObject,
                                MUIA_DisassemblerButton_ForceHexDump, TRUE,
                            End),
                        End,
                    End),
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct TasksDetailWinData *tdwd = INST_DATA(cl, obj);
        APTR parent;

    #if defined(__amigaos4__)
        // just for completeness, because this one is unused
        texts[13] = NULL;
    #endif
        CopyMemQuick(texts, tdwd->tdwd_Texts, sizeof(tdwd->tdwd_Texts));
        tdwd->tdwd_MainGroup = maingroup;

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
    struct TasksDetailWinData *tdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, (APTR)tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

            case MUIA_TasksDetailWin_Task:
                tdwd->tdwd_Task = (struct TaskEntry *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(TasksDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeTasksDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct TasksDetailWinData), ENTRY(TasksDetailWinDispatcher));
}

