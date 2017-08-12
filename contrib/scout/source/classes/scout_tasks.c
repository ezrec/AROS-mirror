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

struct TasksWinData {
    TEXT twd_Title[WINDOW_TITLE_LENGTH];
    APTR twd_Application;
    APTR twd_TaskList;
    APTR twd_TaskText;
    APTR twd_TaskCount;
    APTR twd_RemoveButton;
    APTR twd_PriorityButton;
    APTR twd_MoreButton;
    APTR twd_SignalButton;
    APTR twd_BreakButton;
    APTR twd_FreezeButton;
    APTR twd_ActivateButton;
    APTR twd_CPUGauge;
    APTR twd_CPUCycle;
    APTR twd_RefreshString;
    ULONG twd_CPUCheck;
    struct MUI_InputHandlerNode twd_TimerHandler;
    struct MsgPort *twd_TimerPort;
    struct TimeRequest *twd_TimerIO;
    BOOL twd_TimerHandlerAdded;
    BOOL twd_TimerOpen;
    struct TaskEntry twd_NewEntry;
    struct TaskEntry twd_CompareEntry;
    struct Task *twd_CheatTask;
};

struct TasksCallbackUserData {
    APTR ud_List;
    ULONG ud_Count;
};

struct CPUCheckChangeMessage {
    ULONG methodID;
    ULONG value;
};

#define SCOUT_SEMA_NAME         "« Scout's CPU patch »"
#define CHEATTASK_STACKSIZE     1024
#define CHEATTASK_NAME          "« Scout's cheat task »"

struct CheatTask {
    struct Task ct_Task;
    TEXT ct_TaskName[32];
    UBYTE ct_Stack[CHEATTASK_STACKSIZE];
};

#if !defined(__amigaos4__)
STATIC LONG totalmicros;
#endif
TEXT updatetimetext[8];

HOOKPROTONHNO(taskslist_con2func, IPTR, struct NList_ConstructMessage *msg)
{
    return AllocListEntry(msg->pool, msg->entry, sizeof(struct TaskEntry));
}
MakeStaticHook(taskslist_con2hook, taskslist_con2func);

HOOKPROTONHNO(taskslist_des2func, void, struct NList_DestructMessage *msg)
{
    FreeListEntry(msg->pool, &msg->entry);
}
MakeStaticHook(taskslist_des2hook, taskslist_des2func);

HOOKPROTONHNO(taskslist_dsp2func, void, struct NList_DisplayMessage *msg)
{
    struct TaskEntry *te = (struct TaskEntry *)msg->entry;

    if (te) {
        msg->strings[0] = te->te_Address;
        msg->strings[1] = te->te_Name;
        msg->strings[2] = te->te_CPU;
        msg->strings[3] = te->te_Type;
        msg->strings[4] = te->te_Pri;
        msg->strings[5] = te->te_Num;
        msg->strings[6] = te->te_State;
        msg->strings[7] = te->te_SigWait;
        msg->strings[8] = te->te_FreeStack;
#ifdef __MORPHOS__
        msg->strings[9] = te->te_FreeStackPPC;
#endif
    } else {
        msg->strings[0] = txtAddress;
        msg->strings[1] = txtNodeName;
        msg->strings[2] = txtCPUPercentTitle;
        msg->strings[3] = txtNodeType;
        msg->strings[4] = txtNodePri;
        msg->strings[5] = txtTaskNum;
        msg->strings[6] = txtTaskState;
        msg->strings[7] = txtTaskSigWait;
        msg->strings[8] = txtTaskFreeStack;
#ifdef __MORPHOS__
        msg->strings[9] = txtTaskFreeStackPPC;
#endif
        msg->preparses[0] = (STRPTR)MUIX_B;
        msg->preparses[1] = (STRPTR)MUIX_B;
        msg->preparses[2] = (STRPTR)MUIX_B;
        msg->preparses[3] = (STRPTR)MUIX_B;
        msg->preparses[4] = (STRPTR)MUIX_B;
        msg->preparses[5] = (STRPTR)MUIX_B;
        msg->preparses[6] = (STRPTR)MUIX_B;
        msg->preparses[7] = (STRPTR)MUIX_B;
        msg->preparses[8] = (STRPTR)MUIX_B;
#ifdef __MORPHOS__
        msg->preparses[9] = (STRPTR)MUIX_B;
#endif
    }
}
MakeStaticHook(taskslist_dsp2hook, taskslist_dsp2func);

STATIC LONG taskslist_cmp2colfunc( struct TaskEntry *te1,
                                   struct TaskEntry *te2,
                                   ULONG column )
{
    switch (column) {
        default:
        case 0: return HexCompare(te1->te_Address, te2->te_Address);
        case 1: return stricmp(te1->te_Name, te2->te_Name);
        case 2: return stricmp(te1->te_CPU, te2->te_CPU);
        case 3: return stricmp(te1->te_Type, te2->te_Type);
        case 4: return PriorityCompare(te1->te_Pri, te2->te_Pri);
        case 5: return stricmp(te1->te_Num, te2->te_Num);
        case 6: return stricmp(te1->te_State, te2->te_State);
        case 7: return stricmp(te1->te_SigWait, te2->te_SigWait);
        case 8: return IntegerCompare(te1->te_FreeStack, te2->te_FreeStack);
#ifdef __MORPHOS__
        case 9: return IntegerCompare(te1->te_FreeStackPPC, te2->te_FreeStackPPC);
#endif
    }
}

STATIC LONG tasklist_cmpfunc( const struct Node *n1,
                              const struct Node *n2 )
{
    return stricmp(((struct TaskEntry *)n1)->te_Name, ((struct TaskEntry *)n2)->te_Name);
}

