/*
    Copyright © 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Search for the next task
    Lang: english
*/
#include <string.h>     /* for strcmp */

#include <exec/execbase.h>
#include <exec/cpu.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#include <proto/arossupport.h>

#include "exec_intern.h"
    
struct ScanTaskFilter {
    struct TagItem *task, *pri, *name, *cpu, *uniqueid;
    struct TagItem *state;
};

static BOOL ScanTask_Filter(struct Task *task, struct ScanTaskFilter *filter);

/*****************************************************************************

    NAME */

        AROS_LH1(IPTR, ScanTasksA,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, tags, A0),

/*  LOCATION */
        struct ExecBase *, SysBase, 186, Exec)

/*  FUNCTION
        Apply the SCANTAG_* tags to the tasks in the
        the ThisTask, TaskReady, and TaskWait SysBase lists.

    INPUTS
        SCANTAG_HOOK:
            The hook function's 'object' is the struct Task *
            The hook function's 'message' is NULL, or the value
                from SCANTAG_HOOK_MESSAGE
            The hook should return (IPTR)0 to continue processing
            tasks, or a non-zero value to stop processing.
            The non-zero value will be returned by ScanTasksA()
        SCANTAG_FILTER_TASK:
            If present, the struct Task * must match the tag value.
            Useful for verifying that a task is known to the scheduler.
        SCANTAG_FILTER_NAME:
            If present, the task's name must match the tag value.
        SCANTAG_FILTER_PRI:
            If present, the task's priority must match the tag value.
        SCANTAG_FILTER_CPU:
            If present, the task must be scheduled to the CPU specified.
        SCANTAG_FILTER_STATE:
            If present, the task's tc_State must match the tag value.

    RESULT
        If SCANTAG_HOOK is not present, returns the first tag that
        matches the filter.

        If SCANTAG_HOOK is present, then the first non-zero value 
        returned by the hook function, or zero.

    NOTES
        The hook will be called in Disable() context

        Filtering always begins on the current task's CPU's scheduler,
        then proceeds around all CPUs. For example, of a 4 CPU system,
        if the current task was on CPU2, the filter would search
        CPU2 first, then CPU3, then CPU0, and finally CPU1.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Task *task;
    IPTR done = 0;
    struct Hook *hook;
    APTR message;
    struct ScanTaskFilter filter;

    hook = (struct Hook *)LibGetTagData(SCANTAG_HOOK, (IPTR)NULL, tags);
    message = (APTR)LibGetTagData(SCANTAG_HOOK_MESSAGE, (IPTR)NULL, tags);

    filter.task = LibFindTagItem(SCANTAG_FILTER_TASK, tags);
    filter.pri  = LibFindTagItem(SCANTAG_FILTER_PRI, tags);
    filter.name = LibFindTagItem(SCANTAG_FILTER_NAME, tags);
    filter.cpu  = LibFindTagItem(SCANTAG_FILTER_CPU, tags);
    filter.uniqueid  = LibFindTagItem(SCANTAG_FILTER_UNIQUEID, tags);
    filter.state = LibFindTagItem(SCANTAG_FILTER_STATE, tags);

    Disable();
    /* Quick return for a quick argument */
#if AROS_SMP
    struct ExecCPUInfo *ThisCPU;
    ForeachNode(&PrivExecBase(SysBase)->CPUList, ThisCPU) {
#else
    do {
        struct ExecBase *ThisCPU = SysBase;
#endif

        /* Call on the running task */
        if (ThisCPU->ThisTask && ScanTask_Filter(ThisCPU->ThisTask, &filter)) {
            done = hook ? CALLHOOKPKT(hook, ThisCPU->ThisTask, message) : (IPTR)ThisCPU->ThisTask;
            if (done)
                break;
        }

        /* Now look into the ready list. */
        ForeachNode(&ThisCPU->TaskReady, task) {
            if (ScanTask_Filter(task, &filter)) {
                done = hook ? CALLHOOKPKT(hook, task, message) : (IPTR)task;
                if (done)
                    break;
            }
        }

        if (done)
            break;

        /* Now inspect the waiting list. */
        ForeachNode(&ThisCPU->TaskWait, task) {
            if (ScanTask_Filter(task, &filter)) {
                done = hook ? CALLHOOKPKT(hook, task, message) : (IPTR)task;
                if (done)
                    break;
            }
        }

        if (done)
            break;

#if AROS_SMP
    }
#else
    /* Nothing to do */
    } while (0);
#endif

    /* Return whatever I found. */
    Enable();
    return done;
    AROS_LIBFUNC_EXIT
} /* FindTask */

static BOOL ScanTask_Filter(struct Task *task, struct ScanTaskFilter *filter)
{
    BOOL found = TRUE;
#if AROS_SMP
    ULONG cpu = GetETask(task)->et_SysCPU->ec_CPUNumber;
#else
    ULONG cpu = 0;
#endif

    if (task == NULL)
        return FALSE;

    if (found && filter->task)
        found = ((struct Task *)filter->task->ti_Data == task);

    if (found && filter->name)
        found = (strcmp((const char *)filter->name->ti_Data, task->tc_Node.ln_Name) == 0) ? TRUE : FALSE;

    if (found && filter->pri)
        found = ((BYTE)filter->pri->ti_Data == task->tc_Node.ln_Pri) ? TRUE : FALSE;

    if (found && filter->uniqueid) {
        ULONG uniqueid = GetETaskID(task);
        found = ((ULONG)filter->uniqueid->ti_Data == uniqueid) ? TRUE : FALSE;
    }

    if (found && filter->cpu)
        found = ((ULONG)filter->cpu->ti_Data == cpu);

    if (found && filter->state)
        found = ((UBYTE)filter->state->ti_Data == task->tc_State) ? TRUE : FALSE;

    return found;
}
