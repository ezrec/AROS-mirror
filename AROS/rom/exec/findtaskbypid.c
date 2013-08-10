#include <exec/execbase.h>
#include <exec/tasks.h>

/*****************************************************************************

    NAME */
#include <proto/exec.h>

	AROS_LH1(struct Task *, FindTaskByPID,

/*  SYNOPSIS */
	AROS_LHA(ULONG, id, D0),

/*  LOCATION */
	struct ExecBase *, SysBase, 166, Exec)

/*  FUNCTION
	Scan through the task lists searching for the task whose
	et_UniqueID field matches.

    INPUTS
	id	-   The task ID to match.

    RESULT
	Address of the Task control structure that matches, or
	NULL otherwise.

    NOTES
    	This function is source-compatible with MorphOS.

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    return (struct Task *)ScanTasks(SCANTAG_FILTER_UNIQUEID, id, TAG_END);

    AROS_LIBFUNC_EXIT
}