HOOKPROTONHNO(taskslist_cmp2func, LONG, struct NList_CompareMessage *msg)
{
    LONG cmp;
    struct TaskEntry *te1, *te2;
    ULONG col1, col2;

    te1 = (struct TaskEntry *)msg->entry1;
    te2 = (struct TaskEntry *)msg->entry2;

    col1 = msg->sort_type & MUIV_NList_TitleMark_ColMask;
    col2 = msg->sort_type2 & MUIV_NList_TitleMark2_ColMask;

    if ((ULONG)msg->sort_type == MUIV_NList_SortType_None) return 0;

    if (msg->sort_type & MUIV_NList_TitleMark_TypeMask) {
        cmp = taskslist_cmp2colfunc(te2, te1, col1);
    } else {
        cmp = taskslist_cmp2colfunc(te1, te2, col1);
    }

    if (cmp != 0 || col1 == col2) return cmp;

    if (msg->sort_type2 & MUIV_NList_TitleMark2_TypeMask) {
        cmp = taskslist_cmp2colfunc(te2, te1, col2);
    } else {
        cmp = taskslist_cmp2colfunc(te1, te2, col2);
    }

    return cmp;
}
MakeStaticHook(taskslist_cmp2hook, taskslist_cmp2func);

STATIC LONG ParseUpdateTime( APTR obj,
                             STRPTR timestr )
{
    TEXT text[8];
    LONG i, secs, mics, result;

    stccpy(text, timestr, sizeof(text));

    i = 0;
    while (text[i] != '.' && text[i] != 0x00) i++;
    text[i++] = 0x00;

    secs = atol((TEXT *)text);
    if (text[i] >= '0' && text[i] <= '9') {
        mics = text[i] - '0';
    } else {
        mics = 0;
    }

    if (secs == 0 && mics == 0) {
        secs = 1;
    }
    _snprintf(text, sizeof(text), "%ld.%ld", secs, mics);
    set(obj, MUIA_String_Contents, text);

    result = secs * 1000 + mics * 100;

    return result;
}

HOOKPROTONH(cpuinterval_callfunc, void, Object *obj, STRPTR *contents )
{
    struct TasksWinData *twd = INST_DATA(OCLASS(obj), obj);

    stccpy(updatetimetext, (TEXT *)(*contents), sizeof(updatetimetext));

    if (twd->twd_TimerHandlerAdded) {
        DoMethod(twd->twd_Application, MUIM_Application_RemInputHandler, &twd->twd_TimerHandler);
        twd->twd_TimerHandler.ihn_Millis = ParseUpdateTime(twd->twd_RefreshString, *contents);
        DoMethod(twd->twd_Application, MUIM_Application_AddInputHandler, &twd->twd_TimerHandler);
    }
}
MakeStaticHook(cpuinterval_callhook, cpuinterval_callfunc);

HOOKPROTONH(realstring_editfunc, ULONG, struct SGWork *sgw, IPTR *msg)
{
    ULONG return_code;
    ULONG i = 0, punkte = 0, notzeros = 0;

    return_code = ~0L;

    if (*msg == SGH_KEY) {
        if (sgw->EditOp == EO_REPLACECHAR || sgw->EditOp == EO_INSERTCHAR) {
            while (sgw->WorkBuffer[i]) {
                if (sgw->WorkBuffer[i++] == '.') punkte++;
            }
            if (punkte > 1 || (!isdigit(sgw->Code) && sgw->Code != '.')) {
                SET_FLAG(sgw->Actions, SGA_BEEP);
                CLEAR_FLAG(sgw->Actions, SGA_USE);
            }
        } else if (sgw->EditOp == EO_ENTER) {
            while (sgw->WorkBuffer[i]) {
                if (sgw->WorkBuffer[i] >= '1' && sgw->WorkBuffer[i] <= '9') notzeros++;
                i++;
            }
            if (notzeros > 0) {
                SET_FLAG(sgw->Actions, SGA_BEEP);
                CLEAR_FLAG(sgw->Actions, SGA_USE);
            }
        }
    } else {
        return_code = 0;
    }

    return return_code;
}
MakeStaticHook(realstring_edithook, realstring_editfunc);

STATIC void cheatTask_func( void )
{
   while (TRUE) {}
}

STATIC struct Task *AddCheatTask( void )
{
    struct Task *result = NULL;

    // AmigaOS4 currently doesn't like our way to get the CPU usage, so we disable it
    // and MorphOS doesn't need the cheat task
    if (!(amigaOS4 || morphOS || arOS)) {
        struct CheatTask *ct;

        if ((ct = tbAllocVecPooled(globalPool, sizeof(*ct))) != NULL) {
            ct->ct_Task.tc_Node.ln_Name = ct->ct_TaskName;
            ct->ct_Task.tc_Node.ln_Pri = -128;
            ct->ct_Task.tc_Node.ln_Type = NT_TASK;
            ct->ct_Task.tc_SPLower = ct->ct_Stack;
            ct->ct_Task.tc_SPUpper = ct->ct_Stack + sizeof(ct->ct_Stack);
            ct->ct_Task.tc_SPReg = ct->ct_Task.tc_SPUpper;
            stccpy(ct->ct_TaskName, CHEATTASK_NAME, sizeof(ct->ct_TaskName));

        #if defined(__amigaos4__)
            AddTask(&ct->ct_Task, (APTR)&cheatTask_func, NULL, NULL);
        #else
            AddTask(&ct->ct_Task, (APTR)&cheatTask_func, NULL);
        #endif

            result = &ct->ct_Task;
        }
    }

    return result;
}

STATIC void RemoveCheatTask( struct Task **task )
{
    if (*task) {
        RemTask(*task);
        tbFreeVecPooled(globalPool, *task);
        *task = NULL;
    }
}

struct Task *TaskExists( struct Task *tasktofind )
{
    struct Task *task, *result = NULL;

    if (tasktofind == myprocess) {
        return myprocess;
    }

    Forbid();

    ITERATE_LIST(&SysBase->TaskReady, struct Task *, task) {
        if (task == tasktofind) {
            result = task;
            break;
        }
    }

