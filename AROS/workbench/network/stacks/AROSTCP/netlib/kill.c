#include <dos/dos.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <utility/hooks.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

#if AROS_SMP
/* Hook function to call in Exec/ScanTasks
 */
AROS_UFH3(IPTR, CheckTaskEntry,
        AROS_UFHA(struct Hook *,    hook, A0),
        AROS_UFHA(APTR,             object, A2),
        AROS_UFHA(APTR,             message, A1))
{
    AROS_USERFUNC_INIT

    pid_t pid = *(pid_t *)(hook->h_Data);
    ULONG exec_sigs = *(ULONG *)message;

    if ((pid_t)object == pid) {
        Signal((struct Task *)object, exec_sigs);
        return TRUE;
    }

    return FALSE;

    AROS_USERFUNC_EXIT
}
#else
static BOOL CheckTask(struct List *tl, pid_t pid)
{
    struct Node *t;
    for (t = tl->lh_Head; t->ln_Succ; t = t->ln_Succ)
	if ((pid_t)t == pid)
	    return TRUE;
    return FALSE;
}
#endif

int kill(pid_t pid, int sigs)
{
    ULONG exec_sigs;
    int task_valid;

    if ((pid == 0) || (sigs < 0))
	return ESRCH;
    switch (sigs) {
    case SIGTERM:
	exec_sigs = SIGBREAKF_CTRL_C;
    case 0:
	exec_sigs = 0;
	break;
    default:
	return EINVAL;
    }

#if AROS_SMP
    if (exec_sigs) {
        struct Hook hook = {
            .h_Entry = (APTR)CheckTaskEntry,
            .h_Data = &pid
        };

        task_valid = ScanTasks(&hook, &exec_sigs);
    } while (0);
#else
    Forbid();

    /*
     * The task can be:
     * a) Current one
     * b) In the TaskReady list
     * c) In the TaskWait list
     */
    task_valid = ((pid == (pid_t)FindTask(NULL)) ||
    		  CheckTask(&SysBase->TaskReady, pid) ||
    		  CheckTask(&SysBase->TaskWait, pid));

    if (task_valid && exec_sigs)
	Signal((struct Task *)pid, exec_sigs);

    Permit();
#endif

    return task_valid ? 0 : ESRCH;
}