    if (result == NULL) {
        ITERATE_LIST(&SysBase->TaskWait, struct Task *, task) {
            if (task == tasktofind) {
                result = task;
                break;
            }
        }
    }

    Permit();

    return result;
}

CONST_STRPTR GetTaskState( UBYTE state,
                           ULONG waitmask )
{
    switch (state) {
        case TS_FROZEN:    return txtTaskStateFrozen;
        case TS_ADDED:     return txtTaskStateAdded;
        case TS_RUN:       return txtTaskStateRunning;
        case TS_READY:     return txtTaskStateReady;
        case TS_WAIT:
            switch (waitmask) {
                case SIGF_ABORT:       return txtTaskStateWaitAbort;
                case SIGF_CHILD:       return txtTaskStateWaitChild;
                case SIGF_SINGLE:      return txtTaskStateWaitSemaphore;
                case SIGF_INTUITION:   return txtTaskStateWaitIntuition;
                case SIGF_NET:         return txtTaskStateWaitNet;
                case SIGF_DOS:         return txtTaskStateWaitDOS;
                case SIGBREAKF_CTRL_C: return txtTaskStateWaitBreakC;
                case SIGBREAKF_CTRL_D: return txtTaskStateWaitBreakD;
                case SIGBREAKF_CTRL_E: return txtTaskStateWaitBreakE;
                case SIGBREAKF_CTRL_F: return txtTaskStateWaitBreakF;
                case 0x00000000:       return txtTaskStateSuspended;
                default:               return txtTaskStateWaiting;
            }
        case TS_EXCEPT:    return txtTaskStateExcept;
        case TS_REMOVED:   return txtTaskStateRemoved;
    #ifdef __amigaos4__
        case TS_CRASHED:   return txtTaskStateCrashed;
        case TS_SUSPENDED: return txtTaskStateSuspended;
    #endif
        case TS_INVALID:   return txtTaskStateInvalid;
        default:           return txtTaskStateUnknown;
    }
}

CONST_STRPTR GetNodeType( UBYTE type )
{
    STATIC TEXT invalidType[64];

    switch (type) {
        case NT_TASK:         return txtNodeTypeTask;
        case NT_INTERRUPT:    return txtNodeTypeInterrupt;
        case NT_DEVICE:       return txtNodeTypeDevice;
        case NT_MSGPORT:      return txtNodeTypeMsgPort;
        case NT_MESSAGE:      return txtNodeTypeMessage;
        case NT_FREEMSG:      return txtNodeTypeFreeMsg;
        case NT_REPLYMSG:     return txtNodeTypeReplyMsg;
        case NT_RESOURCE:     return txtNodeTypeResource;
        case NT_LIBRARY:      return txtNodeTypeLibrary;
        case NT_MEMORY:       return txtNodeTypeMemory;
        case NT_SOFTINT:      return txtNodeTypeSoftInt;
        case NT_FONT:         return txtNodeTypeFont;
        case NT_PROCESS:      return txtNodeTypeProcess;
        case NT_SEMAPHORE:    return txtNodeTypeSemaphore;
        case NT_SIGNALSEM:    return txtNodeTypeSignalSem;
        case NT_BOOTNODE:     return txtNodeTypeBootNode;
        case NT_KICKMEM:      return txtNodeTypeKickMem;
        case NT_GRAPHICS:     return txtNodeTypeGraphics;
        case NT_DEATHMESSAGE: return txtNodeTypeDeathMessage;
    #if defined(__amigaos4__)
        case NT_EXTINTERRUPT: return txtNodeTypeExtInterrupt;
        case NT_EXTSOFTINT:   return txtNodeTypeExtSoftInt;
        case NT_VMAREA:       return txtNodeTypeVMArea;
        case NT_VMAREA_PROXY: return txtNodeTypeVMAreaProxy;
        case NT_INTERFACE:    return txtNodeTypeInterface;
        #if 0
        case NT_KMEMCACHE:    return txtNodeTypeKMemCache;
        #endif
    #endif
        case NT_USER:         return txtNodeTypeUser;
        case NT_EXTENDED:     return txtNodeTypeExtended;
        case NT_UNKNOWN:      return txtNodeTypeUnknown;
        default:              _snprintf(invalidType, sizeof(invalidType), txtNodeTypeInvalid, type); return invalidType;
    };
}

STRPTR GetTaskName( struct Task *task,
                    STRPTR buffer,
                    ULONG maxlen )
{
   buffer[0] = 0x00;

   if (task) {
        if (task->tc_Node.ln_Succ != NULL && (task->tc_Node.ln_Type == NT_PROCESS || task->tc_Node.ln_Type == NT_TASK)) {
            struct Process *pr = (struct Process *)task;

            stccpy(buffer, nonetest(task->tc_Node.ln_Name), maxlen);

            if (task->tc_Node.ln_Type == NT_PROCESS && pr->pr_CLI != ZERO) {
                struct CommandLineInterface *cli= (struct CommandLineInterface *)BADDR(pr->pr_CLI);

                if (cli->cli_Module != ZERO && cli->cli_CommandName != ZERO) {
                    _snprintf(buffer, maxlen, "%s [%b]", buffer, cli->cli_CommandName);
                }
            }
        } else {
            stccpy(buffer, txtNoTask, maxlen);
        }
    } else {
        stccpy(buffer, nonetest(NULL), maxlen);
    }

    return buffer;
}

CONST_STRPTR GetSigBitName( LONG bit )
{
    STATIC TEXT sigNameBuffer[16];

    switch (bit) {
        case SIGB_ABORT:       return "SIGB_ABORT";
        case SIGB_CHILD:       return "SIGB_CHILD";
        case SIGB_SINGLE:      return "SIGB_SINGLE";
        case SIGB_INTUITION:   return "SIGB_INTUITION";
        case SIGB_NET:         return "SIGB_NET";
        case SIGB_DOS:         return "SIGB_DOS";
        case SIGBREAKB_CTRL_C: return "SIGBREAKB_CTRL_C";
        case SIGBREAKB_CTRL_D: return "SIGBREAKB_CTRL_D";
        case SIGBREAKB_CTRL_E: return "SIGBREAKB_CTRL_E";
        case SIGBREAKB_CTRL_F: return "SIGBREAKB_CTRL_F";
        default:
            _snprintf(sigNameBuffer, sizeof(sigNameBuffer), "%lD", bit);
            return sigNameBuffer;
    }
}

STATIC void GetCPUUsage( UNUSED struct Task *task,
                         UNUSED struct TaskEntry *te )
{
#if !defined(__amigaos4__)
// FIXME: AROS?
    if ((totalmicros = TotalMicros2)) {
        ULONG cpu, zehntel;

        cpu = GetTaskData(task);

        zehntel = UDivMod32(totalmicros, 10000);
        if (zehntel == 0) {
            zehntel = cpu;
        } else {
            zehntel = UDivMod32(cpu, zehntel);
        }
        cpu = UDivMod32(zehntel, 100);
        zehntel -= UMult32(cpu, 100);
        if (cpu > 99) {
            cpu = 100;
            zehntel = 0;
        }
        _snprintf(te->te_CPU, sizeof(te->te_CPU), "%3ld.%02ld", cpu, zehntel);
    }
#endif
}

STATIC void UpdateTaskEntry( UNUSED struct Task *task,
                             UNUSED struct TaskEntry *te,
                             UNUSED BOOL cpuflag )
{
#ifdef __MORPHOS__
    register ULONG REG_GPR1 __asm("r1");
    ULONG total, used;
    LONG  stack;

    NewGetTaskAttrsA(task, &total, sizeof(ULONG), TASKINFOTYPE_STACKSIZE, NULL);

    if (task == SysBase->ThisTask && SysBase->LibNode.lib_Version == 50 && SysBase->LibNode.lib_Revision <= 69) {
        used = (ULONG)task->tc_ETask->PPCSPUpper - REG_GPR1;
    } else {
        NewGetTaskAttrsA(task, &used, sizeof(ULONG), TASKINFOTYPE_USEDSTACKSIZE, NULL);
    }

    stack = total - used;

    if (stack >= 1024) {
        _snprintf(te->te_FreeStackPPC, sizeof(te->te_FreeStackPPC), "%8lD", stack);
    } else {
        _snprintf(te->te_FreeStackPPC, sizeof(te->te_FreeStackPPC), MUIX_PH "%8lD" MUIX_PT, stack);
    }
#endif

    GetTaskName(task, te->te_Name, sizeof(te->te_Name));
    healstring(te->te_Name);
    _snprintf(te->te_Pri, sizeof(te->te_Pri), "%4ld", task->tc_Node.ln_Pri);
    stccpy(te->te_State, GetTaskState((UBYTE)task->tc_State, task->tc_SigWait), sizeof(te->te_State));
    _snprintf(te->te_SigWait, sizeof(te->te_SigWait), "$%08lx", task->tc_SigWait); // ULONG

    if ((IPTR)task->tc_SPUpper >= (IPTR)task->tc_SPReg &&
        (IPTR)task->tc_SPReg   >= (IPTR)task->tc_SPLower) {
        SIPTR stackFree;

        stackFree = (IPTR)task->tc_SPReg - (IPTR)task->tc_SPLower;
        if (stackFree >= 512) {
            _snprintf(te->te_FreeStack, sizeof(te->te_FreeStack), "%8lD", stackFree);
        } else {
            _snprintf(te->te_FreeStack, sizeof(te->te_FreeStack), MUIX_PH "%8lD" MUIX_PT, stackFree);
        }
    } else {
        // Stackpointer is out of limits, this program is most probably an AmigaE program
        stccpy(te->te_FreeStack, MUIX_PH "---" MUIX_PT, sizeof(te->te_FreeStack));
    }

    stccpy(te->te_CPU, "  0.00", sizeof(te->te_CPU));

    if (cpuflag && PatchesInstalled) {
        GetCPUUsage(task, te);
    }
}

void GetTaskEntry( struct Task *task,
                   struct TaskEntry *te,
                   BOOL cpuflag )
{
    te->te_Addr = task;

    _snprintf(te->te_Address, sizeof(te->te_Address), ADDRESS_FORMAT, task);
    stccpy(te->te_Type, GetNodeType(task->tc_Node.ln_Type), sizeof(te->te_Type));

    if (task->tc_Node.ln_Type == NT_TASK || ((struct Process *)task)->pr_TaskNum == 0) {
        stccpy(te->te_Num, "---", sizeof(te->te_Num));
    } else {
        _snprintf(te->te_Num, sizeof(te->te_Num), "%3ld", ((struct Process *)task)->pr_TaskNum);
    }

    UpdateTaskEntry(task, te, cpuflag);
}

STATIC void ReceiveList( void (* callback)( struct TaskEntry *te, void *userData ),
                         void *userData )
{
    struct TaskEntry *te;

    if ((te = tbAllocVecPooled(globalPool, sizeof(struct TaskEntry))) != NULL) {
        if (SendDaemon("GetTaskList")) {
            while (ReceiveDecodedEntry(te, sizeof(struct TaskEntry))) {
                callback(te, userData);
            }
        }

        tbFreeVecPooled(globalPool, te);
    }
}

STATIC void IterateList( void (* callback)( struct TaskEntry *te, void *userData ),
                         void *userData,
                         BOOL sort )
{
    struct MinList tmplist;
    struct TaskEntry *te;
    struct Task *task;

    NewList((struct List *)&tmplist);

    Forbid();

    ITERATE_LIST(&SysBase->TaskWait, struct Task *, task) {
        if ((te = AllocVec(sizeof(struct TaskEntry), MEMF_CLEAR)) != NULL) {
            GetTaskEntry(task, te, FALSE);
            AddTail((struct List *)&tmplist, (struct Node *)te);
        }
    }

    ITERATE_LIST(&SysBase->TaskReady, struct Task *, task) {
        if ((te = AllocVec(sizeof(struct TaskEntry), MEMF_CLEAR)) != NULL) {
            GetTaskEntry(task, te, FALSE);
            AddTail((struct List *)&tmplist, (struct Node *)te);
        }
    }

    if ((te = AllocVec(sizeof(struct TaskEntry), MEMF_CLEAR)) != NULL) {
        GetTaskEntry(FindTask(NULL), te, FALSE);
        AddTail((struct List *)&tmplist, (struct Node *)te);
    }

    Permit();

    if (sort) SortLinkedList((struct List *)&tmplist, tasklist_cmpfunc);

    ITERATE_LIST(&tmplist, struct TaskEntry *, te) {
        callback(te, userData);
    }
    FreeLinkedList((struct List *)&tmplist);
}

STATIC void UpdateCallback( struct TaskEntry *te,
                            void *userData )
{
    struct TasksCallbackUserData *ud = (struct TasksCallbackUserData *)userData;

    InsertSortedEntry(ud->ud_List, te);
    ud->ud_Count++;
}

STATIC void PrintCallback( struct TaskEntry *te,
                           void *userData )
{
    PrintFOneLine((BPTR)userData, txtTasksPrintLine, te->te_Address, te->te_Type, te->te_Pri, te->te_Num, te->te_State, te->te_SigWait, StripMUIFormatting(te->te_FreeStack), te->te_Name);
}

STATIC void SendCallback( struct TaskEntry *te,
                          UNUSED void *userData )
{
    SendEncodedEntry(te, sizeof(struct TaskEntry));
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    STATIC STRPTR CYA_CpuUsageText[] = { NULL, NULL, NULL, NULL };
    APTR taskslist, taskstext, taskscount, freezeButton, activateButton, signalButton, breakButton, updateButton, printButton, removeButton, priorityButton, moreButton, exitButton;
    APTR cpucount, cpuCycle, taskstext2;

    CYA_CpuUsageText[0] = txtCPUOff;
    CYA_CpuUsageText[1] = txtCPUFull;
    CYA_CpuUsageText[2] = txtCPUPercent;

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_HelpNode, (IPTR)"Tasks",
        MUIA_Window_ID, MakeID('T','A','S','K'),
        WindowContents, VGroup,

#ifdef __MORPHOS__
            Child, (IPTR)MyNListviewObject(&taskslist, MakeID('T','A','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR,BAR P=" MUIX_R ",BAR P=" MUIX_R, &taskslist_con2hook, &taskslist_des2hook, &taskslist_dsp2hook, &taskslist_cmp2hook, TRUE),
#else
            Child, (IPTR)MyNListviewObject(&taskslist, MakeID('T','A','L','V'), "BAR,BAR,BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR P=" MUIX_R ",BAR P=" MUIX_R ",BAR P=" MUIX_C ",BAR,BAR P=" MUIX_R, &taskslist_con2hook, &taskslist_des2hook, &taskslist_dsp2hook, &taskslist_cmp2hook, TRUE),
#endif
            Child, (IPTR)MyBelowListview(&taskstext, &taskscount),

            Child, (IPTR)MyVSpace(4),

            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(printButton    = MakeButton(txtPrint)),
                Child, (IPTR)(freezeButton   = MakeButton(txtFreeze)),
                Child, (IPTR)(activateButton = MakeButton(txtActivate)),
                Child, (IPTR)HGroup,
                    Child, (IPTR)KeyLabel1(txtCPU, 'c'),
                    Child, (IPTR)(cpucount = (Object *)GaugeObject,
                        MUIA_Frame, MUIV_Frame_Gauge,
                        MUIA_Gauge_Horiz, TRUE,
                        MUIA_Gauge_Max, 100,
                        MUIA_Gauge_InfoText, (IPTR)MUIX_PT "-----",
                        MUIA_Gauge_Current, 0,
                    End),
                End,
                Child, (IPTR)(cpuCycle = (Object *)CycleObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Weight, 0,
                    MUIA_ControlChar, 'c',
                    MUIA_Cycle_Entries, (IPTR)CYA_CpuUsageText,
                    // AmigaOS3 and MorphOS can get the CPU usage, so here we enable the cycler
                    MUIA_Cycle_Active, (opts.CpuDisplay && !amigaOS4) ? *opts.CpuDisplay : 0,
                    MUIA_Disabled, amigaOS4,
                End),
                Child, (IPTR)HGroup,
                    Child, (IPTR)KeyLabel2(txtSeconds, 'e'),
                    Child, (IPTR)(taskstext2 = (Object *)StringObject,
                        StringFrame,
                        MUIA_CycleChain, TRUE,
                        MUIA_String_BufferPos, 0,
                        MUIA_String_MaxLen, 6,
                        MUIA_String_EditHook, (IPTR)&realstring_edithook,
                        MUIA_String_Contents, (IPTR)updatetimetext,
                        MUIA_String_Format, MUIV_String_Format_Center,
                    End),
                End,
            End,
            Child, (IPTR)HGroup, MUIA_Group_SameSize, TRUE,
                Child, (IPTR)(updateButton   = MakeButton(txtUpdate)),
                Child, (IPTR)(removeButton   = MakeButton(txtRemove)),
                Child, (IPTR)(signalButton   = MakeButton(txtSignal)),
                Child, (IPTR)(breakButton    = MakeButton(txtBreak)),
                Child, (IPTR)(priorityButton = MakeButton(txtPriority)),
                Child, (IPTR)(moreButton     = MakeButton(txtMore)),
                Child, (IPTR)(exitButton     = MakeButton(txtExit)),
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct TasksWinData *twd = INST_DATA(cl, obj);
        APTR parent;

        twd->twd_TaskList = taskslist;
        twd->twd_TaskText = taskstext;
        twd->twd_TaskCount = taskscount;
        twd->twd_RemoveButton = removeButton;
        twd->twd_PriorityButton = priorityButton;
        twd->twd_MoreButton = moreButton;
        twd->twd_SignalButton = signalButton;
        twd->twd_BreakButton = breakButton;
        twd->twd_FreezeButton = freezeButton;
        twd->twd_ActivateButton = activateButton;
        twd->twd_CPUGauge = cpucount;
        twd->twd_CPUCycle = cpuCycle;
        twd->twd_CPUCheck = (opts.CpuDisplay) ? *opts.CpuDisplay : 0;
        twd->twd_RefreshString = taskstext2;
        twd->twd_TimerHandler.ihn_Object = obj;
        twd->twd_TimerHandler.ihn_Flags = MUIIHNF_TIMER;
        twd->twd_TimerHandler.ihn_Millis = ParseUpdateTime(twd->twd_RefreshString, updatetimetext);
        twd->twd_TimerHandler.ihn_Method = MUIM_TasksWin_CPUCheckQuick;
        twd->twd_TimerHandlerAdded = FALSE;

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);
        get(parent, MUIA_ApplicationObject, &twd->twd_Application);

        set(obj, MUIA_Window_Title, MyGetWindowTitle(txtTasksTitle, twd->twd_Title, sizeof(twd->twd_Title)));
        set(obj, MUIA_Window_DefaultObject, taskslist);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, twd->twd_RemoveButton,
                                                          twd->twd_PriorityButton,
                                                          twd->twd_MoreButton,
                                                          twd->twd_SignalButton,
                                                          twd->twd_BreakButton,
                                                          twd->twd_FreezeButton,
                                                          twd->twd_ActivateButton,
                                                          (clientstate) ? twd->twd_CPUCycle : NULL,
                                                          (clientstate) ? taskstext2 : NULL,
                                                          NULL);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,           MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(taskslist,      MUIM_Notify, MUIA_NList_Active,        MUIV_EveryTime, obj,                     1, MUIM_TasksWin_ListChange);
        DoMethod(taskslist,      MUIM_Notify, MUIA_NList_DoubleClick,   MUIV_EveryTime, obj,                     1, MUIM_TasksWin_More);
        DoMethod(updateButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Update);
        DoMethod(printButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Print);
        DoMethod(removeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Remove);
        DoMethod(moreButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_More);
        DoMethod(priorityButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Priority);
        DoMethod(freezeButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Freeze);
        DoMethod(activateButton, MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Activate);
        DoMethod(signalButton,   MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Signal);
        DoMethod(breakButton,    MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     1, MUIM_TasksWin_Break);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE,          obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
        DoMethod(cpuCycle,       MUIM_Notify, MUIA_Cycle_Active,        MUIV_EveryTime, obj,                     2, MUIM_TasksWin_CPUCheckChange, MUIV_TriggerValue);
        DoMethod(taskstext2,     MUIM_Notify, MUIA_String_Acknowledge,  MUIV_EveryTime, obj,                     3, MUIM_CallHook, &cpuinterval_callhook, MUIV_TriggerValue);
        DoMethod(taskslist,      MUIM_NList_Sort3, MUIV_NList_Sort3_SortType_1, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);
        // DoMethod(taskslist,      MUIM_NList_Sort3, spalte, MUIV_NList_SortTypeAdd_None, MUIV_NList_Sort3_SortType_Both);

        twd->twd_TimerOpen = FALSE;
        if ((twd->twd_TimerPort = CreateMsgPort()) != NULL) {
            if ((twd->twd_TimerIO = (struct TimeRequest *)CreateIORequest(twd->twd_TimerPort, sizeof(struct TimeRequest))) != NULL) {
                if (OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)twd->twd_TimerIO, 0) == 0) {
                    twd->twd_TimerOpen = TRUE;
                }
            }
        }
        if (!twd->twd_TimerOpen) {
            if (twd->twd_TimerIO) {
                DeleteIORequest((struct IORequest *)twd->twd_TimerIO);
                twd->twd_TimerIO = NULL;
            }
            if (twd->twd_TimerPort) {
                DeleteMsgPort(twd->twd_TimerPort);
                twd->twd_TimerPort = NULL;
            }
            CoerceMethod(cl, obj, OM_DISPOSE);
            obj = NULL;
        }
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);

    RemoveCheatTask(&twd->twd_CheatTask);
    if (twd->twd_TimerHandlerAdded) {
        DoMethod(twd->twd_Application, MUIM_Application_RemInputHandler, &twd->twd_TimerHandler);
        twd->twd_TimerHandlerAdded = FALSE;
    }

    set(obj, MUIA_Window_Open, FALSE);
    DoMethod(twd->twd_TaskList, MUIM_NList_Clear);

    if (PatchesInstalled) {
        SwitchState = 0;
        UninstallPatches();
    }
    FreeTaskBuffer();

    if (twd->twd_TimerOpen) {
        CloseDevice((struct IORequest *)twd->twd_TimerIO);
    }
    if (twd->twd_TimerIO) {
        DeleteIORequest((struct IORequest *)twd->twd_TimerIO);
        twd->twd_TimerIO = NULL;
    }
    if (twd->twd_TimerPort) {
        DeleteMsgPort(twd->twd_TimerPort);
        twd->twd_TimerPort = NULL;
    }

    return (DoSuperMethodA(cl, obj, msg));
}

STATIC IPTR mUpdate( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TasksCallbackUserData ud;

    ApplicationSleep(TRUE);
    set(twd->twd_TaskList, MUIA_NList_Quiet, TRUE);
    DoMethod(twd->twd_TaskList, MUIM_NList_Clear);

    ud.ud_List = twd->twd_TaskList;
    ud.ud_Count = 0;

    if (clientstate) {
        ReceiveList(UpdateCallback, &ud);
    } else {
        IterateList(UpdateCallback, &ud, FALSE);
    }

    SetCountText(twd->twd_TaskCount, ud.ud_Count);
    MySetContents(twd->twd_TaskText, "");

    set(twd->twd_TaskList, MUIA_NList_Quiet, FALSE);
    set(twd->twd_TaskList, MUIA_NList_Active, MUIV_NList_Active_Off);
    DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE, twd->twd_RemoveButton,
                                                      twd->twd_PriorityButton,
                                                      twd->twd_MoreButton,
                                                      twd->twd_SignalButton,
                                                      twd->twd_BreakButton,
                                                      twd->twd_FreezeButton,
                                                      twd->twd_ActivateButton,
                                                      NULL);

    DoMethod(obj, MUIM_TasksWin_CPUCheckChange, twd->twd_CPUCheck);

    ApplicationSleep(FALSE);

    return 0;
}

STATIC IPTR mPrint( UNUSED struct IClass *cl,
                     UNUSED Object *obj,
                     UNUSED Msg msg )
{
    PrintTasks(NULL);

    return 0;
}

STATIC IPTR mRemove( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        if (MyRequest(msgYesNo, msgWantToRemoveTask, te->te_Type, te->te_Name)) {
            BOOL withPorts;

            withPorts = MyRequest(msgYesNo, msgWantToRemoveTaskWithStuff, te->te_Name);

            if (MyDoCommand("RemoveTask %s %s", te->te_Address, (withPorts) ? "WITHPORTS" : "") == RETURN_OK) {
                DoMethod(obj, MUIM_TasksWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mPriority( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        LONG pri;

        pri = atol(te->te_Pri);
        if (GetPriority(te->te_Name, &pri)) {
            if (MyDoCommand("SetTaskPri %s %ld", te->te_Address, pri) == RETURN_OK) {
                _snprintf(te->te_Pri, sizeof(te->te_Pri), "%4ld", pri);
                RedrawActiveEntry(twd->twd_TaskList);
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
        struct TasksWinData *twd = INST_DATA(cl, obj);
        struct TaskEntry *te;

        if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
            struct Task *task;

            if ((task = MyFindTask(te->te_Address)) != NULL) {
                APTR detailWin;

                if ((detailWin = (Object *)TasksDetailWindowObject,
                        MUIA_Window_ParentWindow, (IPTR)obj,
                        MUIA_Window_MaxChildWindowCount, (opts.SingleWindows) ? 1 : 0,
                    End) != NULL) {
                    COLLECT_RETURNIDS;
                    set(detailWin, MUIA_TasksDetailWin_Task, te);
                    set(detailWin, MUIA_Window_Open, TRUE);
                    REISSUE_RETURNIDS;
                }
            } else {
                MyRequest(msgErrorContinue, msgCantFindTask);
                DoMethod(obj, MUIM_TasksWin_Update);
            }
        }
    }

    return 0;
}

STATIC IPTR mListChange( struct IClass *cl,
                          Object *obj,
                          UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        MySetContents(twd->twd_TaskText, "%s \"%s\"", te->te_Address, te->te_Name);
        DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, FALSE, twd->twd_RemoveButton,
                                                           twd->twd_PriorityButton,
                                                           twd->twd_SignalButton,
                                                           twd->twd_BreakButton,
                                                           twd->twd_FreezeButton,
                                                           twd->twd_ActivateButton,
                                                           (clientstate) ? NULL : twd->twd_MoreButton,
                                                           (clientstate || amigaOS4) ? NULL : twd->twd_CPUCycle,
                                                           NULL);
    }

    return 0;
}

STATIC IPTR mFreeze( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        if (MyDoCommand("FreezeTask %s", te->te_Address) == RETURN_OK) {
            stccpy(te->te_State, GetTaskState(TS_FROZEN, 0), sizeof(te->te_State));
            RedrawActiveEntry(twd->twd_TaskList);
        }
    }

    return 0;
}

STATIC IPTR mActivate( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        if (MyDoCommand("ActivateTask %s", te->te_Address) == RETURN_OK) {
            stccpy(te->te_State, GetTaskState(TS_WAIT, -1), sizeof(te->te_State));
            RedrawActiveEntry(twd->twd_TaskList);
        }
    }

    return 0;
}

STATIC IPTR mSignal( struct IClass *cl,
                      Object *obj,
                      UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        IPTR sigs;
        APTR sigwin;

        IsUHex(te->te_SigWait, &sigs);
        if ((sigwin = (Object *)SignalWindowObject,
                MUIA_Window_ParentWindow, (IPTR)obj,
            End) != NULL) {
            if (DoMethod(sigwin, MUIM_SignalWin_GetSignals, te->te_Name, &sigs)) {
                if (MyDoCommand("SignalTask %s " ADDRESS_FORMAT, te->te_Address, sigs) == RETURN_OK) {
                    Delay(25);
                    RedrawActiveEntry(twd->twd_TaskList);
                }
            }
        }
    }

    return 0;
}

STATIC IPTR mBreak( struct IClass *cl,
                     Object *obj,
                     UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TaskEntry *te;

    if ((te = (struct TaskEntry *)GetActiveEntry(twd->twd_TaskList)) != NULL) {
        if (MyDoCommand("BreakTask %s", te->te_Address) == RETURN_OK) {
            Delay(25);
            RedrawActiveEntry(twd->twd_TaskList);
        }
    }

    return 0;
}

STATIC IPTR mCPUCheckChange( struct IClass *cl,
                              Object *obj,
                              UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct CPUCheckChangeMessage *cccm = (struct CPUCheckChangeMessage *)msg;

    if (!amigaOS4 || morphOS) {
        twd->twd_CPUCheck = cccm->value;

        if (cccm->value != 0 && !PatchesInstalled) {
            AsmTimerBase = (struct Library *)twd->twd_TimerIO->Request.io_Device;
            SwitchState = 0;
            InstallPatches();
        }

        if (PatchesInstalled) {
            ClearTaskData();
            SwitchState = (char)cccm->value;

            if (cccm->value == 0) {
                if (twd->twd_TimerHandlerAdded) {
                    DoMethod(twd->twd_Application, MUIM_Application_RemInputHandler, &twd->twd_TimerHandler);
                    twd->twd_TimerHandlerAdded = FALSE;
                }
            } else {
                if (!twd->twd_TimerHandlerAdded) {
                    twd->twd_TimerHandler.ihn_Millis = ParseUpdateTime(twd->twd_RefreshString, updatetimetext);
                    DoMethod(twd->twd_Application, MUIM_Application_AddInputHandler, &twd->twd_TimerHandler);
                    twd->twd_TimerHandlerAdded = TRUE;
                }

                if (cccm->value == 2) {
                    if (twd->twd_CheatTask == NULL) {
                        twd->twd_CheatTask = AddCheatTask();
                    }
                }
            }

            if (cccm->value != 2) {
                RemoveCheatTask(&twd->twd_CheatTask);
                set(twd->twd_CPUGauge, MUIA_Gauge_Current, 0);
                set(twd->twd_CPUGauge, MUIA_Gauge_InfoText, MUIX_PT "-----");
            }

            DoMethod(obj, MUIM_TasksWin_CPUCheck);
        }
    }

    return 0;
}

STATIC IPTR mCPUCheck( struct IClass *cl,
                        Object *obj,
                        UNUSED Msg msg )
{
    struct TasksWinData *twd = INST_DATA(cl, obj);
    struct TasksCallbackUserData ud;
    struct Task *task;
    BOOL changed;
    IPTR i, oldTaskCnt = 0, sortCol = 0;

    get(twd->twd_TaskList, MUIA_NList_Entries, &oldTaskCnt);
    get(twd->twd_TaskList, MUIA_NList_SortType, &sortCol);
    sortCol &= MUIV_NList_TitleMark_ColMask;

    ud.ud_List = twd->twd_TaskList;
    ud.ud_Count = oldTaskCnt;

    ClearTaskData();

    while ((task = GetAddedTask()) != NULL) {
        if (TaskExists(task)) {
            GetTaskEntry(task, &twd->twd_NewEntry, (twd->twd_CPUCheck != 0));
            UpdateCallback(&twd->twd_NewEntry, &ud);
        }
    }

    changed = FALSE;

    for (i = 0; ; i++) {
        struct TaskEntry *old;

        DoMethod(twd->twd_TaskList, MUIM_NList_GetEntry, i, &old);

        if (old != NULL) {
            if ((task = TaskExists(old->te_Addr)) != NULL) {
                CopyMemQuick(old, &twd->twd_CompareEntry, sizeof(struct TaskEntry));
                UpdateTaskEntry(task, old, twd->twd_CPUCheck);
                if (memcmp(old, &twd->twd_CompareEntry, sizeof(struct TaskEntry)) != 0) {
                    if (sortCol != 2) DoMethod(twd->twd_TaskList, MUIM_NList_Redraw, i);
                    changed = TRUE;
                }
            } else {
                DoMethod(twd->twd_TaskList, MUIM_NList_Remove, i);
                i--;
                ud.ud_Count--;
            }
        } else {
            break;
        }
    }

    if (ud.ud_Count != oldTaskCnt) SetCountText(twd->twd_TaskCount, ud.ud_Count);
    if (changed && sortCol == 2) DoMethod(twd->twd_TaskList, MUIM_List_Sort);

    if (twd->twd_CPUCheck == 2) {
    #if !defined(__amigaos4__)
        if (totalmicros) {
            IPTR cheatcpu = GetTaskData(twd->twd_CheatTask);

            set(twd->twd_CPUGauge, MUIA_Gauge_Current, UDivMod32(UMult32(totalmicros - cheatcpu, 100), totalmicros));
            set(twd->twd_CPUGauge, MUIA_Gauge_InfoText, "%ld%%");
        }
    #endif
    }

    return 0;
}

STATIC IPTR mCPUCheckQuick( UNUSED struct IClass *cl,
                             Object *obj,
                             UNUSED Msg msg )
{
    if (PatchesInstalled && SwitchState != 0) {
        DoMethod(obj, MUIM_TasksWin_CPUCheck);
    }

    return TRUE;
}

DISPATCHER(TasksWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                       return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                   return (mDispose(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Update:         return (mUpdate(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Print:          return (mPrint(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Remove:         return (mRemove(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Priority:       return (mPriority(cl, obj, (APTR)msg));
        case MUIM_TasksWin_More:           return (mMore(cl, obj, (APTR)msg));
        case MUIM_TasksWin_ListChange:     return (mListChange(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Freeze:         return (mFreeze(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Activate:       return (mActivate(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Signal:         return (mSignal(cl, obj, (APTR)msg));
        case MUIM_TasksWin_Break:          return (mBreak(cl, obj, (APTR)msg));
        case MUIM_TasksWin_CPUCheckChange: return (mCPUCheckChange(cl, obj, (APTR)msg));
        case MUIM_TasksWin_CPUCheck:       return (mCPUCheck(cl, obj, (APTR)msg));
        case MUIM_TasksWin_CPUCheckQuick:  return (mCPUCheckQuick(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

void PrintTasks( STRPTR filename )
{
    BPTR handle;

    if ((handle = HandlePrintStart(filename)) != ZERO) {
        PrintFOneLine(handle, txtTasksPrintHeader);
        IterateList(PrintCallback, (void *)handle, TRUE);
    }

    HandlePrintStop();
}

void SendTaskList( STRPTR UNUSED dummy )
{
    IterateList(SendCallback, NULL, TRUE);
}

APTR MakeTasksWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct TasksWinData), ENTRY(TasksWinDispatcher));
}

